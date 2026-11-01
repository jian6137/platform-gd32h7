#include "audio_player.h"
#include "gd32h7xx.h"
#include <string.h>

/* Ring buffer size: 8,192 16-bit samples (16 KB in RAM).
   Must be a power of 2 for fast bitwise wrap-around.
   At 44.1 kHz stereo, this holds ~93ms of audio (~6x a 16.6ms video frame). */
#define AUDIO_RING_SAMPLES      8192U
#define AUDIO_RING_MASK         (AUDIO_RING_SAMPLES - 1U)
#define AUDIO_REFILL_BATCH      1024U   /* Refill in 1024-sample (2KB) batches to guarantee zero overflow */

__attribute__((aligned(32))) static int16_t s_audio_ring[AUDIO_RING_SAMPLES];
static volatile uint32_t s_ring_head = 0U; /* Producer (FatFs f_read or synth) */
static volatile uint32_t s_ring_tail = 0U; /* Consumer (SPI1_IRQHandler) */
static volatile uint64_t s_samples_played = 0U;

/* Temporary read buffer for FatFs block reads (strictly sized to AUDIO_REFILL_BATCH) */
__attribute__((aligned(32))) static int16_t s_file_read_buf[AUDIO_REFILL_BATCH];

static FIL s_wav_file;
static audio_wav_info_t s_wav_info;
static bool s_file_open = false;
static bool s_is_active = false;
static bool s_is_synth = false;
static uint32_t s_audio_file_pos = 0U;
static uint32_t s_synth_phase_left = 0U;
static uint32_t s_synth_phase_right = 0U;
static const char *s_last_error = "None";
static volatile uint8_t s_volume_percent = 80U; /* Default 80% volume */
static volatile uint32_t s_volume_scale = 205U;  /* (80 * 256 + 50) / 100 = 205 */

/* Precomputed 100-point full-scale sine table (at 44.1kHz, 100 samples = exactly 441.0 Hz Concert A chord) */
static const int16_t s_sine_lut[100] = {
         0,   1506,   3007,   4497,   5968,   7416,   8834,  10218,  11562,  12859,
     14106,  15298,  16429,  17495,  18492,  19416,  20263,  21031,  21715,  22314,
     22825,  23245,  23574,  23810,  23952,  24000,  23952,  23810,  23574,  23245,
     22825,  22314,  21715,  21031,  20263,  19416,  18492,  17495,  16429,  15298,
     14106,  12859,  11562,  10218,   8834,   7416,   5968,   4497,   3007,   1506,
         0,  -1506,  -3007,  -4497,  -5968,  -7416,  -8834, -10218, -11562, -12859,
    -14106, -15298, -16429, -17495, -18492, -19416, -20263, -21031, -21715, -22314,
    -22825, -23245, -23574, -23810, -23952, -24000, -23952, -23810, -23574, -23245,
    -22825, -22314, -21715, -21031, -20263, -19416, -18492, -17495, -16429, -15298,
    -14106, -12859, -11562, -10218,  -8834,  -7416,  -5968,  -4497,  -3007,  -1506,
};

#pragma pack(push, 1)
typedef struct {
    uint32_t chunk_id;     /* "RIFF" */
    uint32_t chunk_size;
    uint32_t format;       /* "WAVE" */
} riff_header_t;

typedef struct {
    uint32_t subchunk_id;  /* "fmt " */
    uint32_t subchunk_size;
    uint16_t audio_format; /* 1 = PCM */
    uint16_t num_channels; /* 1 = Mono, 2 = Stereo */
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} fmt_chunk_t;
#pragma pack(pop)

/*!
    \brief      SPI1 Interrupt Service Routine (handles I2S transmit FIFO empty)
                Matches the proven architecture of GD32H759I official 12_HPDF_I2S_Audio demo.
*/
void SPI1_IRQHandler(void)
{
    if(!s_is_active) {
        spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TP);
        nvic_irq_disable(SPI1_IRQn);
        NVIC_ClearPendingIRQ(SPI1_IRQn);
        return;
    }

    if(RESET != spi_i2s_interrupt_flag_get(SPI1, SPI_I2S_INT_FLAG_TP)) {
        if(s_ring_head != s_ring_tail) {
            int16_t sample = s_audio_ring[s_ring_tail];
            s_ring_tail = (s_ring_tail + 1U) & AUDIO_RING_MASK;
            int32_t scaled = ((int32_t)sample * (int32_t)s_volume_scale) >> 8;
            if(scaled > 32767) {
                scaled = 32767;
            } else if(scaled < -32768) {
                scaled = -32768;
            }
            spi_i2s_data_transmit(SPI1, (uint32_t)(uint16_t)(int16_t)scaled);
            s_samples_played++;
        } else {
            /* Ring buffer underrun: output silence smoothly without hanging */
            spi_i2s_data_transmit(SPI1, 0U);
        }
    }

    /* Clear transmit underrun error flag if set to ensure interrupt does not loop */
    if(RESET != spi_i2s_flag_get(SPI1, SPI_FLAG_TXURERR)) {
        spi_i2s_flag_clear(SPI1, SPI_STATC_TXURERRC);
    }
}

/*!
    \brief      Helper to read samples from WAV (or generate synthetic sine wave) into ring buffer
*/
static uint32_t refill_ring_buffer(uint32_t max_samples)
{
    if(!s_file_open || max_samples == 0U) {
        return 0U;
    }

    /* In synthetic test tone mode, stream sine wave directly into ring buffer */
    if(s_is_synth) {
        uint32_t pairs = max_samples / 2U;
        for(uint32_t i = 0U; i < pairs; ++i) {
            int16_t left = s_sine_lut[s_synth_phase_left];
            int16_t right = s_sine_lut[s_synth_phase_right];
            s_synth_phase_left = (s_synth_phase_left + 1U) % 100U;
            s_synth_phase_right = (s_synth_phase_right + 2U) % 100U;

            s_audio_ring[s_ring_head] = left;
            s_ring_head = (s_ring_head + 1U) & AUDIO_RING_MASK;
            s_audio_ring[s_ring_head] = right;
            s_ring_head = (s_ring_head + 1U) & AUDIO_RING_MASK;
        }
        return pairs * 2U;
    }

    /* Guard against buffer overflow: strictly cap each read to AUDIO_REFILL_BATCH */
    if(max_samples > AUDIO_REFILL_BATCH) {
        max_samples = AUDIO_REFILL_BATCH;
    }

    uint32_t samples_added = 0U;

    if(s_wav_info.num_channels == 2U) {
        /* Stereo read: guarantee 4-byte alignment for 16-bit L+R stereo pairs */
        uint32_t bytes_needed = (max_samples * 2U) & ~3U;
        uint32_t remaining = (s_wav_info.data_bytes > s_audio_file_pos) ? (s_wav_info.data_bytes - s_audio_file_pos) : 0U;
        uint32_t to_read = (bytes_needed < remaining) ? bytes_needed : remaining;
        to_read &= ~3U;

        UINT bytes_read = 0U;
        if(to_read > 0U) {
            f_read(&s_wav_file, s_file_read_buf, to_read, &bytes_read);
            s_audio_file_pos += bytes_read;
        }

        /* Loop back to audio data start if EOF reached */
        if(bytes_read < bytes_needed) {
            f_lseek(&s_wav_file, s_wav_info.data_offset);
            s_audio_file_pos = 0U;
            UINT wrap_read = 0U;
            uint32_t wrap_needed = (bytes_needed - bytes_read) & ~3U;
            if(wrap_needed > 0U) {
                f_read(&s_wav_file, ((uint8_t *)s_file_read_buf) + bytes_read,
                       wrap_needed, &wrap_read);
                s_audio_file_pos += wrap_read;
                bytes_read += wrap_read;
            }
        }

        uint32_t samples_read = (bytes_read / 2U) & ~1U; /* Exact even sample count (L/R pairs) */
        for(uint32_t i = 0U; i < samples_read; ++i) {
            s_audio_ring[s_ring_head] = s_file_read_buf[i];
            s_ring_head = (s_ring_head + 1U) & AUDIO_RING_MASK;
        }
        samples_added = samples_read;
    } else {
        /* Mono -> Stereo expansion (L = R) */
        uint32_t mono_samples_needed = max_samples / 2U;
        uint32_t bytes_needed = mono_samples_needed * 2U;
        uint32_t remaining = (s_wav_info.data_bytes > s_audio_file_pos) ? (s_wav_info.data_bytes - s_audio_file_pos) : 0U;
        uint32_t to_read = (bytes_needed < remaining) ? bytes_needed : remaining;

        UINT bytes_read = 0U;
        if(to_read > 0U) {
            f_read(&s_wav_file, s_file_read_buf, to_read, &bytes_read);
            s_audio_file_pos += bytes_read;
        }

        if(bytes_read < bytes_needed) {
            f_lseek(&s_wav_file, s_wav_info.data_offset);
            s_audio_file_pos = 0U;
            UINT wrap_read = 0U;
            f_read(&s_wav_file, ((uint8_t *)s_file_read_buf) + bytes_read,
                   bytes_needed - bytes_read, &wrap_read);
            s_audio_file_pos += wrap_read;
            bytes_read += wrap_read;
        }

        uint32_t mono_count = bytes_read / 2U;
        for(uint32_t i = 0U; i < mono_count; ++i) {
            int16_t sample = s_file_read_buf[i];
            s_audio_ring[s_ring_head] = sample;
            s_ring_head = (s_ring_head + 1U) & AUDIO_RING_MASK;
            s_audio_ring[s_ring_head] = sample;
            s_ring_head = (s_ring_head + 1U) & AUDIO_RING_MASK;
        }
        samples_added = mono_count * 2U;
    }

    return samples_added;
}

void audio_player_init(void)
{
    /* Set NVIC priority grouping to PRE1_SUB3 (matching official GD32 EVAL demo) */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    /* Enable GPIO clocks for remapped I2S1 pins (GPIOB, GPIOC, GPIOG) */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* 
       Remapped I2S1 pins with GPIO_AF_5:
       - PC6  (AF5) -> I2S1_MCK (Direct trace on PCB to CS4344 MCLK via JP63 pins 2-3)
       - PB10 (AF5) -> I2S1_CK  (Exposed on JP8 Pin 35 -> wire to JP58 Pin 3 for SCLK)
       - PB12 (AF5) -> I2S1_WS  (Exposed on JP9 Pin 2  -> wire to JP54 Pin 3 for LRCK)
       - PG3  (AF5) -> I2S1_SD  (Exposed on JP9 Pin 19 -> wire to JP40 Pin 3 for SDIN)

       NOTE: PD3 (LCD_G7), PB9 (LCD_B7), and PC3 (SDCKE0) are completely untouched!
       Onboard jumper shunts remain in default peripheral positions:
       - JP58: Pins (1, 2) [LCD_G7 preserved - full green colors]
       - JP54: Pins (1, 2) [LCD_B7 preserved - full blue colors]
       - JP40: Pins (1, 2) [SDCKE0 preserved - SDRAM functional]
       - JP63: Pins (2, 3) [PC6 -> CS4344 MCLK]
    */
    /* PC6 -> I2S1_MCK (High-speed 85MHz for crisp master clock) */
    gpio_af_set(GPIOC, GPIO_AF_5, GPIO_PIN_6);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_6);

    /* PB10 -> I2S1_CK (Bit Clock on JP8 Pin 35) */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_10);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_10);

    /* PB12 -> I2S1_WS (Word Select / LRCK on JP9 Pin 2) */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_12);

    /* PG3 -> I2S1_SD (Serial Data on JP9 Pin 19) */
    gpio_af_set(GPIOG, GPIO_AF_5, GPIO_PIN_3);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_3);

    /* Explicitly enable PLL0Q clock output for SPI1/I2S1 peripheral */
    rcu_pll_clock_output_enable(RCU_PLL0Q);

    /* Enable SPI1 peripheral clock and configure clock source to PLL0Q */
    rcu_periph_clock_enable(RCU_SPI1);
    rcu_spi_clock_config(IDX_SPI1, RCU_SPISRC_PLL0Q);

    /* Ensure SPI1 is in reset state with interrupts disabled until playback starts */
    spi_i2s_deinit(SPI1);
    spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TP);
    nvic_irq_disable(SPI1_IRQn);
    NVIC_ClearPendingIRQ(SPI1_IRQn);
}

bool audio_player_open(const char *filename)
{
    s_last_error = "OK";
    s_is_synth = false;

    FRESULT res = FR_NO_FILE;
    if(filename != NULL && filename[0] != '\0') {
        res = f_open(&s_wav_file, filename, FA_READ);
    }

    if(res != FR_OK) {
        /* Fallback: try alternate common WAV filenames on SD card */
        static const char * const alt_names[] = {
            "0:/song.wav", "0:/SONG.WAV", "song.wav", "SONG.WAV",
            "0:/music.wav", "0:/MUSIC.WAV", "0:/audio.wav", "0:/AUDIO.WAV"
        };
        for(uint32_t i = 0U; i < sizeof(alt_names) / sizeof(alt_names[0]); ++i) {
            if(filename != NULL && strcmp(filename, alt_names[i]) == 0) {
                continue;
            }
            res = f_open(&s_wav_file, alt_names[i], FA_READ);
            if(res == FR_OK) {
                break;
            }
        }
    }

    if(res != FR_OK) {
        /* No WAV file found on SD card: activate SYNTHETIC SINE TONE so user immediately hears audio! */
        return audio_player_open_synth(44100U);
    }

    riff_header_t riff;
    UINT br = 0U;
    res = f_read(&s_wav_file, &riff, sizeof(riff), &br);
    if(res != FR_OK || br != sizeof(riff)) {
        f_close(&s_wav_file);
        return audio_player_open_synth(44100U);
    }

    /* Check "RIFF" (0x46464952) and "WAVE" (0x45564157) */
    if(riff.chunk_id != 0x46464952U || riff.format != 0x45564157U) {
        f_close(&s_wav_file);
        return audio_player_open_synth(44100U);
    }

    bool found_fmt = false;
    bool found_data = false;
    memset(&s_wav_info, 0, sizeof(s_wav_info));

    /* Parse subchunks with RIFF 2-byte word padding */
    while(!f_eof(&s_wav_file)) {
        uint32_t chunk_header[2];
        res = f_read(&s_wav_file, chunk_header, 8U, &br);
        if(res != FR_OK || br != 8U) {
            break;
        }

        uint32_t sub_id = chunk_header[0];
        uint32_t sub_size = chunk_header[1];
        uint32_t padded_size = sub_size + (sub_size & 1U); /* RIFF chunks are 2-byte aligned */

        if(sub_id == 0x20746D66U) { /* "fmt " */
            uint8_t fmt_buf[40];
            uint32_t read_bytes = (sub_size < sizeof(fmt_buf)) ? sub_size : sizeof(fmt_buf);
            f_read(&s_wav_file, fmt_buf, read_bytes, &br);
            if(read_bytes < 16U) {
                break;
            }

            uint16_t format_tag = *(uint16_t *)&fmt_buf[0];
            uint16_t channels   = *(uint16_t *)&fmt_buf[2];
            uint32_t srate      = *(uint32_t *)&fmt_buf[4];
            uint16_t bps        = *(uint16_t *)&fmt_buf[14];

            /* Support standard PCM (1) and WAVE_FORMAT_EXTENSIBLE (0xFFFE) */
            bool is_pcm = (format_tag == 1U);
            if(format_tag == 0xFFFEU && read_bytes >= 26U) {
                uint16_t sub_tag = *(uint16_t *)&fmt_buf[24];
                if(sub_tag == 1U) {
                    is_pcm = true;
                }
            }

            if(!is_pcm || (channels != 1U && channels != 2U) || bps != 16U) {
                f_close(&s_wav_file);
                return audio_player_open_synth(44100U);
            }

            s_wav_info.sample_rate = srate;
            s_wav_info.num_channels = channels;
            s_wav_info.bits_per_sample = bps;
            found_fmt = true;

            if(padded_size > read_bytes) {
                f_lseek(&s_wav_file, f_tell(&s_wav_file) + (padded_size - read_bytes));
            }
        } else if(sub_id == 0x61746164U) { /* "data" */
            s_wav_info.data_bytes = sub_size;
            s_wav_info.data_offset = f_tell(&s_wav_file);
            found_data = true;
            break;
        } else {
            /* Skip unrecognized subchunk (e.g. LIST, JUNK, ID3) with 2-byte alignment */
            f_lseek(&s_wav_file, f_tell(&s_wav_file) + padded_size);
        }
    }

    if(!found_fmt || !found_data || s_wav_info.sample_rate == 0U) {
        f_close(&s_wav_file);
        return audio_player_open_synth(44100U);
    }

    uint32_t bytes_per_sec = s_wav_info.sample_rate * (uint32_t)s_wav_info.num_channels * 2U;
    s_wav_info.duration_ms = (bytes_per_sec > 0U) ? (uint32_t)(((uint64_t)s_wav_info.data_bytes * 1000U) / bytes_per_sec) : 0U;

    s_file_open = true;
    s_audio_file_pos = 0U;
    s_samples_played = 0U;
    s_ring_head = 0U;
    s_ring_tail = 0U;

    /* Seek to audio data */
    f_lseek(&s_wav_file, s_wav_info.data_offset);

    /* Safely pre-fill ring buffer in batches of AUDIO_REFILL_BATCH (zero buffer overflow) */
    for(uint32_t b = 0U; b < 3U; ++b) {
        refill_ring_buffer(AUDIO_REFILL_BATCH);
    }

    /* Configure I2S prescaler and protocol for Cirrus Logic CS4344-CZZR DAC:
       - CS4344 is hardwired strictly to Philips I2S (I2S_STD_PHILIPS)
       - Low clock polarity (I2S_CKPL_LOW)
       - 16-bit frame format (I2S_FRAMEFORMAT_DT16B_CH16B)
       - Master clock output enabled (I2S_MCKOUT_ENABLE) producing 256*Fs on PC6
    */
    spi_i2s_deinit(SPI1);
    i2s_psc_config(SPI1, s_wav_info.sample_rate, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_PHILIPS, I2S_CKPL_LOW);

    return true;
}

bool audio_player_open_synth(uint32_t sample_rate)
{
    if(sample_rate == 0U) {
        sample_rate = 44100U;
    }

    s_is_synth = true;
    s_file_open = true;
    s_last_error = "Synthetic Sine Tone (441Hz A4 chord)";

    s_wav_info.sample_rate = sample_rate;
    s_wav_info.num_channels = 2U;
    s_wav_info.bits_per_sample = 16U;
    s_wav_info.data_bytes = 0xFFFFFFFFU;
    s_wav_info.data_offset = 0U;
    s_wav_info.duration_ms = 999999U;

    s_audio_file_pos = 0U;
    s_samples_played = 0U;
    s_ring_head = 0U;
    s_ring_tail = 0U;
    s_synth_phase_left = 0U;
    s_synth_phase_right = 0U;

    /* Pre-fill ring buffer with synthetic sine wave in safe batches */
    for(uint32_t b = 0U; b < 3U; ++b) {
        refill_ring_buffer(AUDIO_REFILL_BATCH);
    }

    /* Configure I2S prescaler and protocol for Cirrus Logic CS4344-CZZR DAC */
    spi_i2s_deinit(SPI1);
    i2s_psc_config(SPI1, s_wav_info.sample_rate, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_ENABLE);
    i2s_init(SPI1, I2S_MODE_MASTERTX, I2S_STD_PHILIPS, I2S_CKPL_LOW);

    return true;
}

bool audio_player_is_synth(void)
{
    return s_is_synth;
}

void audio_player_start(void)
{
    if(!s_file_open) {
        return;
    }

    s_is_active = true;

    /* Prime transmit FIFO with first sample before starting clock to prevent underrun */
    if(s_ring_head != s_ring_tail) {
        int16_t sample = s_audio_ring[s_ring_tail];
        s_ring_tail = (s_ring_tail + 1U) & AUDIO_RING_MASK;
        int32_t scaled = ((int32_t)sample * (int32_t)s_volume_scale) >> 8;
        if(scaled > 32767) {
            scaled = 32767;
        } else if(scaled < -32768) {
            scaled = -32768;
        }
        spi_i2s_data_transmit(SPI1, (uint32_t)(uint16_t)(int16_t)scaled);
        s_samples_played++;
    } else {
        spi_i2s_data_transmit(SPI1, 0U);
    }

    /* Enable NVIC interrupt for SPI1 */
    NVIC_ClearPendingIRQ(SPI1_IRQn);
    nvic_irq_enable(SPI1_IRQn, 0, 1);

    /* Enable I2S1 peripheral and start master clock/transfer */
    i2s_enable(SPI1);
    spi_master_transfer_start(SPI1, SPI_TRANS_START);

    /* Enable SPI/I2S Transmit FIFO empty interrupt */
    spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_TP);
}

void audio_player_stop(void)
{
    if(!s_is_active) {
        return;
    }

    s_is_active = false;

    spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TP);
    nvic_irq_disable(SPI1_IRQn);
    NVIC_ClearPendingIRQ(SPI1_IRQn);

    spi_master_transfer_start(SPI1, SPI_TRANS_IDLE);
    i2s_disable(SPI1);
}

void audio_player_service(void)
{
    if(!s_is_active || !s_file_open) {
        return;
    }

    /* Calculate free space in ring buffer */
    uint32_t head = s_ring_head;
    uint32_t tail = s_ring_tail;
    uint32_t used = (head >= tail) ? (head - tail) : (AUDIO_RING_SAMPLES - (tail - head));
    uint32_t free_samples = (AUDIO_RING_SAMPLES - 1U) - used;

    /* Refill in chunks when free space exceeds refill batch threshold */
    if(free_samples >= AUDIO_REFILL_BATCH) {
        refill_ring_buffer(AUDIO_REFILL_BATCH);
    }
}

void audio_player_rewind(void)
{
    if(!s_file_open) {
        return;
    }

    /* Disable interrupt temporarily to safely reset ring buffer */
    spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TP);

    f_lseek(&s_wav_file, s_wav_info.data_offset);
    s_audio_file_pos = 0U;
    s_samples_played = 0U;
    s_ring_head = 0U;
    s_ring_tail = 0U;

    /* Pre-fill ring buffer with fresh audio data from start of file */
    for(uint32_t b = 0U; b < 3U; ++b) {
        refill_ring_buffer(AUDIO_REFILL_BATCH);
    }

    if(s_is_active) {
        spi_i2s_interrupt_enable(SPI1, SPI_I2S_INT_TP);
    }
}

bool audio_player_is_active(void)
{
    return s_is_active;
}

uint32_t audio_player_get_time_ms(void)
{
    if(!s_is_active || s_wav_info.sample_rate == 0U) {
        return 0U;
    }

    /* 2 samples (Left + Right) = 1 audio frame in stereo */
    uint64_t frames_played = (s_wav_info.num_channels == 2U) ? (s_samples_played / 2U) : s_samples_played;
    return (uint32_t)((frames_played * 1000ULL) / s_wav_info.sample_rate);
}

const audio_wav_info_t *audio_player_get_info(void)
{
    return &s_wav_info;
}

const char *audio_player_get_last_error(void)
{
    return s_last_error;
}

uint64_t audio_player_get_samples_played(void)
{
    return s_samples_played;
}

void audio_player_set_volume(uint8_t volume_percent)
{
    if(volume_percent > 100U) {
        volume_percent = 100U;
    }
    s_volume_percent = volume_percent;
    /* Scale factor: (volume * 256 + 50) / 100 ensures exact 256 for 100% and 0 for 0% */
    s_volume_scale = ((uint32_t)volume_percent * 256U + 50U) / 100U;
}

uint8_t audio_player_get_volume(void)
{
    return s_volume_percent;
}

void audio_player_volume_up(void)
{
    if(s_volume_percent + 5U >= 100U) {
        audio_player_set_volume(100U);
    } else {
        audio_player_set_volume(s_volume_percent + 5U);
    }
}

void audio_player_volume_down(void)
{
    if(s_volume_percent <= 5U) {
        audio_player_set_volume(0U);
    } else {
        audio_player_set_volume(s_volume_percent - 5U);
    }
}



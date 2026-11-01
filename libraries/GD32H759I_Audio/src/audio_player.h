#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t sample_rate;
    uint16_t num_channels;
    uint16_t bits_per_sample;
    uint32_t data_bytes;
    uint32_t data_offset;
    uint32_t duration_ms;
} audio_wav_info_t;

/*!
    \brief      Initialize audio player hardware (GPIOs, RCU clocks, DMA, SPI1/I2S1)
*/
void audio_player_init(void);

/*!
    \brief      Open and parse a WAV file from FatFs (with fallback to synthetic mode)
    \param[in]  filename: path to the WAV file (e.g. "0:/song.wav")
    \retval     true if audio playback is ready (WAV file or synthetic tone)
*/
bool audio_player_open(const char *filename);

/*!
    \brief      Open synthetic audio tone generator (for instant verification without WAV file)
    \param[in]  sample_rate: sample rate in Hz (e.g. 44100)
    \retval     true if synthetic mode initialized
*/
bool audio_player_open_synth(uint32_t sample_rate);

/*!
    \brief      Check if currently playing synthetic tone generator
    \retval     true if in synthetic mode, false if playing real WAV file
*/
bool audio_player_is_synth(void);

/*!
    \brief      Start audio DMA streaming
*/
void audio_player_start(void);

/*!
    \brief      Stop audio DMA streaming
*/
void audio_player_stop(void);

/*!
    \brief      Service the audio DMA buffers (must be called frequently in main loop)
*/
void audio_player_service(void);

/*!
    \brief      Seek audio playback to the start of audio data (for looping)
*/
void audio_player_rewind(void);

/*!
    \brief      Check if audio playback is currently active
    \retval     true if audio is active
*/
bool audio_player_is_active(void);

/*!
    \brief      Get current audio playback elapsed time in milliseconds
    \retval     Elapsed time in milliseconds since start/rewind
*/
uint32_t audio_player_get_time_ms(void);

/*!
    \brief      Get information about the currently opened WAV file
    \retval     Pointer to audio_wav_info_t struct
*/
const audio_wav_info_t *audio_player_get_info(void);

/*!
    \brief      Get description of last audio error
    \retval     Pointer to error string
*/
const char *audio_player_get_last_error(void);

/*!
    \brief      Get total count of 16-bit audio samples transmitted via I2S
    \retval     64-bit sample count
*/
uint64_t audio_player_get_samples_played(void);

/*!
    \brief      Set master digital audio volume
    \param[in]  volume_percent: volume from 0 (mute) to 100 (full scale)
*/
void audio_player_set_volume(uint8_t volume_percent);

/*!
    \brief      Get current master digital audio volume
    \retval     Volume percent (0 to 100)
*/
uint8_t audio_player_get_volume(void);

/*!
    \brief      Increment audio volume by 5% (up to 100%)
*/
void audio_player_volume_up(void);

/*!
    \brief      Decrement audio volume by 5% (down to 0%)
*/
void audio_player_volume_down(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_PLAYER_H */


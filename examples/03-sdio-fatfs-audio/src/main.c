#include "gd32h7xx.h"
#include "gd32h759i_eval.h"
#include "systick.h"
#include "exmc_sdram.h"
#include "gd32h759i_lcd.h"
#include "lcd_console.h"
#include "sdcard.h"
#include "ff.h"
#include "audio_player.h"
#include "gd25x512me.h"
#include "at24cxx.h"
#include "dci_ov2640.h"
#include <stdio.h>

static FATFS s_fs;

int main(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    systick_config();
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_com_init(EVAL_COM0);

    exmc_sdram_config();
    gd32h759i_lcd_init();

    lcd_console_printf("GD32H759I-EVAL Multimedia & Storage Demo\n");

    /* Initialize I2S1/SPI1 Audio Output and prepare 44.1 kHz synthetic tone fallback */
    audio_player_init();
    audio_player_open_synth(44100U);
    lcd_console_printf("Audio I2S1 initialized (44.1 kHz)\n");

    /* Configure SDIO0 pins and mount FatFs volume */
    sdio_gpio_config();
    FRESULT res = f_mount(&s_fs, "0:", 1);
    if (res == FR_OK) {
        lcd_console_printf("MicroSD mounted via SDIO0 + FatFs!\n");
        DIR dir;
        FILINFO fno;
        if (f_opendir(&dir, "0:/") == FR_OK) {
            while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0) {
                lcd_console_printf("  %s (%lu B)\n", fno.fname, (unsigned long)fno.fsize);
            }
            f_closedir(&dir);
        }
    } else {
        lcd_console_printf("f_mount returned %d (insert FAT32 MicroSD)\n", (int)res);
    }

    while (1) {
        gd_eval_led_toggle(LED1);
        delay_1ms(500);
    }
}

#include "gd32h7xx.h"
#include "gd32h759i_eval.h"
#include "systick.h"
#include "gd32h759i_lcd.h"
#include "lcd_console.h"
#include "bsp_ts_gt911.h"
#include <stdio.h>

int main(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    systick_config();
    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM0);

    /* Initialize 480x272 RGB565 TLI LCD + On-Screen Console */
    gd32h759i_lcd_init();

    lcd_console_printf("GD32H759I-EVAL LCD & GT911 Touch Demo\n");
    lcd_console_printf("CPU Clock: %lu MHz\n", (unsigned long)(SystemCoreClock / 1000000UL));

    /* Initialize GT911 capacitive touch controller */
    if (SUCCESS == gt911_init()) {
        lcd_console_printf("GT911 initialized (ID: 0x%08lX)\n", (unsigned long)gt911_read_id());
    } else {
        lcd_console_printf("GT911 init failed (check JP13=I2C)\n");
    }

    lcd_console_printf("Touch the screen to draw & log coordinates...\n");

    while (1) {
        if (SUCCESS == gt911_handler() && gt911.pressed) {
            uint16_t tx = gt911.x[0];
            uint16_t ty = gt911.y[0];
            gd32h759i_lcd_fill_rect((tx > 2) ? (tx - 2) : 0,
                                    (ty > 2) ? (ty - 2) : 0,
                                    5, 5, 0xFFE0U);
            gd32h759i_lcd_flush();
            printf("Touch: X=%u, Y=%u\r\n", (unsigned)tx, (unsigned)ty);
        }
        gd_eval_led_toggle(LED1);
        delay_1ms(20);
    }
}


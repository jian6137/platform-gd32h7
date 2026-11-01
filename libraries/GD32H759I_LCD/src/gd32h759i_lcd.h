/*!
    \file    gd32h759i_lcd.h
    \brief   480x272 RGB565 TLI LCD Display Driver for GD32H759I-EVAL
*/

#ifndef GD32H759I_LCD_H
#define GD32H759I_LCD_H

#include <stdint.h>
#include "gd32h7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH               480U
#define LCD_HEIGHT              272U
#define LCD_HSYNC               41U
#define LCD_HBP                 2U
#define LCD_HFP                 2U
#define LCD_VSYNC               10U
#define LCD_VBP                 2U
#define LCD_VFP                 2U

#define LCD_BYTES_PER_PIXEL     2U
#define LCD_FRAME_BYTES         (LCD_WIDTH * LCD_HEIGHT * LCD_BYTES_PER_PIXEL)

extern uint8_t g_display_framebuffer[];

/* Internal AXI SRAM framebuffer used by TLI layer0 (64-byte cacheline aligned) */
#define DISPLAY_FRAMEBUFFER     ((uintptr_t)g_display_framebuffer)

/* Initialize TLI GPIOs, backlight (PG13), PLL2 pixel clock, and TLI Layer0 */
void gd32h759i_lcd_init(void);
void gd32h759i_lcd_gpio_config(void);
void gd32h759i_lcd_tli_config(void);
void gd32h759i_lcd_backlight_set(uint8_t enable);
void gd32h759i_lcd_fill(uint16_t rgb565_color);
void gd32h759i_lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565_color);
void gd32h759i_lcd_flush(void);

/* Diagnostic patterns */
void display_color_bars(void);
void display_status_pattern(uint16_t bg_color, uint16_t box_color);

#ifdef __cplusplus
}
#endif

#endif /* GD32H759I_LCD_H */

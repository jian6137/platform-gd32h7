/*!
    \file    lcd_console.h
    \brief   On-screen diagnostic console and text renderer for GD32H759I TLI LCD
*/

#ifndef LCD_CONSOLE_H
#define LCD_CONSOLE_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include "gd32h759i_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_CONSOLE_COLS   60
#define LCD_CONSOLE_ROWS   17

/* 16-bit RGB565 Color Definitions */
#define CONSOLE_COLOR_BLACK       0x0000U
#define CONSOLE_COLOR_NAVY        0x000FU
#define CONSOLE_COLOR_DARKBLUE    0x0014U
#define CONSOLE_COLOR_WHITE       0xFFFFU
#define CONSOLE_COLOR_RED         0xF800U
#define CONSOLE_COLOR_GREEN       0x07E0U
#define CONSOLE_COLOR_BLUE        0x001FU
#define CONSOLE_COLOR_YELLOW      0xFFE0U
#define CONSOLE_COLOR_CYAN        0x07FFU
#define CONSOLE_COLOR_MAGENTA     0xF81FU
#define CONSOLE_COLOR_GRAY        0x8410U
#define CONSOLE_COLOR_LIGHTGRAY   0xC618U

void lcd_console_init(uint16_t bg_color, uint16_t fg_color);
void lcd_console_set_color(uint16_t fg_color);
void lcd_console_set_bg_color(uint16_t bg_color);
void lcd_console_clear(void);
void lcd_console_putc(char c);
void lcd_console_print(const char *str);
void lcd_console_printf(const char *fmt, ...);
void lcd_console_flush(void);
void lcd_draw_string_xy(uint16_t px, uint16_t py, const char *str, uint16_t fg, uint16_t bg);
void lcd_draw_volume_osd(uint8_t volume_percent);

#ifdef __cplusplus
}
#endif

#endif /* LCD_CONSOLE_H */


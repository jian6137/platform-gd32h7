/*!
    \file    gd32h759i_lcd.c
    \brief   480x272 RGB565 TLI LCD Display Driver for GD32H759I-EVAL
*/

#include "gd32h759i_lcd.h"

/* Internal AXI SRAM framebuffer used by TLI layer (255 KB, 64-byte cacheline aligned) */
__attribute__((section(".display_fb"), aligned(64)))
uint8_t g_display_framebuffer[LCD_FRAME_BYTES];

static void configure_tli_af(uint32_t port, uint32_t pins, uint32_t alternate_function)
{
    gpio_af_set(port, alternate_function, pins);
    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pins);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, pins);
}

void gd32h759i_lcd_gpio_config(void)
{
    static const rcu_periph_enum ports[] = {
        RCU_GPIOA, RCU_GPIOB, RCU_GPIOC, RCU_GPIOD,
        RCU_GPIOE, RCU_GPIOF, RCU_GPIOG, RCU_GPIOH
    };

    for (uint32_t i = 0U; i < sizeof(ports) / sizeof(ports[0]); ++i) {
        rcu_periph_clock_enable(ports[i]);
    }

    /* TLI Display Pins */
    configure_tli_af(GPIOE, GPIO_PIN_15, GPIO_AF_10);
    configure_tli_af(GPIOA, GPIO_PIN_7, GPIO_AF_14);
    configure_tli_af(GPIOG, GPIO_PIN_7, GPIO_AF_14);
    configure_tli_af(GPIOH, GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_3 | GPIO_PIN_2, GPIO_AF_14);
    configure_tli_af(GPIOA, GPIO_PIN_5, GPIO_AF_14);
    configure_tli_af(GPIOG, GPIO_PIN_6, GPIO_AF_14);
    configure_tli_af(GPIOC, GPIO_PIN_4, GPIO_AF_14);
    configure_tli_af(GPIOD, GPIO_PIN_3, GPIO_AF_14);
    configure_tli_af(GPIOC, GPIO_PIN_7 | GPIO_PIN_1, GPIO_AF_14);
    configure_tli_af(GPIOH, GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13, GPIO_AF_14);
    configure_tli_af(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_AF_14);
    configure_tli_af(GPIOB, GPIO_PIN_9 | GPIO_PIN_8, GPIO_AF_14);
    configure_tli_af(GPIOB, GPIO_PIN_5, GPIO_AF_3);
    configure_tli_af(GPIOC, GPIO_PIN_11, GPIO_AF_14); /* LCD_B4 on JP52(1,2) */
    configure_tli_af(GPIOG, GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_14, GPIO_AF_14);

    /* LCD Backlight Enable: PG13 */
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_85MHZ, GPIO_PIN_13);
    gpio_bit_set(GPIOG, GPIO_PIN_13);
}

void gd32h759i_lcd_backlight_set(uint8_t enable)
{
    if (enable) {
        gpio_bit_set(GPIOG, GPIO_PIN_13);
    } else {
        gpio_bit_reset(GPIOG, GPIO_PIN_13);
    }
}

void gd32h759i_lcd_tli_config(void)
{
    tli_parameter_struct tli;
    tli_layer_parameter_struct layer;

    rcu_periph_clock_enable(RCU_TLI);
    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_192M_836M);
    (void)rcu_pll2_config(25U, 192U, 3U, 3U, 3U);
    rcu_pll_clock_output_enable(RCU_PLL2R);
    rcu_tli_clock_div_config(RCU_PLL2R_DIV8);
    rcu_osci_on(RCU_PLL2_CK);
    (void)rcu_osci_stab_wait(RCU_PLL2_CK);

    tli_struct_para_init(&tli);
    tli.synpsz_hpsz = LCD_HSYNC - 1U;
    tli.synpsz_vpsz = LCD_VSYNC - 1U;
    tli.backpsz_hbpsz = LCD_HSYNC + LCD_HBP - 1U;
    tli.backpsz_vbpsz = LCD_VSYNC + LCD_VBP - 1U;
    tli.activesz_hasz = LCD_HSYNC + LCD_HBP + LCD_WIDTH - 1U;
    tli.activesz_vasz = LCD_VSYNC + LCD_VBP + LCD_HEIGHT - 1U;
    tli.totalsz_htsz = LCD_HSYNC + LCD_HBP + LCD_WIDTH + LCD_HFP - 1U;
    tli.totalsz_vtsz = LCD_VSYNC + LCD_VBP + LCD_HEIGHT + LCD_VFP - 1U;
    tli.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init(&tli);

    tli_layer_struct_para_init(&layer);
    layer.layer_window_leftpos = LCD_HSYNC + LCD_HBP;
    layer.layer_window_rightpos = LCD_HSYNC + LCD_HBP + LCD_WIDTH - 1U;
    layer.layer_window_toppos = LCD_VSYNC + LCD_VBP;
    layer.layer_window_bottompos = LCD_VSYNC + LCD_VBP + LCD_HEIGHT - 1U;
    layer.layer_ppf = LAYER_PPF_RGB565;
    layer.layer_sa = 0xFFU;
    layer.layer_default_alpha = 0U;
    layer.layer_default_blue = 0U;
    layer.layer_default_green = 0U;
    layer.layer_default_red = 0U;
    layer.layer_acf1 = LAYER_ACF1_SA;
    layer.layer_acf2 = LAYER_ACF2_SA;
    layer.layer_frame_bufaddr = DISPLAY_FRAMEBUFFER;
    layer.layer_frame_buf_stride_offset = LCD_WIDTH * LCD_BYTES_PER_PIXEL;
    layer.layer_frame_line_length = LCD_WIDTH * LCD_BYTES_PER_PIXEL + 7U;
    layer.layer_frame_total_line_number = LCD_HEIGHT;
    tli_layer_init(LAYER0, &layer);

    tli_dither_config(TLI_DITHER_ENABLE);
    tli_layer_enable(LAYER0);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
}

void gd32h759i_lcd_init(void)
{
    gd32h759i_lcd_gpio_config();
    gd32h759i_lcd_tli_config();
}

void gd32h759i_lcd_flush(void)
{
    SCB_CleanDCache_by_Addr((volatile void *)DISPLAY_FRAMEBUFFER, (int32_t)LCD_FRAME_BYTES);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

void gd32h759i_lcd_fill(uint16_t rgb565_color)
{
    volatile uint16_t *frame = (volatile uint16_t *)DISPLAY_FRAMEBUFFER;
    for (uint32_t i = 0U; i < LCD_WIDTH * LCD_HEIGHT; ++i) {
        frame[i] = rgb565_color;
    }
    gd32h759i_lcd_flush();
}

void gd32h759i_lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565_color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }
    uint32_t x_end = (uint32_t)x + w;
    uint32_t y_end = (uint32_t)y + h;
    if (x_end > LCD_WIDTH) {
        x_end = LCD_WIDTH;
    }
    if (y_end > LCD_HEIGHT) {
        y_end = LCD_HEIGHT;
    }
    volatile uint16_t *frame = (volatile uint16_t *)DISPLAY_FRAMEBUFFER;
    for (uint32_t row = y; row < y_end; ++row) {
        for (uint32_t col = x; col < x_end; ++col) {
            frame[row * LCD_WIDTH + col] = rgb565_color;
        }
    }
}


void display_color_bars(void)
{
    volatile uint16_t *frame = (volatile uint16_t *)DISPLAY_FRAMEBUFFER;
    static const uint16_t colors[8] = {
        0xF800U, /* Red */
        0x07E0U, /* Green */
        0x001FU, /* Blue */
        0xFFE0U, /* Yellow */
        0xF81FU, /* Magenta */
        0x07FFU, /* Cyan */
        0xFFFFU, /* White */
        0x0000U  /* Black */
    };

    for (uint32_t pixel = 0U; pixel < LCD_WIDTH * LCD_HEIGHT; ++pixel) {
        uint32_t band = (pixel % LCD_WIDTH) / (LCD_WIDTH / 8U);
        if (band > 7U) {
            band = 7U;
        }
        frame[pixel] = colors[band];
    }
    gd32h759i_lcd_flush();
}

void display_status_pattern(uint16_t bg_color, uint16_t box_color)
{
    volatile uint16_t *frame = (volatile uint16_t *)DISPLAY_FRAMEBUFFER;
    for (uint32_t y = 0U; y < LCD_HEIGHT; ++y) {
        for (uint32_t x = 0U; x < LCD_WIDTH; ++x) {
            if ((x >= (LCD_WIDTH / 4U)) && (x < (LCD_WIDTH * 3U / 4U)) &&
                (y >= (LCD_HEIGHT / 4U)) && (y < (LCD_HEIGHT * 3U / 4U))) {
                frame[y * LCD_WIDTH + x] = box_color;
            } else {
                frame[y * LCD_WIDTH + x] = bg_color;
            }
        }
    }
    gd32h759i_lcd_flush();
}


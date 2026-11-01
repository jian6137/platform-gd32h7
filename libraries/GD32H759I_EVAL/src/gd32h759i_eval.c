/*!
    \file    gd32h759i_eval.c
    \brief   Firmware functions to manage LEDs, keys, COM ports on GD32H759I-EVAL

    \version 2025-02-19, V2.1.0, demo for GD32H7xx
*/

#include "gd32h759i_eval.h"

static const uint32_t GPIO_PORT[LEDn]            = {LED1_GPIO_PORT, LED2_GPIO_PORT};
static const uint32_t GPIO_PIN[LEDn]             = {LED1_PIN, LED2_PIN};
static const rcu_periph_enum COM_CLK[COMn]       = {EVAL_COM_CLK};
static const uint32_t COM_TX_PIN[COMn]           = {EVAL_COM_TX_PIN};
static const uint32_t COM_RX_PIN[COMn]           = {EVAL_COM_RX_PIN};
static const rcu_periph_enum GPIO_CLK[LEDn]      = {LED1_GPIO_CLK, LED2_GPIO_CLK};
static const uint32_t KEY_PORT[KEYn]             = {WAKEUP_KEY_GPIO_PORT, TAMPER_KEY_GPIO_PORT, USER_KEY_GPIO_PORT};
static const uint32_t KEY_PIN[KEYn]              = {WAKEUP_KEY_PIN, TAMPER_KEY_PIN, USER_KEY_PIN};
static const rcu_periph_enum KEY_CLK[KEYn]       = {WAKEUP_KEY_GPIO_CLK, TAMPER_KEY_GPIO_CLK, USER_KEY_GPIO_CLK};
static const exti_line_enum KEY_EXTI_LINE[KEYn]  = {WAKEUP_KEY_EXTI_LINE, TAMPER_KEY_EXTI_LINE, USER_KEY_EXTI_LINE};
static const uint8_t KEY_PORT_SOURCE[KEYn]       = {WAKEUP_KEY_EXTI_PORT_SOURCE, TAMPER_KEY_EXTI_PORT_SOURCE, USER_KEY_EXTI_PORT_SOURCE};
static const uint8_t KEY_PIN_SOURCE[KEYn]        = {WAKEUP_KEY_EXTI_PIN_SOURCE, TAMPER_KEY_EXTI_PIN_SOURCE, USER_KEY_EXTI_PIN_SOURCE};
static const IRQn_Type KEY_IRQn[KEYn]            = {WAKEUP_KEY_EXTI_IRQn, TAMPER_KEY_EXTI_IRQn, USER_KEY_EXTI_IRQn};

void gd_eval_led_init(led_typedef_enum lednum)
{
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
    gpio_mode_set(GPIO_PORT[lednum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN[lednum]);
    gpio_output_options_set(GPIO_PORT[lednum], GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN[lednum]);
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void gd_eval_led_on(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void gd_eval_led_off(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void gd_eval_led_toggle(led_typedef_enum lednum)
{
    GPIO_TG(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode)
{
    rcu_periph_clock_enable(KEY_CLK[keynum]);
    rcu_periph_clock_enable(RCU_SYSCFG);

    gpio_mode_set(KEY_PORT[keynum], GPIO_MODE_INPUT, GPIO_PUPD_NONE, KEY_PIN[keynum]);

    if (keymode == KEY_MODE_EXTI) {
        nvic_irq_enable(KEY_IRQn[keynum], 2U, 0U);
        syscfg_exti_line_config(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);
        exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_RISING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
    }
}

uint8_t gd_eval_key_state_get(key_typedef_enum keynum)
{
    return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
}

void gd_eval_com_init(uint32_t com)
{
    uint32_t com_id = 0U;

    rcu_periph_clock_enable(EVAL_COM_GPIO_CLK);
    rcu_periph_clock_enable(COM_CLK[com_id]);

    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_TX_PIN[com_id]);
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_RX_PIN[com_id]);

    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN[com_id]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, COM_TX_PIN[com_id]);

    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN[com_id]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, COM_RX_PIN[com_id]);

    usart_deinit(com);
    usart_baudrate_set(com, 115200U);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);
    usart_enable(com);
}


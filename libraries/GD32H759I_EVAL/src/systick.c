/*!
    \file    systick.c
    \brief   SysTick configuration and 1ms timebase functions

    \version 2025-02-19, V2.1.0, demo for GD32H7xx
*/

#include "gd32h7xx.h"
#include "systick.h"

static volatile uint32_t delay;
static volatile uint32_t g_ms_ticks = 0U;

void systick_config(void)
{
    if (SysTick_Config(SystemCoreClock / 1000U)) {
        while (1) {
        }
    }
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay_1ms(uint32_t count)
{
    delay = count;
    while (0U != delay) {
    }
}

void delay_decrement(void)
{
    g_ms_ticks++;
    if (0U != delay) {
        delay--;
    }
}

uint32_t systick_get_ms(void)
{
    return g_ms_ticks;
}

__attribute__((weak)) void systick_user_callback(void)
{
}

void SysTick_Handler(void)
{
    delay_decrement();
    systick_user_callback();
}


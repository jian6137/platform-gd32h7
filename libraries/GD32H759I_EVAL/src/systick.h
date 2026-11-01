/*!
    \file    systick.h
    \brief   Header file of SysTick millisecond timer and delay utilities

    \version 2025-02-19, V2.1.0, demo for GD32H7xx
*/

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* configure systick for 1kHz (1ms) tick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_1ms(uint32_t count);
/* delay decrement and tick increment (called by SysTick_Handler) */
void delay_decrement(void);
/* get elapsed milliseconds since boot */
uint32_t systick_get_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTICK_H */


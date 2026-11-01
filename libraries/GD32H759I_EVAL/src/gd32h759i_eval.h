/*!
    \file    gd32h759i_eval.h
    \brief   Definitions for GD32H759I_EVAL's LEDs, keys and COM ports hardware resources

    \version 2025-02-19, V2.1.0, demo for GD32H7xx
*/

#ifndef GD32H759I_EVAL_H
#define GD32H759I_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32h7xx.h"

/* exported types */
typedef enum {
    LED1 = 0,
    LED2 = 1
} led_typedef_enum;

typedef enum {
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
    KEY_USER = 2
} key_typedef_enum;

typedef enum {
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             2U

#define LED1_PIN                         GPIO_PIN_10
#define LED1_GPIO_PORT                   GPIOF
#define LED1_GPIO_CLK                    RCU_GPIOF

#define LED2_PIN                         GPIO_PIN_6
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCU_GPIOA

/* eval board low layer button */
#define KEYn                             3U

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_IRQn

/* tamper push-button */
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN13
#define TAMPER_KEY_EXTI_IRQn             EXTI10_15_IRQn

/* user push-button */
#define USER_KEY_PIN                     GPIO_PIN_8
#define USER_KEY_GPIO_PORT               GPIOF
#define USER_KEY_GPIO_CLK                RCU_GPIOF
#define USER_KEY_EXTI_LINE               EXTI_8
#define USER_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOF
#define USER_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN8
#define USER_KEY_EXTI_IRQn               EXTI5_9_IRQn

/* eval board low layer COM */
#define COMn                             1U

/* definition for COM, connected to USART0 */
#define EVAL_COM                         USART0
#define EVAL_COM0                        EVAL_COM
#define EVAL_COM_CLK                     RCU_USART0

#define EVAL_COM_TX_PIN                  GPIO_PIN_9
#define EVAL_COM_RX_PIN                  GPIO_PIN_10

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_7

/* function declarations */
void gd_eval_led_init(led_typedef_enum lednum);
void gd_eval_led_on(led_typedef_enum lednum);
void gd_eval_led_off(led_typedef_enum lednum);
void gd_eval_led_toggle(led_typedef_enum lednum);
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode);
uint8_t gd_eval_key_state_get(key_typedef_enum keynum);
void gd_eval_com_init(uint32_t com);

#ifdef __cplusplus
}
#endif

#endif /* GD32H759I_EVAL_H */

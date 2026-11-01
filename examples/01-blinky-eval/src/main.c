#include "gd32h7xx.h"
#include "gd32h759i_eval.h"
#include "systick.h"
#include "exmc_sdram.h"
#include <stdio.h>

static void mpu_sdram_config(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    ARM_MPU_Disable();

    /* Region 0: Default 4GB space - no access to prevent speculative fetches */
    mpu_init_struct.region_base_address  = 0x00000000U;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission    = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable    = 0x87U;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* Region 1: External SDRAM (32MB @ 0xC0000000) Write-Back Cacheable */
    mpu_init_struct.region_base_address  = SDRAM_DEVICE0_ADDR;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_32MB;
    mpu_init_struct.access_permission    = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable    = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE1;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}

int main(void)
{
    /* Enable Cortex-M7 I-Cache and D-Cache */
    SCB_EnableICache();
    SCB_EnableDCache();

    /* Configure 1ms SysTick timer */
    systick_config();

    /* Initialize onboard LEDs, push-buttons, and USART0 (115200 8N1) */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
    gd_eval_com_init(EVAL_COM0);

    /* Configure MPU and initialize 32MB external SDRAM at 0xC0000000 */
    mpu_sdram_config();
    exmc_sdram_config();

    printf("\r\n========================================\r\n");
    printf(" GD32H759I-EVAL PlatformIO Blinky Demo\r\n");
    printf(" SystemCoreClock: %lu Hz (%lu MHz)\r\n",
           (unsigned long)SystemCoreClock,
           (unsigned long)(SystemCoreClock / 1000000UL));
    printf("========================================\r\n");

    /* Quick SDRAM read/write sanity check */
    volatile uint32_t *sdram_ptr = (volatile uint32_t *)SDRAM_DEVICE0_ADDR;
    sdram_ptr[0] = 0xCAFEBABEUL;
    sdram_ptr[1] = 0x12345678UL;
    if (sdram_ptr[0] == 0xCAFEBABEUL && sdram_ptr[1] == 0x12345678UL) {
        printf("[OK] EXMC SDRAM verified at 0x%08lX\r\n", (unsigned long)SDRAM_DEVICE0_ADDR);
    } else {
        printf("[ERR] EXMC SDRAM check failed!\r\n");
    }

    uint32_t tick = 0;
    while (1) {
        gd_eval_led_toggle(LED1);
        delay_1ms(250);
        gd_eval_led_toggle(LED2);
        delay_1ms(250);

        if (RESET == gd_eval_key_state_get(KEY_WAKEUP)) {
            printf("[%lu] WAKEUP key pressed\r\n", (unsigned long)tick);
        }
        if (RESET == gd_eval_key_state_get(KEY_TAMPER)) {
            printf("[%lu] TAMPER key pressed\r\n", (unsigned long)tick);
        }
        tick++;
    }
}


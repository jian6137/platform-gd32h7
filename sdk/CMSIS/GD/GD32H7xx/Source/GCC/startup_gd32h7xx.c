/*!
    \file    startup_gd32h7xx.c
    \brief   GCC Cortex-M7 Startup, Complete 217-IRQ Vector Table, and Newlib Syscall Stubs
             for GigaDevice GD32H7xx series MCUs.

    \version 2026-11-01, V1.4.0-pio, PlatformIO GD32H7 Development Platform
    \license Apache-2.0
*/

#include <stdint.h>
#include <stddef.h>
#include "gd32h7xx.h"

/* Linker script symbols */
extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _end;

extern int main(void);
extern void SystemInit(void);
__attribute__((weak)) void __libc_init_array(void) {}

void Default_Handler(void)
{
    while (1) {
    }
}

#define WEAK_DEFAULT __attribute__((weak, alias("Default_Handler")))

/* Cortex-M7 Core Exception Handlers */
WEAK_DEFAULT void NMI_Handler(void);
WEAK_DEFAULT void HardFault_Handler(void);
WEAK_DEFAULT void MemManage_Handler(void);
WEAK_DEFAULT void BusFault_Handler(void);
WEAK_DEFAULT void UsageFault_Handler(void);
WEAK_DEFAULT void SVC_Handler(void);
WEAK_DEFAULT void DebugMon_Handler(void);
WEAK_DEFAULT void PendSV_Handler(void);
WEAK_DEFAULT void SysTick_Handler(void);

/* GD32H7xx External Peripheral Interrupt Handlers (IRQn 0 .. 216) */
WEAK_DEFAULT void WWDGT_IRQHandler(void);
WEAK_DEFAULT void VAVD_LVD_VOVD_IRQHandler(void);
WEAK_DEFAULT void TAMPER_STAMP_LXTAL_IRQHandler(void);
WEAK_DEFAULT void RTC_WKUP_IRQHandler(void);
WEAK_DEFAULT void FMC_IRQHandler(void);
WEAK_DEFAULT void RCU_IRQHandler(void);
WEAK_DEFAULT void EXTI0_IRQHandler(void);
WEAK_DEFAULT void EXTI1_IRQHandler(void);
WEAK_DEFAULT void EXTI2_IRQHandler(void);
WEAK_DEFAULT void EXTI3_IRQHandler(void);
WEAK_DEFAULT void EXTI4_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel0_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel4_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel5_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel6_IRQHandler(void);
WEAK_DEFAULT void ADC0_1_IRQHandler(void);
WEAK_DEFAULT void EXTI5_9_IRQHandler(void);
WEAK_DEFAULT void TIMER0_BRK_IRQHandler(void);
WEAK_DEFAULT void TIMER0_UP_IRQHandler(void);
WEAK_DEFAULT void TIMER0_TRG_CMT_IRQHandler(void);
WEAK_DEFAULT void TIMER0_Channel_IRQHandler(void);
WEAK_DEFAULT void TIMER1_IRQHandler(void);
WEAK_DEFAULT void TIMER2_IRQHandler(void);
WEAK_DEFAULT void TIMER3_IRQHandler(void);
WEAK_DEFAULT void I2C0_EV_IRQHandler(void);
WEAK_DEFAULT void I2C0_ER_IRQHandler(void);
WEAK_DEFAULT void I2C1_EV_IRQHandler(void);
WEAK_DEFAULT void I2C1_ER_IRQHandler(void);
WEAK_DEFAULT void SPI0_IRQHandler(void);
WEAK_DEFAULT void SPI1_IRQHandler(void);
WEAK_DEFAULT void USART0_IRQHandler(void);
WEAK_DEFAULT void USART1_IRQHandler(void);
WEAK_DEFAULT void USART2_IRQHandler(void);
WEAK_DEFAULT void EXTI10_15_IRQHandler(void);
WEAK_DEFAULT void RTC_Alarm_IRQHandler(void);
WEAK_DEFAULT void TIMER7_BRK_IRQHandler(void);
WEAK_DEFAULT void TIMER7_UP_IRQHandler(void);
WEAK_DEFAULT void TIMER7_TRG_CMT_IRQHandler(void);
WEAK_DEFAULT void TIMER7_Channel_IRQHandler(void);
WEAK_DEFAULT void DMA0_Channel7_IRQHandler(void);
WEAK_DEFAULT void EXMC_IRQHandler(void);
WEAK_DEFAULT void SDIO0_IRQHandler(void);
WEAK_DEFAULT void TIMER4_IRQHandler(void);
WEAK_DEFAULT void SPI2_IRQHandler(void);
WEAK_DEFAULT void UART3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void TIMER5_DAC_UDR_IRQHandler(void);
WEAK_DEFAULT void TIMER6_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel0_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel4_IRQHandler(void);
WEAK_DEFAULT void ENET0_IRQHandler(void);
WEAK_DEFAULT void ENET0_WKUP_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel5_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel6_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel7_IRQHandler(void);
WEAK_DEFAULT void USART5_IRQHandler(void);
WEAK_DEFAULT void I2C2_EV_IRQHandler(void);
WEAK_DEFAULT void I2C2_ER_IRQHandler(void);
WEAK_DEFAULT void USBHS0_EP1_OUT_IRQHandler(void);
WEAK_DEFAULT void USBHS0_EP1_IN_IRQHandler(void);
WEAK_DEFAULT void USBHS0_WKUP_IRQHandler(void);
WEAK_DEFAULT void USBHS0_IRQHandler(void);
WEAK_DEFAULT void DCI_IRQHandler(void);
WEAK_DEFAULT void CAU_IRQHandler(void);
WEAK_DEFAULT void HAU_TRNG_IRQHandler(void);
WEAK_DEFAULT void FPU_IRQHandler(void);
WEAK_DEFAULT void UART6_IRQHandler(void);
WEAK_DEFAULT void UART7_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void SPI4_IRQHandler(void);
WEAK_DEFAULT void SPI5_IRQHandler(void);
WEAK_DEFAULT void SAI0_IRQHandler(void);
WEAK_DEFAULT void TLI_IRQHandler(void);
WEAK_DEFAULT void TLI_ER_IRQHandler(void);
WEAK_DEFAULT void IPA_IRQHandler(void);
WEAK_DEFAULT void SAI1_IRQHandler(void);
WEAK_DEFAULT void OSPI0_IRQHandler(void);
WEAK_DEFAULT void I2C3_EV_IRQHandler(void);
WEAK_DEFAULT void I2C3_ER_IRQHandler(void);
WEAK_DEFAULT void RSPDIF_IRQHandler(void);
WEAK_DEFAULT void DMAMUX_OVR_IRQHandler(void);
WEAK_DEFAULT void HPDF_INT0_IRQHandler(void);
WEAK_DEFAULT void HPDF_INT1_IRQHandler(void);
WEAK_DEFAULT void HPDF_INT2_IRQHandler(void);
WEAK_DEFAULT void HPDF_INT3_IRQHandler(void);
WEAK_DEFAULT void SAI2_IRQHandler(void);
WEAK_DEFAULT void TIMER14_IRQHandler(void);
WEAK_DEFAULT void TIMER15_IRQHandler(void);
WEAK_DEFAULT void TIMER16_IRQHandler(void);
WEAK_DEFAULT void MDIO_IRQHandler(void);
WEAK_DEFAULT void MDMA_IRQHandler(void);
WEAK_DEFAULT void SDIO1_IRQHandler(void);
WEAK_DEFAULT void HWSEM_IRQHandler(void);
WEAK_DEFAULT void ADC2_IRQHandler(void);
WEAK_DEFAULT void CMP0_1_IRQHandler(void);
WEAK_DEFAULT void CTC_IRQHandler(void);
WEAK_DEFAULT void RAMECCMU_IRQHandler(void);
WEAK_DEFAULT void OSPI1_IRQHandler(void);
WEAK_DEFAULT void RTDEC0_IRQHandler(void);
WEAK_DEFAULT void RTDEC1_IRQHandler(void);
WEAK_DEFAULT void FAC_IRQHandler(void);
WEAK_DEFAULT void TMU_IRQHandler(void);
WEAK_DEFAULT void TIMER22_IRQHandler(void);
WEAK_DEFAULT void TIMER23_IRQHandler(void);
WEAK_DEFAULT void TIMER30_IRQHandler(void);
WEAK_DEFAULT void TIMER31_IRQHandler(void);
WEAK_DEFAULT void TIMER40_IRQHandler(void);
WEAK_DEFAULT void TIMER41_IRQHandler(void);
WEAK_DEFAULT void TIMER42_IRQHandler(void);
WEAK_DEFAULT void TIMER43_IRQHandler(void);
WEAK_DEFAULT void TIMER44_IRQHandler(void);
WEAK_DEFAULT void TIMER50_IRQHandler(void);
WEAK_DEFAULT void TIMER51_IRQHandler(void);
WEAK_DEFAULT void USBHS1_EP1_OUT_IRQHandler(void);
WEAK_DEFAULT void USBHS1_EP1_IN_IRQHandler(void);
WEAK_DEFAULT void USBHS1_WKUP_IRQHandler(void);
WEAK_DEFAULT void USBHS1_IRQHandler(void);
WEAK_DEFAULT void ENET1_IRQHandler(void);
WEAK_DEFAULT void ENET1_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN0_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN0_Message_IRQHandler(void);
WEAK_DEFAULT void CAN0_Busoff_IRQHandler(void);
WEAK_DEFAULT void CAN0_Error_IRQHandler(void);
WEAK_DEFAULT void CAN0_FastError_IRQHandler(void);
WEAK_DEFAULT void CAN0_TEC_IRQHandler(void);
WEAK_DEFAULT void CAN0_REC_IRQHandler(void);
WEAK_DEFAULT void CAN1_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN1_Message_IRQHandler(void);
WEAK_DEFAULT void CAN1_Busoff_IRQHandler(void);
WEAK_DEFAULT void CAN1_Error_IRQHandler(void);
WEAK_DEFAULT void CAN1_FastError_IRQHandler(void);
WEAK_DEFAULT void CAN1_TEC_IRQHandler(void);
WEAK_DEFAULT void CAN1_REC_IRQHandler(void);
WEAK_DEFAULT void CAN2_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN2_Message_IRQHandler(void);
WEAK_DEFAULT void CAN2_Busoff_IRQHandler(void);
WEAK_DEFAULT void CAN2_Error_IRQHandler(void);
WEAK_DEFAULT void CAN2_FastError_IRQHandler(void);
WEAK_DEFAULT void CAN2_TEC_IRQHandler(void);
WEAK_DEFAULT void CAN2_REC_IRQHandler(void);
WEAK_DEFAULT void EFUSE_IRQHandler(void);
WEAK_DEFAULT void I2C0_WKUP_IRQHandler(void);
WEAK_DEFAULT void I2C1_WKUP_IRQHandler(void);
WEAK_DEFAULT void I2C2_WKUP_IRQHandler(void);
WEAK_DEFAULT void I2C3_WKUP_IRQHandler(void);
WEAK_DEFAULT void LPDTS_IRQHandler(void);
WEAK_DEFAULT void LPDTS_WKUP_IRQHandler(void);
WEAK_DEFAULT void TIMER0_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER7_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER1_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER2_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER3_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER4_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER22_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER23_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER30_DEC_IRQHandler(void);
WEAK_DEFAULT void TIMER31_DEC_IRQHandler(void);

__attribute__((weak)) void Reset_Handler(void)
{
    uint32_t *source = &_sidata;
    uint32_t *destination = &_sdata;
    while (destination < &_edata) {
        *destination++ = *source++;
    }
    destination = &_sbss;
    while (destination < &_ebss) {
        *destination++ = 0U;
    }

    SystemInit();
    __libc_init_array();
    (void)main();

    while (1) {
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
__attribute__((section(".isr_vector"), used))
const uintptr_t g_pfnVectors[16U + 217U] = {
    [0 ... (16U + 216U)] = (uintptr_t)Default_Handler,
    [0]  = (uintptr_t)&_estack,
    [1]  = (uintptr_t)Reset_Handler,
    [2]  = (uintptr_t)NMI_Handler,
    [3]  = (uintptr_t)HardFault_Handler,
    [4]  = (uintptr_t)MemManage_Handler,
    [5]  = (uintptr_t)BusFault_Handler,
    [6]  = (uintptr_t)UsageFault_Handler,
    [11] = (uintptr_t)SVC_Handler,
    [12] = (uintptr_t)DebugMon_Handler,
    [14] = (uintptr_t)PendSV_Handler,
    [15] = (uintptr_t)SysTick_Handler,

    /* External Interrupts */
    [16U + WWDGT_IRQn]              = (uintptr_t)WWDGT_IRQHandler,
    [16U + VAVD_LVD_VOVD_IRQn]      = (uintptr_t)VAVD_LVD_VOVD_IRQHandler,
    [16U + TAMPER_STAMP_LXTAL_IRQn] = (uintptr_t)TAMPER_STAMP_LXTAL_IRQHandler,
    [16U + RTC_WKUP_IRQn]           = (uintptr_t)RTC_WKUP_IRQHandler,
    [16U + FMC_IRQn]                = (uintptr_t)FMC_IRQHandler,
    [16U + RCU_IRQn]                = (uintptr_t)RCU_IRQHandler,
    [16U + EXTI0_IRQn]              = (uintptr_t)EXTI0_IRQHandler,
    [16U + EXTI1_IRQn]              = (uintptr_t)EXTI1_IRQHandler,
    [16U + EXTI2_IRQn]              = (uintptr_t)EXTI2_IRQHandler,
    [16U + EXTI3_IRQn]              = (uintptr_t)EXTI3_IRQHandler,
    [16U + EXTI4_IRQn]              = (uintptr_t)EXTI4_IRQHandler,
    [16U + DMA0_Channel0_IRQn]      = (uintptr_t)DMA0_Channel0_IRQHandler,
    [16U + DMA0_Channel1_IRQn]      = (uintptr_t)DMA0_Channel1_IRQHandler,
    [16U + DMA0_Channel2_IRQn]      = (uintptr_t)DMA0_Channel2_IRQHandler,
    [16U + DMA0_Channel3_IRQn]      = (uintptr_t)DMA0_Channel3_IRQHandler,
    [16U + DMA0_Channel4_IRQn]      = (uintptr_t)DMA0_Channel4_IRQHandler,
    [16U + DMA0_Channel5_IRQn]      = (uintptr_t)DMA0_Channel5_IRQHandler,
    [16U + DMA0_Channel6_IRQn]      = (uintptr_t)DMA0_Channel6_IRQHandler,
    [16U + ADC0_1_IRQn]             = (uintptr_t)ADC0_1_IRQHandler,
    [16U + EXTI5_9_IRQn]            = (uintptr_t)EXTI5_9_IRQHandler,
    [16U + TIMER0_BRK_IRQn]         = (uintptr_t)TIMER0_BRK_IRQHandler,
    [16U + TIMER0_UP_IRQn]          = (uintptr_t)TIMER0_UP_IRQHandler,
    [16U + TIMER0_TRG_CMT_IRQn]     = (uintptr_t)TIMER0_TRG_CMT_IRQHandler,
    [16U + TIMER0_Channel_IRQn]     = (uintptr_t)TIMER0_Channel_IRQHandler,
    [16U + TIMER1_IRQn]             = (uintptr_t)TIMER1_IRQHandler,
    [16U + TIMER2_IRQn]             = (uintptr_t)TIMER2_IRQHandler,
    [16U + TIMER3_IRQn]             = (uintptr_t)TIMER3_IRQHandler,
    [16U + I2C0_EV_IRQn]            = (uintptr_t)I2C0_EV_IRQHandler,
    [16U + I2C0_ER_IRQn]            = (uintptr_t)I2C0_ER_IRQHandler,
    [16U + I2C1_EV_IRQn]            = (uintptr_t)I2C1_EV_IRQHandler,
    [16U + I2C1_ER_IRQn]            = (uintptr_t)I2C1_ER_IRQHandler,
    [16U + SPI0_IRQn]               = (uintptr_t)SPI0_IRQHandler,
    [16U + SPI1_IRQn]               = (uintptr_t)SPI1_IRQHandler,
    [16U + USART0_IRQn]             = (uintptr_t)USART0_IRQHandler,
    [16U + USART1_IRQn]             = (uintptr_t)USART1_IRQHandler,
    [16U + USART2_IRQn]             = (uintptr_t)USART2_IRQHandler,
    [16U + EXTI10_15_IRQn]          = (uintptr_t)EXTI10_15_IRQHandler,
    [16U + RTC_Alarm_IRQn]          = (uintptr_t)RTC_Alarm_IRQHandler,
    [16U + TIMER7_BRK_IRQn]         = (uintptr_t)TIMER7_BRK_IRQHandler,
    [16U + TIMER7_UP_IRQn]          = (uintptr_t)TIMER7_UP_IRQHandler,
    [16U + TIMER7_TRG_CMT_IRQn]     = (uintptr_t)TIMER7_TRG_CMT_IRQHandler,
    [16U + TIMER7_Channel_IRQn]     = (uintptr_t)TIMER7_Channel_IRQHandler,
    [16U + DMA0_Channel7_IRQn]      = (uintptr_t)DMA0_Channel7_IRQHandler,
    [16U + EXMC_IRQn]               = (uintptr_t)EXMC_IRQHandler,
    [16U + SDIO0_IRQn]              = (uintptr_t)SDIO0_IRQHandler,
    [16U + TIMER4_IRQn]             = (uintptr_t)TIMER4_IRQHandler,
    [16U + SPI2_IRQn]               = (uintptr_t)SPI2_IRQHandler,
    [16U + UART3_IRQn]              = (uintptr_t)UART3_IRQHandler,
    [16U + UART4_IRQn]              = (uintptr_t)UART4_IRQHandler,
    [16U + TIMER5_DAC_UDR_IRQn]     = (uintptr_t)TIMER5_DAC_UDR_IRQHandler,
    [16U + TIMER6_IRQn]             = (uintptr_t)TIMER6_IRQHandler,
    [16U + DMA1_Channel0_IRQn]      = (uintptr_t)DMA1_Channel0_IRQHandler,
    [16U + DMA1_Channel1_IRQn]      = (uintptr_t)DMA1_Channel1_IRQHandler,
    [16U + DMA1_Channel2_IRQn]      = (uintptr_t)DMA1_Channel2_IRQHandler,
    [16U + DMA1_Channel3_IRQn]      = (uintptr_t)DMA1_Channel3_IRQHandler,
    [16U + DMA1_Channel4_IRQn]      = (uintptr_t)DMA1_Channel4_IRQHandler,
    [16U + ENET0_IRQn]              = (uintptr_t)ENET0_IRQHandler,
    [16U + ENET0_WKUP_IRQn]         = (uintptr_t)ENET0_WKUP_IRQHandler,
    [16U + DMA1_Channel5_IRQn]      = (uintptr_t)DMA1_Channel5_IRQHandler,
    [16U + DMA1_Channel6_IRQn]      = (uintptr_t)DMA1_Channel6_IRQHandler,
    [16U + DMA1_Channel7_IRQn]      = (uintptr_t)DMA1_Channel7_IRQHandler,
    [16U + USART5_IRQn]             = (uintptr_t)USART5_IRQHandler,
    [16U + I2C2_EV_IRQn]            = (uintptr_t)I2C2_EV_IRQHandler,
    [16U + I2C2_ER_IRQn]            = (uintptr_t)I2C2_ER_IRQHandler,
    [16U + USBHS0_EP1_OUT_IRQn]     = (uintptr_t)USBHS0_EP1_OUT_IRQHandler,
    [16U + USBHS0_EP1_IN_IRQn]      = (uintptr_t)USBHS0_EP1_IN_IRQHandler,
    [16U + USBHS0_WKUP_IRQn]        = (uintptr_t)USBHS0_WKUP_IRQHandler,
    [16U + USBHS0_IRQn]             = (uintptr_t)USBHS0_IRQHandler,
    [16U + DCI_IRQn]                = (uintptr_t)DCI_IRQHandler,
    [16U + CAU_IRQn]                = (uintptr_t)CAU_IRQHandler,
    [16U + HAU_TRNG_IRQn]           = (uintptr_t)HAU_TRNG_IRQHandler,
    [16U + FPU_IRQn]                = (uintptr_t)FPU_IRQHandler,
    [16U + UART6_IRQn]              = (uintptr_t)UART6_IRQHandler,
    [16U + UART7_IRQn]              = (uintptr_t)UART7_IRQHandler,
    [16U + SPI3_IRQn]               = (uintptr_t)SPI3_IRQHandler,
    [16U + SPI4_IRQn]               = (uintptr_t)SPI4_IRQHandler,
    [16U + SPI5_IRQn]               = (uintptr_t)SPI5_IRQHandler,
    [16U + SAI0_IRQn]               = (uintptr_t)SAI0_IRQHandler,
    [16U + TLI_IRQn]                = (uintptr_t)TLI_IRQHandler,
    [16U + TLI_ER_IRQn]             = (uintptr_t)TLI_ER_IRQHandler,
    [16U + IPA_IRQn]                = (uintptr_t)IPA_IRQHandler,
    [16U + SAI1_IRQn]               = (uintptr_t)SAI1_IRQHandler,
    [16U + OSPI0_IRQn]              = (uintptr_t)OSPI0_IRQHandler,
    [16U + I2C3_EV_IRQn]            = (uintptr_t)I2C3_EV_IRQHandler,
    [16U + I2C3_ER_IRQn]            = (uintptr_t)I2C3_ER_IRQHandler,
    [16U + RSPDIF_IRQn]             = (uintptr_t)RSPDIF_IRQHandler,
    [16U + DMAMUX_OVR_IRQn]         = (uintptr_t)DMAMUX_OVR_IRQHandler,
    [16U + HPDF_INT0_IRQn]          = (uintptr_t)HPDF_INT0_IRQHandler,
    [16U + HPDF_INT1_IRQn]          = (uintptr_t)HPDF_INT1_IRQHandler,
    [16U + HPDF_INT2_IRQn]          = (uintptr_t)HPDF_INT2_IRQHandler,
    [16U + HPDF_INT3_IRQn]          = (uintptr_t)HPDF_INT3_IRQHandler,
    [16U + SAI2_IRQn]               = (uintptr_t)SAI2_IRQHandler,
    [16U + TIMER14_IRQn]            = (uintptr_t)TIMER14_IRQHandler,
    [16U + TIMER15_IRQn]            = (uintptr_t)TIMER15_IRQHandler,
    [16U + TIMER16_IRQn]            = (uintptr_t)TIMER16_IRQHandler,
    [16U + MDIO_IRQn]               = (uintptr_t)MDIO_IRQHandler,
    [16U + MDMA_IRQn]               = (uintptr_t)MDMA_IRQHandler,
    [16U + SDIO1_IRQn]              = (uintptr_t)SDIO1_IRQHandler,
    [16U + HWSEM_IRQn]              = (uintptr_t)HWSEM_IRQHandler,
    [16U + ADC2_IRQn]               = (uintptr_t)ADC2_IRQHandler,
    [16U + CMP0_1_IRQn]             = (uintptr_t)CMP0_1_IRQHandler,
    [16U + CTC_IRQn]                = (uintptr_t)CTC_IRQHandler,
    [16U + RAMECCMU_IRQn]           = (uintptr_t)RAMECCMU_IRQHandler,
    [16U + OSPI1_IRQn]              = (uintptr_t)OSPI1_IRQHandler,
    [16U + RTDEC0_IRQn]             = (uintptr_t)RTDEC0_IRQHandler,
    [16U + RTDEC1_IRQn]             = (uintptr_t)RTDEC1_IRQHandler,
    [16U + FAC_IRQn]                = (uintptr_t)FAC_IRQHandler,
    [16U + TMU_IRQn]                = (uintptr_t)TMU_IRQHandler,
    [16U + TIMER22_IRQn]            = (uintptr_t)TIMER22_IRQHandler,
    [16U + TIMER23_IRQn]            = (uintptr_t)TIMER23_IRQHandler,
    [16U + TIMER30_IRQn]            = (uintptr_t)TIMER30_IRQHandler,
    [16U + TIMER31_IRQn]            = (uintptr_t)TIMER31_IRQHandler,
    [16U + TIMER40_IRQn]            = (uintptr_t)TIMER40_IRQHandler,
    [16U + TIMER41_IRQn]            = (uintptr_t)TIMER41_IRQHandler,
    [16U + TIMER42_IRQn]            = (uintptr_t)TIMER42_IRQHandler,
    [16U + TIMER43_IRQn]            = (uintptr_t)TIMER43_IRQHandler,
    [16U + TIMER44_IRQn]            = (uintptr_t)TIMER44_IRQHandler,
    [16U + TIMER50_IRQn]            = (uintptr_t)TIMER50_IRQHandler,
    [16U + TIMER51_IRQn]            = (uintptr_t)TIMER51_IRQHandler,
    [16U + USBHS1_EP1_OUT_IRQn]     = (uintptr_t)USBHS1_EP1_OUT_IRQHandler,
    [16U + USBHS1_EP1_IN_IRQn]      = (uintptr_t)USBHS1_EP1_IN_IRQHandler,
    [16U + USBHS1_WKUP_IRQn]        = (uintptr_t)USBHS1_WKUP_IRQHandler,
    [16U + USBHS1_IRQn]             = (uintptr_t)USBHS1_IRQHandler,
    [16U + ENET1_IRQn]              = (uintptr_t)ENET1_IRQHandler,
    [16U + ENET1_WKUP_IRQn]         = (uintptr_t)ENET1_WKUP_IRQHandler,
    [16U + CAN0_WKUP_IRQn]          = (uintptr_t)CAN0_WKUP_IRQHandler,
    [16U + CAN0_Message_IRQn]       = (uintptr_t)CAN0_Message_IRQHandler,
    [16U + CAN0_Busoff_IRQn]        = (uintptr_t)CAN0_Busoff_IRQHandler,
    [16U + CAN0_Error_IRQn]         = (uintptr_t)CAN0_Error_IRQHandler,
    [16U + CAN0_FastError_IRQn]     = (uintptr_t)CAN0_FastError_IRQHandler,
    [16U + CAN0_TEC_IRQn]           = (uintptr_t)CAN0_TEC_IRQHandler,
    [16U + CAN0_REC_IRQn]           = (uintptr_t)CAN0_REC_IRQHandler,
    [16U + CAN1_WKUP_IRQn]          = (uintptr_t)CAN1_WKUP_IRQHandler,
    [16U + CAN1_Message_IRQn]       = (uintptr_t)CAN1_Message_IRQHandler,
    [16U + CAN1_Busoff_IRQn]        = (uintptr_t)CAN1_Busoff_IRQHandler,
    [16U + CAN1_Error_IRQn]         = (uintptr_t)CAN1_Error_IRQHandler,
    [16U + CAN1_FastError_IRQn]     = (uintptr_t)CAN1_FastError_IRQHandler,
    [16U + CAN1_TEC_IRQn]           = (uintptr_t)CAN1_TEC_IRQHandler,
    [16U + CAN1_REC_IRQn]           = (uintptr_t)CAN1_REC_IRQHandler,
    [16U + CAN2_WKUP_IRQn]          = (uintptr_t)CAN2_WKUP_IRQHandler,
    [16U + CAN2_Message_IRQn]       = (uintptr_t)CAN2_Message_IRQHandler,
    [16U + CAN2_Busoff_IRQn]        = (uintptr_t)CAN2_Busoff_IRQHandler,
    [16U + CAN2_Error_IRQn]         = (uintptr_t)CAN2_Error_IRQHandler,
    [16U + CAN2_FastError_IRQn]     = (uintptr_t)CAN2_FastError_IRQHandler,
    [16U + CAN2_TEC_IRQn]           = (uintptr_t)CAN2_TEC_IRQHandler,
    [16U + CAN2_REC_IRQn]           = (uintptr_t)CAN2_REC_IRQHandler,
    [16U + EFUSE_IRQn]              = (uintptr_t)EFUSE_IRQHandler,
    [16U + I2C0_WKUP_IRQn]          = (uintptr_t)I2C0_WKUP_IRQHandler,
    [16U + I2C1_WKUP_IRQn]          = (uintptr_t)I2C1_WKUP_IRQHandler,
    [16U + I2C2_WKUP_IRQn]          = (uintptr_t)I2C2_WKUP_IRQHandler,
    [16U + I2C3_WKUP_IRQn]          = (uintptr_t)I2C3_WKUP_IRQHandler,
    [16U + LPDTS_IRQn]              = (uintptr_t)LPDTS_IRQHandler,
    [16U + LPDTS_WKUP_IRQn]         = (uintptr_t)LPDTS_WKUP_IRQHandler,
    [16U + TIMER0_DEC_IRQn]         = (uintptr_t)TIMER0_DEC_IRQHandler,
    [16U + TIMER7_DEC_IRQn]         = (uintptr_t)TIMER7_DEC_IRQHandler,
    [16U + TIMER1_DEC_IRQn]         = (uintptr_t)TIMER1_DEC_IRQHandler,
    [16U + TIMER2_DEC_IRQn]         = (uintptr_t)TIMER2_DEC_IRQHandler,
    [16U + TIMER3_DEC_IRQn]         = (uintptr_t)TIMER3_DEC_IRQHandler,
    [16U + TIMER4_DEC_IRQn]         = (uintptr_t)TIMER4_DEC_IRQHandler,
    [16U + TIMER22_DEC_IRQn]        = (uintptr_t)TIMER22_DEC_IRQHandler,
    [16U + TIMER23_DEC_IRQn]        = (uintptr_t)TIMER23_DEC_IRQHandler,
    [16U + TIMER30_DEC_IRQn]        = (uintptr_t)TIMER30_DEC_IRQHandler,
    [16U + TIMER31_DEC_IRQn]        = (uintptr_t)TIMER31_DEC_IRQHandler,
};
#pragma GCC diagnostic pop

/* Standard Newlib syscall stubs (weak so user code can override) */
__attribute__((weak)) void *_sbrk(ptrdiff_t incr)
{
    static uint8_t *heap_end = NULL;
    uint8_t *prev_heap_end;

    if (heap_end == NULL) {
        heap_end = (uint8_t *)&_end;
    }
    prev_heap_end = heap_end;
    heap_end += incr;
    return (void *)prev_heap_end;
}

__attribute__((weak)) int _close(int file) { (void)file; return -1; }
__attribute__((weak)) int _fstat(int file, void *st) { (void)file; (void)st; return 0; }
__attribute__((weak)) int _isatty(int file) { (void)file; return 1; }
__attribute__((weak)) int _lseek(int file, int ptr, int dir) { (void)file; (void)ptr; (void)dir; return 0; }
__attribute__((weak)) int _read(int file, char *ptr, int len) { (void)file; (void)ptr; (void)len; return 0; }
__attribute__((weak)) int _getpid(void) { return 1; }
__attribute__((weak)) int _kill(int pid, int sig) { (void)pid; (void)sig; return -1; }

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
    (void)file;
#if defined(USE_STDPERIPH_DRIVER) && defined(PIO_GD32H7_PRINTF_USART0)
    for (int i = 0; i < len; ++i) {
        if (ptr[i] == '\n') {
            usart_data_transmit(USART0, (uint8_t)'\r');
            while (RESET == usart_flag_get(USART0, USART_FLAG_TBE)) {}
        }
        usart_data_transmit(USART0, (uint8_t)ptr[i]);
        while (RESET == usart_flag_get(USART0, USART_FLAG_TBE)) {}
    }
#else
    (void)ptr;
#endif
    return len;
}


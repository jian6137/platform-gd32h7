/*!
    \file    exmc_sdram.h
    \brief   Header file of EXMC 32MB SDRAM driver for GD32H759I-EVAL

    \version 2025-02-19, V2.1.0, demo for GD32H7xx
*/

#ifndef EXMC_SDRAM_H
#define EXMC_SDRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32h7xx.h"

#define SDRAM_DEVICE0_ADDR                         ((uint32_t)0xC0000000U)
#define SDRAM_DEVICE1_ADDR                         ((uint32_t)0xD0000000U)

/* Initialize EXMC SDRAM peripheral (pass EXMC_SDRAM_DEVICE0 for onboard SDRAM) */
void exmc_synchronous_dynamic_ram_init(uint32_t sdram_device);

/* Convenience wrapper for EXMC_SDRAM_DEVICE0 */
void exmc_sdram_config(void);

/* Fill the buffer with specified value */
void fill_buffer(uint8_t *pbuffer, uint16_t buffer_lengh, uint16_t offset);

/* Write a byte buffer (data is 8 bits) to the EXMC SDRAM memory */
void sdram_writebuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t writeaddr, uint32_t numbytetowrite);

/* Read a block of 8-bit data from the EXMC SDRAM memory */
void sdram_readbuffer_8(uint32_t sdram_device, uint8_t *pbuffer, uint32_t readaddr, uint32_t numbytetoread);

#ifdef __cplusplus
}
#endif

#endif /* EXMC_SDRAM_H */


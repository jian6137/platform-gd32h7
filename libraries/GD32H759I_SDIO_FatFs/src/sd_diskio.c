/*!
    \file    sd_diskio.c
    \brief   FatFs diskio port for GD32H759I SDIO0 using official GigaDevice sdcard driver
*/

#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include <string.h>

static DSTATUS disk_state = STA_NOINIT;

DSTATUS disk_initialize(BYTE pdrv)
{
    if(pdrv != 0U) {
        return STA_NOINIT;
    }

    if(sd_io_init() == SD_OK) {
        disk_state &= ~STA_NOINIT;
    } else {
        disk_state |= STA_NOINIT;
    }

    return disk_state;
}

DSTATUS disk_status(BYTE pdrv)
{
    if(pdrv != 0U) {
        return STA_NOINIT;
    }
    return disk_state;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    sd_error_enum status = SD_OK;

    if(pdrv != 0U || buff == NULL || count == 0U || (disk_state & STA_NOINIT)) {
        return RES_PARERR;
    }

    if(((uintptr_t)buff & 0x3U) == 0U) {
        if(count == 1U) {
            status = sd_block_read((uint32_t *)(void *)buff, (uint32_t)sector, 512U);
        } else {
            status = sd_multiblocks_read((uint32_t *)(void *)buff, (uint32_t)sector, 512U, count);
        }
    } else {
        static uint32_t align_buf[128];
        while(count-- > 0U) {
            status = sd_block_read(align_buf, (uint32_t)sector, 512U);
            if(status != SD_OK) {
                break;
            }
            memcpy(buff, align_buf, 512U);
            buff += 512U;
            sector++;
        }
    }

    return (status == SD_OK) ? RES_OK : RES_ERROR;
}

#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    sd_error_enum status = SD_OK;

    if(pdrv != 0U || buff == NULL || count == 0U || (disk_state & STA_NOINIT)) {
        return RES_PARERR;
    }

    if(((uintptr_t)buff & 0x3U) == 0U) {
        if(count == 1U) {
            status = sd_block_write((uint32_t *)(void *)buff, (uint32_t)sector, 512U);
        } else {
            status = sd_multiblocks_write((uint32_t *)(void *)buff, (uint32_t)sector, 512U, count);
        }
    } else {
        static uint32_t align_buf[128];
        while(count-- > 0U) {
            memcpy(align_buf, buff, 512U);
            status = sd_block_write(align_buf, (uint32_t)sector, 512U);
            if(status != SD_OK) {
                break;
            }
            buff += 512U;
            sector++;
        }
    }

    return (status == SD_OK) ? RES_OK : RES_ERROR;
}
#endif

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if(pdrv != 0U || (disk_state & STA_NOINIT)) {
        return RES_NOTRDY;
    }

    switch(cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_COUNT:
        if(buff == NULL) {
            return RES_PARERR;
        }
        *(LBA_t *)buff = (LBA_t)sd_card_capacity_get() * 2U;
        return RES_OK;
    case GET_SECTOR_SIZE:
        if(buff == NULL) {
            return RES_PARERR;
        }
        *(WORD *)buff = 512U;
        return RES_OK;
    case GET_BLOCK_SIZE:
        if(buff == NULL) {
            return RES_PARERR;
        }
        *(DWORD *)buff = 1U;
        return RES_OK;
    default:
        return RES_PARERR;
    }
}
__attribute__((weak)) DWORD get_fattime(void)
{
    return ((DWORD)(2026 - 1980) << 25) | ((DWORD)1 << 21) | ((DWORD)1 << 16);
}

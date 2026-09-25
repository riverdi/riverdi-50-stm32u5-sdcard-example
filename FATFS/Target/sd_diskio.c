/**
  ******************************************************************************
  * @file    sd_diskio.c
  * @brief   FatFS disk I/O driver — STM32U5G9, polling mode
  ******************************************************************************
  *
  * Uses blocking HAL_SD_ReadBlocks / HAL_SD_WriteBlocks (polling, no DMA).
  * This is appropriate for a demo — for production, enable DMA and replace
  * this file with the DMA+RTOS template (sd_diskio_dma_rtos_template_bspv1.c).
  *
  * FreeRTOS: blocking HAL calls keep the task blocked but do not starve other
  * tasks because FreeRTOS will context-switch during HAL's internal SysTick
  * wait loops (as long as no critical sections are held).
  *
  ******************************************************************************
  */

#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include <string.h>

/* Transfer timeout (ms) */
#define SD_TIMEOUT          5000U
#define SD_DEFAULT_BLOCK_SIZE 512U

/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* -------------------------------------------------------------------------- */
/* Forward declarations                                                        */
/* -------------------------------------------------------------------------- */
static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS SD_initialize(BYTE lun);
DSTATUS SD_status(BYTE lun);
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
#endif
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff);
#endif

/* Driver registration table */
const Diskio_drvTypeDef SD_Driver =
{
    SD_initialize,
    SD_status,
    SD_read,
#if _USE_WRITE == 1
    SD_write,
#endif
#if _USE_IOCTL == 1
    SD_ioctl,
#endif
};

/* -------------------------------------------------------------------------- */
/* Private helpers                                                             */
/* -------------------------------------------------------------------------- */

static DSTATUS SD_CheckStatus(BYTE lun)
{
    Stat = STA_NOINIT;
    if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
    {
        Stat &= ~STA_NOINIT;
    }
    return Stat;
}

/* Wait until card is ready or timeout expires */
static int SD_WaitReady(void)
{
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < SD_TIMEOUT)
    {
        if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
            return 0;
        osDelay(1);   /* yield — let other FreeRTOS tasks run */
    }
    return -1;
}

/* -------------------------------------------------------------------------- */
/* FatFS driver functions                                                      */
/* -------------------------------------------------------------------------- */

/**
  * @brief  Initialise the SD drive.
  *         Called by f_mount(). Checks card presence first — safe without card.
  */
DSTATUS SD_initialize(BYTE lun)
{
    Stat = STA_NOINIT;

    /* BSP_SD_Init checks BSP_SD_IsDetected() before HAL_SD_Init() */
    if (BSP_SD_Init() == MSD_OK)
    {
        /* Wait for card to enter TRANSFER state after init */
        uint32_t start = HAL_GetTick();
        while (HAL_GetTick() - start < SD_TIMEOUT)
        {
            if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
            {
                Stat &= ~STA_NOINIT;
                break;
            }
            osDelay(1);
        }
    }

    return Stat;
}

/**
  * @brief  Return current disk status.
  */
DSTATUS SD_status(BYTE lun)
{
    return SD_CheckStatus(lun);
}

/**
  * @brief  Read sector(s).
  */
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    if (SD_WaitReady() < 0)
        return RES_ERROR;

    if (BSP_SD_ReadBlocks((uint32_t *)buff, (uint32_t)sector,
                          count, SD_TIMEOUT) != MSD_OK)
    {
        return RES_ERROR;
    }

    /* Wait for transfer to complete */
    if (SD_WaitReady() < 0)
        return RES_ERROR;

    return RES_OK;
}

#if _USE_WRITE == 1
/**
  * @brief  Write sector(s).
  */
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    if (SD_WaitReady() < 0)
        return RES_ERROR;

    if (BSP_SD_WriteBlocks((uint32_t *)buff, (uint32_t)sector,
                           count, SD_TIMEOUT) != MSD_OK)
    {
        return RES_ERROR;
    }

    /* Wait for transfer to complete */
    if (SD_WaitReady() < 0)
        return RES_ERROR;

    return RES_OK;
}
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
/**
  * @brief  IOCTL — disk control.
  */
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
    BSP_SD_CardInfo CardInfo;

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;

    switch (cmd)
    {
    case CTRL_SYNC:
        /* Polling mode: writes are synchronous — nothing to flush */
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        BSP_SD_GetCardInfo(&CardInfo);
        *(DWORD *)buff = CardInfo.LogBlockNbr;
        res = RES_OK;
        break;

    case GET_SECTOR_SIZE:
        BSP_SD_GetCardInfo(&CardInfo);
        *(WORD *)buff = CardInfo.LogBlockSize;
        res = RES_OK;
        break;

    case GET_BLOCK_SIZE:
        BSP_SD_GetCardInfo(&CardInfo);
        *(DWORD *)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
        res = RES_OK;
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
}
#endif /* _USE_IOCTL == 1 */

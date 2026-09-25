/* bsp_driver_sd.c — SD card BSP for STM32U5G9 (polling mode)
 * HAL_SD_MspInit() is in CubeMX-generated sdmmc.c, not here. */
#include "bsp_driver_sd.h"

extern SD_HandleTypeDef hsd1;  /* defined in CubeMX-generated sdmmc.c */

/* Retry init up to 3 times, clearing stale SDMMC flags before each attempt. */
uint8_t BSP_SD_Init(void)
{
    if (BSP_SD_IsDetected() != SD_PRESENT)
    {
        return MSD_ERROR_SD_NOT_PRESENT;
    }

    for (uint8_t attempt = 0; attempt < 3; attempt++)
    {
        /* Root cause of hot-swap failure: HAL_SD_DeInit clears only
         * SDMMC_STATIC_DATA_FLAGS, which does NOT include SDMMC_FLAG_CTIMEOUT
         * (a command flag).  After a CMD13 timeout during card-removal
         * detection, CTIMEOUT stays set in SDMMC->STA.  The next CMD8 in
         * HAL_SD_Init() sees it immediately and returns HAL_ERROR.
         * Fix: enable the peripheral clock and clear all command-path flags
         * (including CTIMEOUT) before every init attempt. */
        __HAL_RCC_SDMMC1_CLK_ENABLE();
        __SDMMC_CLEAR_FLAG(SDMMC1, SDMMC_FLAG_CTIMEOUT | SDMMC_FLAG_CCRCFAIL |
                                    SDMMC_FLAG_CMDREND  | SDMMC_FLAG_CMDSENT);

        hsd1.State                    = HAL_SD_STATE_RESET;
        hsd1.Instance                 = SDMMC1;
        hsd1.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
        hsd1.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
        hsd1.Init.BusWide             = SDMMC_BUS_WIDE_1B;
        hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
        hsd1.Init.ClockDiv            = 4;

        if (HAL_SD_Init(&hsd1) == HAL_OK)
        {
            return MSD_OK;
        }

        HAL_SD_DeInit(&hsd1);
        HAL_Delay(200);
    }

    return MSD_ERROR;
}

/* Clear stale SDMMC flags before deinit to avoid CTIMEOUT on next init. */
uint8_t BSP_SD_DeInit(void)
{
    __HAL_RCC_SDMMC1_CLK_ENABLE();
    __SDMMC_CLEAR_FLAG(SDMMC1, SDMMC_FLAG_CTIMEOUT | SDMMC_FLAG_CCRCFAIL |
                                SDMMC_FLAG_CMDREND  | SDMMC_FLAG_CMDSENT);
    HAL_SD_DeInit(&hsd1);
    return MSD_OK;
}

/**
  * @brief  Reads blocks in polling mode.
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr,
                          uint32_t NumOfBlocks, uint32_t Timeout)
{
    if (HAL_SD_ReadBlocks(&hsd1, (uint8_t *)pData,
                          ReadAddr, NumOfBlocks, Timeout) != HAL_OK)
    {
        return MSD_ERROR;
    }
    return MSD_OK;
}

/**
  * @brief  Writes blocks in polling mode.
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr,
                           uint32_t NumOfBlocks, uint32_t Timeout)
{
    if (HAL_SD_WriteBlocks(&hsd1, (uint8_t *)pData,
                           WriteAddr, NumOfBlocks, Timeout) != HAL_OK)
    {
        return MSD_ERROR;
    }
    return MSD_OK;
}

/**
  * @brief  Erases SD blocks.
  */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
    if (HAL_SD_Erase(&hsd1, StartAddr, EndAddr) != HAL_OK)
    {
        return MSD_ERROR;
    }
    return MSD_OK;
}

/* Returns SD_TRANSFER_OK or SD_TRANSFER_BUSY. */
uint8_t BSP_SD_GetCardState(void)
{
    return (HAL_SD_GetCardState(&hsd1) == HAL_SD_CARD_TRANSFER)
           ? SD_TRANSFER_OK
           : SD_TRANSFER_BUSY;
}

/**
  * @brief  Returns SD card geometry information.
  */
void BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo)
{
    HAL_SD_GetCardInfo(&hsd1, CardInfo);
}

/* Reads the CD GPIO pin via BSP_PlatformIsDetected(). */
uint8_t BSP_SD_IsDetected(void)
{
    return BSP_PlatformIsDetected();
}

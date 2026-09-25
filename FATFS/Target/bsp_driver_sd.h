/**
  ******************************************************************************
  * @file    bsp_driver_sd.h
  * @brief   SD card BSP driver interface — STM32U5G9 adaptation
  ******************************************************************************
  *
  * Thin BSP layer over HAL SD driver.
  * BSP_SD_Init() checks card presence before calling HAL_SD_Init(), so the
  * system does NOT crash or hang when no card is inserted.
  *
  ******************************************************************************
  */
#ifndef __BSP_DRIVER_SD_U5G9_H
#define __BSP_DRIVER_SD_U5G9_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx_hal.h"
#include "fatfs_platform.h"

/* SD Card information type */
#define BSP_SD_CardInfo   HAL_SD_CardInfoTypeDef

/* Return codes */
#define MSD_OK                    ((uint8_t)0x00)
#define MSD_ERROR                 ((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT  ((uint8_t)0x02)

/* Transfer state */
#define SD_TRANSFER_OK            ((uint8_t)0x00)
#define SD_TRANSFER_BUSY          ((uint8_t)0x01)

/* SD_DATATIMEOUT used in polling read/write (ticks) */
#define SD_DATATIMEOUT            ((uint32_t)5000)

/* BSP API */
uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_DeInit(void);
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr,
                          uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr,
                           uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t BSP_SD_GetCardState(void);
void    BSP_SD_GetCardInfo(BSP_SD_CardInfo *CardInfo);
uint8_t BSP_SD_IsDetected(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRIVER_SD_U5G9_H */

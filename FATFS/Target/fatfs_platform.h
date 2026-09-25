/**
  ******************************************************************************
  * @file           : fatfs_platform.h
  * @brief          : SD card detect pin configuration for STM32U5G9
  ******************************************************************************
  *
  * Card detect (CD) pin: PJ2, active LOW — card present = GPIO LOW.
  * Confirmed from Riverdi U5G9 schematic: SDIO_CD net routed to PJ2.
  *
  ******************************************************************************
  */
#ifndef __FATFS_PLATFORM_U5G9_H
#define __FATFS_PLATFORM_U5G9_H

#include "stm32u5xx_hal.h"

#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)

/* Card detect pin: PJ2, active LOW (pulled HIGH via R34 10k when no card) */
#define SD_DETECT_PIN            GPIO_PIN_2
#define SD_DETECT_GPIO_PORT      GPIOJ
#define SD_DETECT_GPIO_CLK_EN()  __HAL_RCC_GPIOJ_CLK_ENABLE()

void BSP_PlatformInit(void);
uint8_t BSP_PlatformIsDetected(void);

#endif /* __FATFS_PLATFORM_U5G9_H */

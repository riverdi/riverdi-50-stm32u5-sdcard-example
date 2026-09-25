/**
  ******************************************************************************
  * @file           : fatfs_platform.c
  * @brief          : SD card detect pin — STM32U5G9 adaptation
  ******************************************************************************
  *
  * PJ2 (SDIO_CD) is read as GPIO_INPUT with no internal pull — the board has
  * an external 10k pull-up (R34) to VCC.  Card present → PJ2 LOW.
  *
  * Call BSP_PlatformInit() once before using BSP_PlatformIsDetected(),
  * e.g. from main() before osKernelStart().
  *
  ******************************************************************************
  */
#include "fatfs_platform.h"

/**
  * @brief  Configure the SD card detect GPIO (PJ2).
  *         Must be called once after HAL_Init() and RCC clocks are enabled.
  */
void BSP_PlatformInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    SD_DETECT_GPIO_CLK_EN();

    GPIO_InitStruct.Pin  = SD_DETECT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    /*
     * Use GPIO_PULLUP if the board has no external pull-up on the CD pin.
     * Use GPIO_NOPULL  if there is an external pull-up resistor.
     */
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  Check whether SD card is physically present.
  * @retval SD_PRESENT  (0x01) — card inserted (PJ2 is LOW)
  *         SD_NOT_PRESENT (0x00) — no card (PJ2 is HIGH)
  */
uint8_t BSP_PlatformIsDetected(void)
{
    /* NOTE: The physical CD switch on this slot does not activate reliably.
     * The detection is therefore bypassed — BSP_SD_Init() will attempt
     * HAL_SD_Init() regardless, which fails gracefully if no card is present.
     * To restore hardware detection, replace the return below with:
     *   return (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) == GPIO_PIN_RESET)
     *          ? SD_PRESENT : SD_NOT_PRESENT;
     */
    return SD_PRESENT;
}

/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  FatFS application layer — STM32U5G9 adaptation
  ******************************************************************************
  *
  * Call MX_FATFS_Init() in main() before osKernelStart().
  * Do NOT call MX_SDMMC1_SD_Init() unconditionally in main() — SD is
  * initialised safely inside BSP_SD_Init() which checks card presence first.
  *
  ******************************************************************************
  */
#include "fatfs.h"
#include "rtc.h"

uint8_t retSD;      /* Return value from FATFS_LinkDriver */
char    SDPath[4];  /* Logical drive path e.g. "0:/" */
FATFS   SDFatFS;    /* FatFS work area — one per logical drive */
FIL     SDFile;     /* Default file handle (also used in sd_task.c) */

/**
  * @brief  Register the SD driver with FatFS.
  *         Must be called once before any f_mount() / f_open() calls.
  */
void MX_FATFS_Init(void)
{
    retSD = FATFS_LinkDriver(&SD_Driver, SDPath);
}

/**
  * @brief  Returns current RTC time packed in FAT DWORD format.
  *         Called by FatFS for file creation / modification timestamps.
  *
  *         FAT format:
  *           bits 31-25  year - 1980  (RTC Year is offset from 2000 → +20)
  *           bits 24-21  month  1-12
  *           bits 20-16  day    1-31
  *           bits 15-11  hour   0-23
  *           bits 10-5   minute 0-59
  *           bits  4-0   second/2  0-29
  */
DWORD get_fattime(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /*
     * On STM32U5 (and H7): GetTime must be called before GetDate to
     * unlock the shadow register — otherwise Date returns a stale value.
     */
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    return ((DWORD)(sDate.Year + 20) << 25)
         | ((DWORD)sDate.Month      << 21)
         | ((DWORD)sDate.Date       << 16)
         | ((DWORD)sTime.Hours      << 11)
         | ((DWORD)sTime.Minutes    << 5)
         | ((DWORD)sTime.Seconds    >> 1);
}

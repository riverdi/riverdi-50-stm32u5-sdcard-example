#include "sd_task.h"
#include "fatfs.h"
#include "bsp_driver_sd.h"
#include "fatfs_platform.h"
#include "rtc.h"
#include "cmsis_os.h"

#include <stdio.h>
#include <string.h>

volatile uint8_t sd_card_detected = 0;
volatile uint8_t sd_write_status = 0;
volatile FRESULT sd_result = FR_OK;

#define RTC_INIT_MAGIC  0xA55AU

static void SD_RTC_Init(void)
{
	RTC_TimeTypeDef time = {0};
	RTC_DateTypeDef date = {0};

	char month[4];
	unsigned int day, year;
	unsigned int hour, minute, second;

	//if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) == RTC_INIT_MAGIC)
	  //  return;

	sscanf(__DATE__, "%3s %u %u", month, &day, &year);
	sscanf(__TIME__, "%u:%u:%u", &hour, &minute, &second);

	date.Date = day;
	date.Year = year - 2000;

	time.Hours   = hour;
	time.Minutes = minute;
	time.Seconds = second;

	if (!strcmp(month, "Jan")) date.Month = 1;
	else if (!strcmp(month, "Feb")) date.Month = 2;
	else if (!strcmp(month, "Mar")) date.Month = 3;
	else if (!strcmp(month, "Apr")) date.Month = 4;
	else if (!strcmp(month, "May")) date.Month = 5;
	else if (!strcmp(month, "Jun")) date.Month = 6;
	else if (!strcmp(month, "Jul")) date.Month = 7;
	else if (!strcmp(month, "Aug")) date.Month = 8;
	else if (!strcmp(month, "Sep")) date.Month = 9;
	else if (!strcmp(month, "Oct")) date.Month = 10;
	else if (!strcmp(month, "Nov")) date.Month = 11;
	else if (!strcmp(month, "Dec")) date.Month = 12;

	date.WeekDay = RTC_WEEKDAY_MONDAY;

	HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, RTC_INIT_MAGIC);
}

void SD_Task(void *argument)
{
	char buffer[256];
	int length;
    FIL file;
    UINT bytes_written;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    FATFS fs;
    BYTE work[4096];



    MX_FATFS_Init();
    SD_RTC_Init();

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    length = snprintf(buffer,
                      sizeof(buffer),
                      "SD card write test - OK\r\n"
					  "To verify the write works: delete this file and power-cycle the board.\r\n"
					  "If the file reappears, SD card write is working correctly.\r\n");
    osDelay(400);


        if (BSP_SD_IsDetected() == SD_PRESENT)
        {
            if (!sd_card_detected)
            {


                sd_card_detected = 1;

                osDelay(500);

                sd_result = f_mount(&SDFatFS, SDPath, 1);

                if (sd_result == FR_NO_FILESYSTEM)
                {
                    sd_result = f_mkfs(SDPath, FM_ANY, 0, work, sizeof(work));

                    if (sd_result == FR_OK)
                        sd_result = f_mount(&fs, SDPath, 1);
                }


                if (sd_result == FR_OK)
                {
                    sd_result = f_open(&file,
                                       "0:/TEST.TXT",
                                       FA_CREATE_ALWAYS | FA_WRITE);

                    if (sd_result == FR_OK)
                    {


                        sd_result = f_write(&file,
                                            buffer,
                                            length,
                                            &bytes_written);

                        if (sd_result == FR_OK)
                            sd_result = f_close(&file);
                        else
                            f_close(&file);
                    }

                    f_mount(NULL, SDPath, 0);

                }

                sd_write_status = (sd_result == FR_OK) ? 1 : 2;
            }
        }
        else
        {
            sd_card_detected = 0;
            sd_write_status = 0;
        }

for (;;)
        {
        osDelay(500);
    }
}

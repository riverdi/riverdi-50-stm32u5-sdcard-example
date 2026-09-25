/**
  ******************************************************************************
  * @file    sd_task.h
  * @brief   SD card FreeRTOS task header — STM32U5G9
  ******************************************************************************
  */
#ifndef __SD_TASK_U5G9_H
#define __SD_TASK_U5G9_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint8_t sd_card_detected;
extern volatile uint8_t sd_write_status;  /* 0=idle, 1=ok, 2=error */
void SD_Task(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* __SD_TASK_U5G9_H */

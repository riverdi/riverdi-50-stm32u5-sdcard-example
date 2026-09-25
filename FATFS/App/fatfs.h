/**
  ******************************************************************************
  * @file   fatfs.h
  * @brief  FatFS application layer header — STM32U5G9
  ******************************************************************************
  */
#ifndef __FATFS_U5G9_H
#define __FATFS_U5G9_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

extern uint8_t retSD;
extern char    SDPath[4];
extern FATFS   SDFatFS;
extern FIL     SDFile;

void MX_FATFS_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __FATFS_U5G9_H */

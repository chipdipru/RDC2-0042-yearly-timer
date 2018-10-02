/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __RTC_H
#define __RTC_H


#include "RDC2_0041_board.h"
#include "DS3231.h"


#define       DS3231_I2C_ADDRESS           0x68


#define       RTC_IS_NOT_READY             0
#define       RTC_IS_READY                 1

#define       BYTES_FOR_TIME               7
#define       BYTES_FOR_DISPLAY            4

#define       DISPLAY_HOURS                (DS3231_YEAR_OFFSET + 1)
#define       DISPLAY_MINUTES              (DISPLAY_HOURS + 2)

#define       I2C_RST_ATTEMPTS             30


#define       RTC_INT_ACTIVE               1



uint8_t RTC_Init();

void RTC_SetTime(uint8_t *TimeData);

void RTC_ReadTime(uint8_t *TimeData);

void RTC_ISR(void);

uint8_t RTC_IsIntActive();

void RTC_ClearIntStatus();

void RTC_ReadStatus(uint8_t *Status);

void RTC_SetAlarm1(uint8_t *AlarmData);

void RTC_SetAlarm2(uint8_t *AlarmData);

void RTC_SetAlarm(uint8_t AlarmReg, uint8_t AlarmSize, uint8_t *AlarmData);

void RTC_EnableAlarm1();

void RTC_EnableAlarm2();

void RTC_DisableAlarm1();

void RTC_SetCtrlBit(uint8_t BitToSet);

void RTC_ClearCtrlBit(uint8_t BitToClear);

uint8_t BinToDec(uint8_t BinVal);

uint8_t DecToBin (uint8_t DecVal);


#endif //__RTC_H



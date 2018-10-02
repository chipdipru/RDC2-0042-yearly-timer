/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __DS3231_H
#define __DS3231_H


#define       DS3231_TIME_BASE_REG         0x00
#define       DS3231_ALARM1_BASE_REG       0x07
#define       DS3231_ALARM2_BASE_REG       0x0B
#define       DS3231_CONTROL_REG           0x0E
#define       DS3231_STATUS_REG            0x0F

#define       DS3231_SECONDS_OFFSET        0
#define       DS3231_MINUTES_OFFSET        (DS3231_SECONDS_OFFSET + 1)
#define       DS3231_HOURS_OFFSET          (DS3231_MINUTES_OFFSET + 1)
#define       DS3231_DAYS_OFFSET           (DS3231_HOURS_OFFSET + 1)
#define       DS3231_DATE_OFFSET           (DS3231_DAYS_OFFSET + 1)
#define       DS3231_MONTH_OFFSET          (DS3231_DATE_OFFSET + 1)
#define       DS3231_YEAR_OFFSET           (DS3231_MONTH_OFFSET + 1)


#define       DS3231_UNITS_MASK            0x0F
#define       DS3231_TENS_OFFSET           0x04
#define       DS3231_HOUR_MASK             0x3F
#define       DS3231_MONTH_MASK            0x1F

//control reg bits
#define       DS3231_A1IE                  (1 << 0)
#define       DS3231_A2IE                  (1 << 1)
#define       DS3231_INTCN                 (1 << 2)

//status reg bits
#define       DS3231_A1F                   (1 << 0)
#define       DS3231_A2F                   (1 << 1)
#define       DS3231_EN32kHz               (1 << 3)
#define       DS3231_OSF                   (1 << 7)

#define       DS3231_ALARM_MASK            (1 << 7)

#define       DS3231_ALARM_SEC_OFFSET      0
#define       DS3231_ALARM_MIN_OFFSET      1
#define       DS3231_ALARM_HOUR_OFFSET     2
#define       DS3231_ALARM_DAY_OFFSET      3

#define       ALARM_1_SIZE                 4
#define       ALARM_2_SIZE                 3


#endif //__DS3231_H




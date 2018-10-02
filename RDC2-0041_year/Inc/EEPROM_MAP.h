/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __EEPROM_MAP_H
#define __EEPROM_MAP_H



#define       EEPROM_PAGE_SIZE             32 //размер станицы EEPROM

//разметка памяти
#define       SYSTEM_DATA_START_ADDRESS    (1 * EEPROM_PAGE_SIZE)
#define       DAYS_DATA_START_ADDRESS      (3 * EEPROM_PAGE_SIZE) //информация о кол-ве событий в каждом дне
#define       EVENTS_DATA_START_ADDRESS    (32 * EEPROM_PAGE_SIZE)
#define       EVENTS_DATA_PER_DAY_SIZE     (32 * EEPROM_PAGE_SIZE)
#define       TIME_ZONE_ADDRESS            SYSTEM_DATA_START_ADDRESS
#define       TOTAL_EVENTS_COUNT_ADDRESS   (TIME_ZONE_ADDRESS + 8)
#define       TOTAL_EVENTS_COUNT_SIZE      2
#define       YEAR_EVENTS_START_ADDRESS    (SYSTEM_DATA_START_ADDRESS + EEPROM_PAGE_SIZE)

//свойства события
#define       EVENT_HOURS_SIZE             1
#define       EVENT_MINUTES_SIZE           1
#define       EVENT_RELAY_STATE_SIZE       1
#define       EVENT_MONTH_SIZE             1
#define       EVENT_MONTH_DAY_SIZE         1
#define       EVENT_RESERVED_SIZE          0
#define       EVENT_SIZE                   (EVENT_HOURS_SIZE + EVENT_MINUTES_SIZE + EVENT_RELAY_STATE_SIZE + \
                                            EVENT_MONTH_SIZE + EVENT_MONTH_DAY_SIZE + EVENT_RESERVED_SIZE)

#define       EVENT_HOURS_OFFSET           0
#define       EVENT_MINUTES_OFFSET         (EVENT_HOURS_OFFSET + EVENT_HOURS_SIZE)
#define       EVENT_RELAY_STATE_OFFSET     (EVENT_MINUTES_OFFSET + EVENT_MINUTES_SIZE)
#define       EVENT_MONTH_OFFSET           (EVENT_RELAY_STATE_OFFSET + EVENT_RELAY_STATE_SIZE)
#define       EVENT_MONTH_DAY_OFFSET       (EVENT_MONTH_OFFSET + EVENT_MONTH_SIZE)

#define       EVENTS_PER_EEPROM_PAGE       (EEPROM_PAGE_SIZE / EVENT_SIZE)


#endif //__EEPROM_MAP_H


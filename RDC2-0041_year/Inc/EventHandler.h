/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __EVENT_HANDLER_H
#define __EVENT_HANDLER_H


#include "stdint.h"


#define       EVENTS_COUNT_MAX             1524

#define       EVENT_EXISTS                 1
#define       EVENT_DONE                   1
#define       RELAY_OFF                    0


typedef struct
{
  uint8_t Hours;       //часы события
  uint8_t Minutes;     //минуты события
  uint8_t RelayState;  //состояние реле
  uint8_t Month;       //месяц
  uint8_t Day;         //день месяца
 uint16_t Number;      //номер
  
} EventType;

typedef struct
{
  uint8_t EventsCount; //количество событий в дне
  uint8_t PendingEvent; //номер ожидаемого события
    
} DayType;



void SetEventState(uint8_t EventState);

uint8_t PrevEventNum(uint16_t *EventNum);

uint8_t NextEventNum(uint16_t *EventNum);

void SetEventsCount(uint16_t NewCount);

uint16_t GetEventsCount();

void SetEventData(EventType *EventData);

uint8_t ExecuteEvent(uint8_t MonthNow);


#endif //__EVENT_HANDLER_H



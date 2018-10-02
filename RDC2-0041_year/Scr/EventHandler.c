/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "EventHandler.h"
#include "LED.h"


static EventType Event;
static EventType* EventPtr = &Event;
static uint16_t YearEventsCount = 0;


void SetEventState(uint8_t EventState)
{
  if (EventState == RELAY_OFF)
  {
    RELAY_GPIO->ODR &= ~(1 << RELAY_PIN);
    LED_OFF();
  }
  else
  {
    RELAY_GPIO->ODR |= 1 << RELAY_PIN;
    LED_ON();
  }
}
//------------------------------------------------------------------------------
uint8_t PrevEventNum(uint16_t *EventNum)
{
  if (EventPtr->Number == EVENTS_COUNT_MAX)
    return 0;
  
  else if (EventPtr->Number != 0)
  {
    *EventNum = EventPtr->Number - 1;
    return EVENT_EXISTS;
  }
  
  else
  {
    *EventNum = YearEventsCount - 1;
    return EVENT_EXISTS;
  }
}
//------------------------------------------------------------------------------
uint8_t NextEventNum(uint16_t *EventNum)
{  
  if (EventPtr->Number == EVENTS_COUNT_MAX)
    return 0;
  
  *EventNum = EventPtr->Number + 1;
  if (*EventNum == YearEventsCount)
    *EventNum = 0;
  return EVENT_EXISTS;
}
//------------------------------------------------------------------------------
uint8_t ExecuteEvent(uint8_t MonthNow)
{
  if ((EventPtr->Number < EVENTS_COUNT_MAX)
   && (EventPtr->Month == MonthNow))
  {
    SetEventState(EventPtr->RelayState);
    return EVENT_DONE;
  }
  
  return 0;
}
//------------------------------------------------------------------------------
void SetEventsCount(uint16_t NewCount)
{
  YearEventsCount = NewCount;
}
//------------------------------------------------------------------------------
uint16_t GetEventsCount()
{
  return YearEventsCount;
}
//------------------------------------------------------------------------------
void SetEventData(EventType *EventData)
{
  EventPtr->Hours = EventData->Hours;
  EventPtr->Minutes = EventData->Minutes;
  EventPtr->RelayState = EventData->RelayState;
  EventPtr->Month = EventData->Month;
  EventPtr->Day = EventData->Day;
  EventPtr->Number = EventData->Number;  
}

  
  
  
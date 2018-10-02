/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "RDC2_0041_board.h"
#include "RDC2_0041_USB.h"
#include "SPI_EEPROM.h"
#include "Display.h"
#include "Key.h"
#include "EEPROM_MAP.h"
#include "RTC.h"
#include "LED.h"


void (*DataForDisplay)(uint8_t *DisData);
static volatile uint8_t State = 0;
static EventType TimeLeft;
static EventType *TimeLeftPtr = &TimeLeft;


int main()
{
  RDC2_0041_Init();
  
  //SCB->SCR = SCB_SCR_SLEEPONEXIT_Msk;
  
  for(;;)
  {
    //__WFI();
    
    if ((*RDC2_0041_USB_GetStatus()) != RDC2_0041_USB_IDLE)
    {
      uint8_t *NewData = (uint8_t *)RDC2_0041_USB_GetPacket();
      uint8_t USBFlagToClear = 0;
      
      if (((*RDC2_0041_USB_GetStatus()) & RDC2_0041_USB_EEPROM_READ) == RDC2_0041_USB_EEPROM_READ)
      {      
        ReadSettings(NewData);      
        RDC2_0041_USB_WhileNotReadyToSend();
        RDC2_0041_USB_SendData(NewData);        
        USBFlagToClear = RDC2_0041_USB_EEPROM_READ;
      }
      
      else if (((*RDC2_0041_USB_GetStatus()) & RDC2_0041_USB_EEPROM_WRITE) == RDC2_0041_USB_EEPROM_WRITE)
      {           
        if ((*(NewData + USB_START_STOP_FLAG_POS)) == USB_CMD_START_FLAG)
        {
          RTC_DisableAlarm1();
          RTC_ClearIntStatus();
        }
        
        WriteSettings(NewData);
        
        if ((*(NewData + USB_START_STOP_FLAG_POS)) == USB_CMD_STOP_FLAG)
        {
          uint8_t RTCtime[BYTES_FOR_TIME + BYTES_FOR_DISPLAY];
          RTC_ReadTime(RTCtime);
          SyncEvents(RTCtime);
          
          if (DataForDisplay == NextEventTimeLeft)
            State |= DISPLAY_UPDATE_PENDING;
        }
        
        USBFlagToClear = RDC2_0041_USB_EEPROM_WRITE;
      }
      
      else if (((*RDC2_0041_USB_GetStatus()) & RDC2_0041_USB_TIME) == RDC2_0041_USB_TIME)
      {
        if ((*(NewData + USB_TIME_SUBCMD_POS)) == TIME_SUBCMD_SET)
        {
          SyncTime(&NewData[USB_TIME_DATA_POS]);
          RTC_ReadTime(NewData);
          SyncEvents(NewData);  
          State |= DISPLAY_UPDATE_PENDING;
        }
        
        else if ((*(NewData + USB_TIME_SUBCMD_POS)) == TIME_SUBCMD_GET)
        {
          GetTime(&NewData[USB_TIME_DATA_POS]);         
          RDC2_0041_USB_WhileNotReadyToSend();
          RDC2_0041_USB_SendData(NewData);
        }
        
        USBFlagToClear = RDC2_0041_USB_TIME;
      }
      
      RDC2_0041_USB_ClearStatus(USBFlagToClear);
    }
    
    if (RTC_IsIntActive() == RTC_INT_ACTIVE)
    {
      RTCIntHandler();
      RTC_ClearIntStatus();
    }
    
    if ((State & DISPLAY_UPDATE_PENDING) == DISPLAY_UPDATE_PENDING)
    {
      State &= ~DISPLAY_UPDATE_PENDING;
      
      uint8_t RTCtime[BYTES_FOR_TIME + BYTES_FOR_DISPLAY];
      RTC_ReadTime(RTCtime);
      DataForDisplay(RTCtime);
      Display_SetValue(&RTCtime[DISPLAY_HOURS]);  
    }
  }
}
//------------------------------------------------------------------------------
void ReadSettings(uint8_t *DataBuffer)
{
  EEPROM_WhileNotReady();
        
  if ((*(DataBuffer + USB_EEPROM_SUBCMD_POS)) == EEPROM_SUBCMD_EVENTS_COUNT)
  {
    EEPROM_Read(TOTAL_EVENTS_COUNT_SIZE, TOTAL_EVENTS_COUNT_ADDRESS, &DataBuffer[USB_ACTIVE_EVENT_POS]);
  }
               
  else if ((*(DataBuffer + USB_EEPROM_SUBCMD_POS)) == EEPROM_SUBCMD_EVENTS_DATA)
  {
    uint16_t StartEventNum = (DataBuffer[USB_ACTIVE_EVENT_POS + 1] << 8) | DataBuffer[USB_ACTIVE_EVENT_POS];    
    uint16_t StartAddress = GetEventAddress(StartEventNum);
    
    EEPROM_Read(DataBuffer[USB_EVENTS_IN_PACKET_POS] * USB_EVENT_SIZE, StartAddress, &DataBuffer[USB_EVENTS_DATA_POS]);
  }
}
//------------------------------------------------------------------------------
void WriteSettings(uint8_t *DataBuffer)
{
  EEPROM_WhileNotReady();
  
  if ((*(DataBuffer + USB_EEPROM_SUBCMD_POS)) == EEPROM_SUBCMD_EVENTS_COUNT)
  {    
    EEPROM_Write(TOTAL_EVENTS_COUNT_SIZE, TOTAL_EVENTS_COUNT_ADDRESS, &DataBuffer[USB_ACTIVE_EVENT_POS]);    
  }
        
  else if ((*(DataBuffer + USB_EEPROM_SUBCMD_POS)) == EEPROM_SUBCMD_EVENTS_DATA)
  {
    uint16_t StartEventNum = (DataBuffer[USB_ACTIVE_EVENT_POS + 1] << 8) | DataBuffer[USB_ACTIVE_EVENT_POS];    
    uint16_t StartAddress = GetEventAddress(StartEventNum);
    
    EEPROM_Write(DataBuffer[USB_EVENTS_IN_PACKET_POS] * USB_EVENT_SIZE, StartAddress, &DataBuffer[USB_EVENTS_DATA_POS]);     
  }
}
//------------------------------------------------------------------------------
void SyncTime(uint8_t *DataBuffer)
{
  RTC_DisableAlarm1();
  RTC_SetTime(DataBuffer);
  EEPROM_WhileNotReady();
  EEPROM_Write(1, TIME_ZONE_ADDRESS, &DataBuffer[BYTES_FOR_TIME]);  
}
//------------------------------------------------------------------------------
void GetTime(uint8_t *DataBuffer)
{
  RTC_ReadTime(DataBuffer);
  EEPROM_WhileNotReady();
  EEPROM_Read(1, TIME_ZONE_ADDRESS, &DataBuffer[BYTES_FOR_TIME]);
}
//------------------------------------------------------------------------------
void RDC2_0041_Init()
{
  //HSI, PLL, 48 MHz
  FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY;  
  // частота PLL: (HSI / 2) * 12 = (8 / 2) * 12 = 48 (МГц)
  RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12);

  RCC->CR |= RCC_CR_PLLON;
  while((RCC->CR & RCC_CR_PLLRDY) == 0);
 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL);
  
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN  | RCC_AHBENR_GPIOFEN;
  
  RELAY_GPIO->MODER |= (1 << (2 * RELAY_PIN));
  
  //LED
  LED_Init();  
  //EEPROM  
  if (EEPROM_Init() != EEPROM_PRESENT)
    ErrorHandler(ERROR_EEPROM);
  //RTC
  if (RTC_Init() != RTC_IS_READY)
    ErrorHandler(ERROR_RTC);
  
  uint8_t Alarm2Time[] = {DS3231_ALARM_MASK, DS3231_ALARM_MASK, DS3231_ALARM_MASK};
  RTC_SetAlarm2(Alarm2Time); //каждую минуту
  RTC_EnableAlarm2();
  
  //Display
  Display_Init(CLOCK_POINT_POSITION, CLOCK_POINT_DURATION);
  DataForDisplay = EmptyFunction;
  
  uint8_t RTCtime[BYTES_FOR_TIME + BYTES_FOR_DISPLAY];
  RTC_ReadTime(RTCtime);
  SyncEvents(RTCtime);
  DataForDisplay(RTCtime);
  Display_SetValue(&RTCtime[DISPLAY_HOURS]);

  //USB
  RDC2_0041_USB_Init();
  //кнопка
  Key_Init(&ChangeDisplayState);
  
  Display_ON();
  Display_SetState(DISPLAY_IS_ON_TIME);
}
//------------------------------------------------------------------------------
void ReadEventData(uint16_t EventNum, EventType* EventPtr)
{
  uint8_t DataToRead[EVENT_SIZE - EVENT_RESERVED_SIZE];  
  uint16_t StartAddress = GetEventAddress(EventNum);
    
  EEPROM_Read(EVENT_SIZE - EVENT_RESERVED_SIZE, StartAddress, DataToRead);
  
  EventPtr->Hours = *(DataToRead + EVENT_HOURS_OFFSET);
  EventPtr->Minutes = *(DataToRead + EVENT_MINUTES_OFFSET);
  EventPtr->RelayState = *(DataToRead + EVENT_RELAY_STATE_OFFSET);
  EventPtr->Month = *(DataToRead + EVENT_MONTH_OFFSET);
  EventPtr->Day = *(DataToRead + EVENT_MONTH_DAY_OFFSET);
  EventPtr->Number = EventNum;
}
//------------------------------------------------------------------------------
uint16_t GetEventAddress(uint16_t EventNum)
{
  return (YEAR_EVENTS_START_ADDRESS + (EventNum / EVENTS_PER_EEPROM_PAGE) * EEPROM_PAGE_SIZE + (EventNum % EVENTS_PER_EEPROM_PAGE) * EVENT_SIZE);
}
//------------------------------------------------------------------------------
void RTCIntHandler()
{
  uint8_t RTCstatus;
    
  RTC_ReadStatus(&RTCstatus);
  TimeLeftUpdate();
  
  uint8_t RTCtime[BYTES_FOR_TIME + BYTES_FOR_DISPLAY];
  RTC_ReadTime(RTCtime);
  
  if ((RTCstatus & DS3231_A1F) == DS3231_A1F)//настпуило событие
  {
    RTC_DisableAlarm1();
    
    if (ExecuteEvent(RTCtime[DS3231_MONTH_OFFSET]) == EVENT_DONE)
    {
      uint16_t NextEvent;
      if (NextEventNum(&NextEvent) == EVENT_EXISTS)
      {
        EventType EventData;
        ReadEventData(NextEvent, &EventData);
        SetEventData(&EventData);
        ActivateEvent(RTCtime, &EventData);
      }      
    }
    
    else
      RTC_EnableAlarm1();
  }
  
  DataForDisplay(RTCtime);
  Display_SetValue(&RTCtime[DISPLAY_HOURS]);
}
//------------------------------------------------------------------------------
void ActivateEvent(uint8_t *Time, EventType* EventPtr)
{
  uint32_t MinutesNow = (DaysFromYearStart(Time[DS3231_YEAR_OFFSET], Time[DS3231_MONTH_OFFSET], Time[DS3231_DATE_OFFSET]) * 24 + Time[DS3231_HOURS_OFFSET]) * 60 + Time[DS3231_MINUTES_OFFSET];
  uint32_t MinBeforeEvent;
  
  if (EventPtr->Number < GetEventsCount())
  {
    uint8_t AlarmTime[ALARM_1_SIZE];
    AlarmTime[DS3231_ALARM_SEC_OFFSET] = 0;
    AlarmTime[DS3231_ALARM_MIN_OFFSET] = EventPtr->Minutes;
    AlarmTime[DS3231_ALARM_HOUR_OFFSET] = EventPtr->Hours;
    AlarmTime[DS3231_ALARM_DAY_OFFSET] = EventPtr->Day;
    
    RTC_SetAlarm1(AlarmTime);
    RTC_EnableAlarm1();
    
    uint32_t EventTimeInMin = (DaysFromYearStart(Time[DS3231_YEAR_OFFSET], EventPtr->Month, EventPtr->Day) * 24 + EventPtr->Hours) * 60 + EventPtr->Minutes;
    if (EventTimeInMin < MinutesNow)
    {
      EventTimeInMin = (DaysFromYearStart(Time[DS3231_YEAR_OFFSET] + 1, EventPtr->Month, EventPtr->Day) * 24 + EventPtr->Hours) * 60 + EventPtr->Minutes;
      MinBeforeEvent = EventTimeInMin + (DaysFromYearStart(Time[DS3231_YEAR_OFFSET], 12, 31) + 1) * 24 * 60 - MinutesNow;
    }
    
    else
    {
      MinBeforeEvent = EventTimeInMin - MinutesNow;    
    }
  }
  
  else
    MinBeforeEvent = NO_EVENT_CODE;
  
  TimeLeftSet(MinBeforeEvent); 
}
//------------------------------------------------------------------------------
void SyncEvents(uint8_t *Time)
{
  uint8_t EventsCount[TOTAL_EVENTS_COUNT_SIZE];    
  EEPROM_WhileNotReady();
  EEPROM_Read(TOTAL_EVENTS_COUNT_SIZE, TOTAL_EVENTS_COUNT_ADDRESS, EventsCount);
  
  uint16_t CountValue = (EventsCount[1] << 8) | EventsCount[0];
  if (CountValue > EVENTS_COUNT_MAX)
    CountValue = 0;
  
  SetEventsCount(CountValue);  
  EventType EventData;
  FindPendingEvent(Time, CountValue, &EventData);  
  SetPreviousEvent();
  ActivateEvent(Time, &EventData);
}
//------------------------------------------------------------------------------
void FindPendingEvent(uint8_t *TimeNow, uint16_t EventsCount, EventType* EventData)
{
  uint8_t MonthNow = TimeNow[DS3231_MONTH_OFFSET];
  uint8_t DateNow = TimeNow[DS3231_DATE_OFFSET];
  uint8_t HourNow = TimeNow[DS3231_HOURS_OFFSET];
  uint8_t MinuteNow = TimeNow[DS3231_MINUTES_OFFSET];
  uint8_t YearNow = TimeNow[DS3231_YEAR_OFFSET];
  
  uint32_t TimeInMinNow = (DaysFromYearStart(YearNow, MonthNow, DateNow) * 24 + HourNow) * 60 + MinuteNow;
  
  uint8_t EventResult = 0;
    
  if (EventsCount != 0)
  {
    for (uint16_t EventNum = 0; EventNum < EventsCount; EventNum++)
    {      
      ReadEventData(EventNum, EventData);
    
      uint32_t EventTimeInMin = (DaysFromYearStart(YearNow, EventData->Month, EventData->Day) * 24 + EventData->Hours) * 60 + EventData->Minutes;
    
      if (EventTimeInMin > TimeInMinNow)
      {
        EventResult = EVENT_EXISTS;
        break;
      }
    }
    
    if (EventResult != EVENT_EXISTS)
      ReadEventData(0, EventData);
  }
  
  else
    EventData->Number = EVENTS_COUNT_MAX;
  
  SetEventData(EventData);
}
//------------------------------------------------------------------------------
uint16_t DaysFromYearStart(uint8_t Year, uint8_t Month, uint8_t Date)
{
  uint16_t DaysSum = 0;
  
  for (uint8_t MonthIndex = 0; MonthIndex < (Month - 1); MonthIndex ++)
    DaysSum += MONTH_DAYS_COUNT[MonthIndex];
  
  if ((Month > 2) && ((Year % 4) == 0)) //если високосный год и февраль прошел
    DaysSum++;
  
  DaysSum += Date - 1;
  
  return DaysSum;
}
//------------------------------------------------------------------------------
void SetPreviousEvent()
{
  uint16_t EventNum = 0;
  
  if (PrevEventNum(&EventNum) == EVENT_EXISTS)
  {
    EventType PrevEvent;
    ReadEventData(EventNum, &PrevEvent);
    SetEventState(PrevEvent.RelayState);
  }
  else
    SetEventState(RELAY_OFF);
}
//------------------------------------------------------------------------------
void ChangeDisplayState()
{
  switch(Display_State())
  {
    case DISPLAY_IS_OFF:
      Display_ON(); //время
      Display_SetState(DISPLAY_IS_ON_TIME);
      Display_PointSet(CLOCK_POINT_POSITION, CLOCK_POINT_DURATION);
      DataForDisplay = EmptyFunction;
      State |= DISPLAY_UPDATE_PENDING;
    break;
    
    case DISPLAY_IS_ON_TIME:
      Display_SetState(DISPLAY_IS_ON_EVENT);      
      DataForDisplay = NextEventTimeLeft;
      State |= DISPLAY_UPDATE_PENDING;
    break;
    
    case DISPLAY_IS_ON_EVENT:
      Display_OFF();
      Display_SetState(DISPLAY_IS_OFF);
      DataForDisplay = EmptyFunction;
    break;
  }  
}
//------------------------------------------------------------------------------
void EmptyFunction(uint8_t *ClockData)
{
  
}
//------------------------------------------------------------------------------
void TimeLeftSet(uint32_t TimeInMin)
{
  if (TimeInMin == NO_EVENT_CODE)  
    TimeLeftPtr->Number = EVENTS_COUNT_MAX;
  
  else
  {
    TimeLeftPtr->Number = 0;
    
    uint16_t TimePart = TimeInMin / 60 / 24; //дни
    TimeLeftPtr->Month = TimePart / TIME_LEFT_MONTH_DAYS_COUNT;
    TimeLeftPtr->Day = TimePart % TIME_LEFT_MONTH_DAYS_COUNT;
    TimePart = TimeInMin - TimePart * 24 * 60; //минуты
    TimeLeftPtr->Hours = TimePart / 60;
    TimeLeftPtr->Minutes = TimePart % 60;
  }
}
//------------------------------------------------------------------------------
void TimeLeftUpdate()
{
  if (TimeLeftPtr->Number != EVENTS_COUNT_MAX)
  {
    if (TimeLeftPtr->Minutes != 0)
      TimeLeftPtr->Minutes--;
    else
    {
      TimeLeftPtr->Minutes = 59;
      if (TimeLeftPtr->Hours != 0)
        TimeLeftPtr->Hours--;
      else
      {
        TimeLeftPtr->Hours = 23;
        if (TimeLeftPtr->Day != 0)
          TimeLeftPtr->Day--;
        else
        {
          TimeLeftPtr->Day = TIME_LEFT_MONTH_DAYS_COUNT - 1;
          if (TimeLeftPtr->Month != 0)
            TimeLeftPtr->Month--;
        }
      }
    }
  }  
}
//------------------------------------------------------------------------------
void NextEventTimeLeft(uint8_t *EventData)
{
  uint8_t PointPos = 1;
  
  if (TimeLeftPtr->Number != EVENTS_COUNT_MAX)
  { 
    uint16_t DaysLeft = TIME_LEFT_MONTH_DAYS_COUNT * TimeLeftPtr->Month + TimeLeftPtr->Day;
    
    if (DaysLeft != 0)
    {
      (*(EventData + DISPLAY_HOURS)) = DaysLeft / 100;
      (*(EventData + DISPLAY_HOURS + 1)) = (DaysLeft % 100) / 10;
      (*(EventData + DISPLAY_MINUTES)) = (DaysLeft % 100) % 10;
      (*(EventData + DISPLAY_MINUTES + 1)) = CHAR_d_INDEX;
      
      if ((*(EventData + DISPLAY_HOURS)) == 0)
      {
        (*(EventData + DISPLAY_HOURS)) = CHAR_SPACE_INDEX;
        
        if ((*(EventData + DISPLAY_HOURS + 1)) == 0)
        (*(EventData + DISPLAY_HOURS + 1)) = CHAR_SPACE_INDEX;
      }
      
      PointPos = 2;
    }
    
    else
    {
      (*(EventData + DISPLAY_HOURS)) = TimeLeftPtr->Hours / 10;
      (*(EventData + DISPLAY_HOURS + 1)) = TimeLeftPtr->Hours % 10;
      (*(EventData + DISPLAY_MINUTES)) = TimeLeftPtr->Minutes / 10;
      (*(EventData + DISPLAY_MINUTES + 1)) = TimeLeftPtr->Minutes % 10;
    }
  }
  
  else
  {
    (*(EventData + DISPLAY_HOURS)) = CHAR_LINE_INDEX;
    (*(EventData + DISPLAY_HOURS + 1)) = CHAR_LINE_INDEX;
    (*(EventData + DISPLAY_MINUTES)) = CHAR_LINE_INDEX;
    (*(EventData + DISPLAY_MINUTES + 1)) = CHAR_LINE_INDEX;
    PointPos = DIGITS_NUM;//точка отключена
  }

  Display_PointSet(PointPos, EVENT_POINT_DURATION);
}
//------------------------------------------------------------------------------
void ErrorHandler(uint8_t ErrorType)
{
  RELAY_GPIO->ODR &= ~(1 << RELAY_PIN);
  
  uint8_t LEDfreq = 1;
      
  switch(ErrorType)
  {
    case ERROR_EEPROM:
      LEDfreq = EEPROM_ERROR_LED_FREQ;
    break;
    
    case ERROR_RTC:
      LEDfreq = RTC_ERROR_LED_FREQ;
    break;
    
    default:
    break;
  }
  
  LED_ONwithFreq(LEDfreq);
  for(;;);
}


/*
********************************************************************************
* COPYRIGHT(c) ��� ���� � ��ϻ, 2018
* 
* ����������� ����������� ��������������� �� �������� ���� ����� (as is).
* ��� ��������������� �������� ������ �����������.
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
  uint8_t Hours;       //���� �������
  uint8_t Minutes;     //������ �������
  uint8_t RelayState;  //��������� ����
  uint8_t Month;       //�����
  uint8_t Day;         //���� ������
 uint16_t Number;      //�����
  
} EventType;

typedef struct
{
  uint8_t EventsCount; //���������� ������� � ���
  uint8_t PendingEvent; //����� ���������� �������
    
} DayType;



void SetEventState(uint8_t EventState);

uint8_t PrevEventNum(uint16_t *EventNum);

uint8_t NextEventNum(uint16_t *EventNum);

void SetEventsCount(uint16_t NewCount);

uint16_t GetEventsCount();

void SetEventData(EventType *EventData);

uint8_t ExecuteEvent(uint8_t MonthNow);


#endif //__EVENT_HANDLER_H



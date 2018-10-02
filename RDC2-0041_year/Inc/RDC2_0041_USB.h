/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2017
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/




#ifndef __RDC2_0041_USB_H
#define __RDC2_0041_USB_H

#include "RDC2_0041_board.h"


#define       USB_MESSAGE_LENGTH           0x40 //длина пакета USB, максимум 64 байта для USB HID


#define       USB_CMD_POS                  0x01 //индекс команды в посылке


//команды
#define       USB_CMD_EEPROM_WRITE         0
#define       USB_CMD_EEPROM_READ          1
#define       USB_CMD_TIME                 2


#define       EEPROM_SUBCMD_EVENTS_COUNT   0xC0
#define       EEPROM_SUBCMD_EVENTS_DATA    0xC1
#define       TIME_SUBCMD_SET              0xC5
#define       TIME_SUBCMD_GET              0xC6

#define       USB_EEPROM_SUBCMD_POS        2
#define       USB_ACTIVE_EVENT_POS         (USB_EEPROM_SUBCMD_POS + 1)
#define       USB_EVENTS_IN_PACKET_POS     (USB_ACTIVE_EVENT_POS + 2)
#define       USB_EVENTS_DATA_POS          (USB_EVENTS_IN_PACKET_POS + 1)

#define       USB_EVENT_HOUR_OFFSET        0
#define       USB_EVENT_MINUTE_OFFSET      1
#define       USB_EVENT_STATE_OFFSET       2
#define       USB_EVENT_MONTH_OFFSET       3
#define       USB_EVENT_DAY_OFFSET         4
#define       USB_EVENT_SIZE               5

#define       USB_TIME_SUBCMD_POS          USB_EEPROM_SUBCMD_POS

#define       USB_TIME_DATA_POS            (USB_TIME_SUBCMD_POS + 1)
#define       USB_START_STOP_FLAG_POS      63

#define       USB_CMD_START_FLAG           0x55
#define       USB_CMD_STOP_FLAG            0xA0


//Status
#define       RDC2_0041_USB_IDLE           0
#define       RDC2_0041_USB_EEPROM_WRITE   (1 << 1)
#define       RDC2_0041_USB_EEPROM_READ    (1 << 2)
#define       RDC2_0041_USB_TIME           (1 << 3)

#define       USB_EVENT_IN_PACKET_MAX      6



void RDC2_0041_USB_Init();

void RDC2_0041_USB_RecPacket(uint8_t *Packet);

void RDC2_0041_USB_SendData(uint8_t *Data);

volatile uint8_t* RDC2_0041_USB_GetStatus();

void RDC2_0041_USB_ClearStatus(uint8_t StatusFlag);

void RDC2_0041_USB_SetStatus(uint8_t StatusFlag);

volatile uint8_t* RDC2_0041_USB_GetPacket();

void RDC2_0041_USB_WhileNotReadyToSend();


#endif //__RDC2_0041_USB_H



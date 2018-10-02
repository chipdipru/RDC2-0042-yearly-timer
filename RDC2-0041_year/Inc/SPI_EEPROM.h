/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __SPI_EEPROM_H
#define __SPI_EEPROM_H


#include "RDC2_0041_board.h"



#define       SPI1_DR_8BIT                 (*(__IO uint8_t *)((uint32_t)&(SPI1->DR)))

//команды
#define       EEPROM_WRITE_ENABLE          0x06
#define       EEPROM_WRITE_DISABLE         0x04
#define       EEPROM_WRITE_TO_ARRAY        0x02
#define       EEPROM_READ_FROM_ARRAY       0x03
#define       EEPROM_READ_STATUS           0x05


#define       EEPROM_STATUS_READY          (1 << 0)
#define       EEPROM_STATUS_WEL            (1 << 1)

#define       EEPROM_PRESENT               1


uint8_t EEPROM_Init();

void EEPROM_Write(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray);

void EEPROM_Read(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray);

void EEPROM_ReadStatus(uint8_t *Status);

void EEPROM_WhileNotReady();

uint8_t EEPROM_IsPresent();

uint8_t SPI_Send(uint8_t DataToSend);


#endif //__SPI_EEPROM_H



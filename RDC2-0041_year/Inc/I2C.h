/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __I2C_H
#define __I2C_H


#include "RDC2_0041_board.h"

//таймер
#define       I2C_TIMER                    TIM17
#define       I2C_TIMER_ENR                APB2ENR
#define       I2C_TIMER_CLK_EN             RCC_APB2ENR_TIM17EN


#define       IC_DETECTED                  1
#define       IC_NOT_DETECTED              0


void I2C_Init(uint8_t Address);

void I2C_Write(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray);

void I2C_Read(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray);

uint8_t I2C_IsReady();


#endif //__I2C_H



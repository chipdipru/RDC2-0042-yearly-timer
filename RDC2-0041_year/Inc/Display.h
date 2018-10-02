/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "RDC2_0041_board.h"


//таймер
#define       DISPLAY_TIMER                TIM3
#define       DISPLAY_TIMER_ENR            APB1ENR
#define       DISPLAY_TIMER_CLK_EN         RCC_APB1ENR_TIM3EN
#define       DISPLAY_TIMER_IRQ            TIM3_IRQn


//символы
#define       CHARACTER_0                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN) | \
                                            (1 << SEGMENT_4_PIN) | (1 << SEGMENT_5_PIN) | (1 << SEGMENT_6_PIN))
#define       CHARACTER_1                  ((1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN))
#define       CHARACTER_2                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_2_PIN) | (1 << SEGMENT_4_PIN) | \
                                            (1 << SEGMENT_5_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_3                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN) | \
                                            (1 << SEGMENT_4_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_4                  ((1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN) | \
                                            (1 << SEGMENT_6_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_5                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_3_PIN) | (1 << SEGMENT_4_PIN) | \
                                            (1 << SEGMENT_6_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_6                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_3_PIN) | (1 << SEGMENT_4_PIN) | \
                                            (1 << SEGMENT_5_PIN) | (1 << SEGMENT_6_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_7                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN))
#define       CHARACTER_8                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN) | (1 << SEGMENT_4_PIN) | \
                                            (1 << SEGMENT_5_PIN) | (1 << SEGMENT_6_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_9                  ((1 << SEGMENT_1_PIN) | (1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN) | \
                                            (1 << SEGMENT_4_PIN) | (1 << SEGMENT_6_PIN) | (1 << SEGMENT_7_PIN))
#define       CHARACTER_SPACE              0
#define       CHARACTER_DP                 (1 << SEGMENT_8_PIN)
#define       CHARACTER_h                  ((1 << SEGMENT_3_PIN) | (1 << SEGMENT_5_PIN) | \
                                            (1 << SEGMENT_6_PIN) | (1 << SEGMENT_7_PIN))

#define       CHARACTER_LINE               (1 << SEGMENT_7_PIN)

#define       CHARACTER_d                  ((1 << SEGMENT_2_PIN) | (1 << SEGMENT_3_PIN) | (1 << SEGMENT_4_PIN) | \
                                            (1 << SEGMENT_5_PIN) | (1 << SEGMENT_7_PIN))

#define       CHAR_h_INDEX                 10
#define       CHAR_LINE_INDEX              11
#define       CHAR_d_INDEX                 12
#define       CHAR_SPACE_INDEX             13

#define       DISPLAY_IS_OFF               0
#define       DISPLAY_IS_ON_TIME           1
#define       DISPLAY_IS_ON_EVENT          2

#define       POINT_OFF                    0
#define       POINT_ON                     1


extern const uint16_t CHARACTERS[];
extern const uint8_t DIGITS_PINS[];

typedef struct
{
  uint8_t Position; //позиция точки на индикаторе
  uint8_t Duration; //длительности одной фазы (вкл или откл)
  uint8_t Phase;    //фаза (вкл или откл)
  uint8_t Counter;  //счетчик фазы
  
} DPointType;


void DISPLAY_TIMER_ISR(void);

void Display_SetValue(uint8_t *DisplayData);

void Display_Init(uint8_t PointPos, uint8_t PointDuration);

void Display_PointSet(uint8_t PointPos, uint8_t PointDuration);

void Display_ON();

void Display_OFF();

uint8_t Display_State();

void Display_SetState(uint8_t NewState);


#endif //__DISPLAY_H



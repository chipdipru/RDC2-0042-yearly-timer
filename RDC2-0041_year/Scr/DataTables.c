/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/



#include "Display.h"



const uint16_t CHARACTERS[] = 
{
  CHARACTER_0,   CHARACTER_1,      CHARACTER_2,   CHARACTER_3,   CHARACTER_4,
  CHARACTER_5,   CHARACTER_6,      CHARACTER_7,   CHARACTER_8,   CHARACTER_9,
  CHARACTER_h,   CHARACTER_LINE,   CHARACTER_d,   CHARACTER_SPACE,
  //CHARACTER_DP,
};


const uint8_t DIGITS_PINS[] =
{
  (1 << DIGIT_4_PIN),   (1 << DIGIT_3_PIN),   
  (1 << DIGIT_2_PIN),   (1 << DIGIT_1_PIN),
};


const uint8_t MONTH_DAYS_COUNT[] =
{
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};



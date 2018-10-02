/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "Display.h"


static uint8_t ValueToShow[DIGITS_NUM];
static volatile uint8_t DisplayState = DISPLAY_IS_OFF;
static volatile DPointType Dpoint;
static volatile DPointType* DpointPtr = &Dpoint;


void DISPLAY_TIMER_ISR(void)
{
  static uint8_t CurDigit = 0;
  
  DISPLAY_TIMER->SR &= ~TIM_SR_UIF;
  
  SEGMENTS_GPIO->ODR &= ~SEGMENTS_MASK;
  DIGITS_GPIO->ODR &= ~DIGITS_MASK;
  
  uint8_t Number = ValueToShow[CurDigit];
  SEGMENTS_GPIO->ODR |= CHARACTERS[Number];
  
  if (CurDigit == DpointPtr->Position)
  {
    if (DpointPtr->Phase == POINT_ON)
      SEGMENTS_GPIO->ODR |= CHARACTER_DP; //точка
    
    DpointPtr->Counter++;
    uint8_t PointDur = DpointPtr->Duration;
    if (DpointPtr->Counter >= PointDur)
    {
      DpointPtr->Counter = 0;
      
      if (DpointPtr->Phase == POINT_OFF)
        DpointPtr->Phase = POINT_ON;
      else
        DpointPtr->Phase = POINT_OFF;
    }    
  }
  
  Number = DIGITS_PINS[CurDigit];
  DIGITS_GPIO->ODR |= Number;
  
  CurDigit++;
  if (CurDigit == DIGITS_NUM)
    CurDigit = 0;  
}
//------------------------------------------------------------------------------
void Display_SetValue(uint8_t *DisplayData)
{
  for (uint8_t i = 0; i < DIGITS_NUM; i++)
    ValueToShow[i] = (*(DisplayData + i));
}
//------------------------------------------------------------------------------
void Display_Init(uint8_t PointPos, uint8_t PointDuration)
{
  //линии индикатора
  SEGMENTS_GPIO->MODER |= ((1 << (2 * SEGMENT_1_PIN)) | (1 << (2 * SEGMENT_2_PIN)) | (1 << (2 * SEGMENT_3_PIN)) | \
                           (1 << (2 * SEGMENT_4_PIN)) | (1 << (2 * SEGMENT_5_PIN)) | (1 << (2 * SEGMENT_6_PIN)) | \
                           (1 << (2 * SEGMENT_7_PIN)) | (1 << (2 * SEGMENT_8_PIN)));
  
  DIGITS_GPIO->MODER |= ((1 << (2 * DIGIT_1_PIN)) | (1 << (2 * DIGIT_2_PIN)) | (1 << (2 * DIGIT_3_PIN)) | (1 << (2 * DIGIT_4_PIN)));
    
  //таймер, период 4 мс
  RCC->DISPLAY_TIMER_ENR |= DISPLAY_TIMER_CLK_EN;
  DISPLAY_TIMER->PSC = 479;
  DISPLAY_TIMER->ARR = 399;
  DISPLAY_TIMER->DIER = TIM_DIER_UIE;
  RCC->DISPLAY_TIMER_ENR &= ~DISPLAY_TIMER_CLK_EN;
  NVIC_EnableIRQ(DISPLAY_TIMER_IRQ);
  NVIC_SetPriority(DISPLAY_TIMER_IRQ, DISPLAY_PRIORITY);
  
  Display_PointSet(PointPos, PointDuration);
}
//------------------------------------------------------------------------------
void Display_PointSet(uint8_t PointPos, uint8_t PointDuration)
{
  DpointPtr->Position = PointPos;
  DpointPtr->Duration = PointDuration;
  DpointPtr->Phase = POINT_OFF;
  DpointPtr->Counter = 0;
}
//------------------------------------------------------------------------------
void Display_ON()
{  
  RCC->DISPLAY_TIMER_ENR |= DISPLAY_TIMER_CLK_EN;
  DISPLAY_TIMER->CR1 |= TIM_CR1_CEN;
}
//------------------------------------------------------------------------------
void Display_OFF()
{
  DisplayState = DISPLAY_IS_OFF;
  DISPLAY_TIMER->CR1 &= ~TIM_CR1_CEN;
  DISPLAY_TIMER->SR &= ~TIM_SR_UIF;
  RCC->DISPLAY_TIMER_ENR &= ~DISPLAY_TIMER_CLK_EN;
  SEGMENTS_GPIO->ODR &= ~SEGMENTS_MASK;
  DIGITS_GPIO->ODR &= ~DIGITS_MASK;
}
//------------------------------------------------------------------------------
uint8_t Display_State()
{
  return DisplayState;
}
//------------------------------------------------------------------------------
void Display_SetState(uint8_t NewState)
{
  DisplayState = NewState;
}




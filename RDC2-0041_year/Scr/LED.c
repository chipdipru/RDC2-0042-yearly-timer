/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/

//В штатном режиме LED управляется выходом.
//При возникновении ошибки - управляется ШИМ.
//Из состояния ошибки устройство выходит только после сброса питания,
//поэтому в функциях нет проверки текущей конфигурации вывода для LED.


#include "LED.h"


//static uint8_t LEDState = LED_PIN_MODE;


void LED_Init()
{
  LED_GPIO->MODER |= (1 << (2 * LED_PIN));
  
  RCC->LED_TIMER_ENR |= LED_TIMER_CLK_EN;
  LED_TIMER->PSC = LED_TIMER_PSC;
  //LED_TIMER->ARR = 100;
  //LED_TIMER->CCR1 = PWMSPtr[PWMChnl]->DutyCycle;
  LED_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
  LED_TIMER->BDTR = TIM_BDTR_MOE;
  LED_TIMER->CCER = TIM_CCER_CC1E;
  RCC->LED_TIMER_ENR &= ~LED_TIMER_CLK_EN;
}
//------------------------------------------------------------------------------
void LED_ON()
{
  LED_GPIO->ODR |= 1 << LED_PIN;
}
//------------------------------------------------------------------------------
void LED_OFF()
{
  LED_GPIO->ODR &= ~(1 << LED_PIN);
}
//------------------------------------------------------------------------------
void LED_ONwithFreq(uint8_t Frequency)
{
  if (Frequency != 0)
  {
    uint16_t ARRVal = LED_TIMER_INPUT_CLK / (LED_TIMER_PSC + 1) / Frequency;
  
    LED_GPIO->ODR &= ~(1 << LED_PIN);
    LED_GPIO->MODER &= ~(3 << (2 * LED_PIN));
    LED_GPIO->MODER |= (2 << (2 * LED_PIN));
    LED_GPIO->AFR[1] |= (LED_TIMER_AF << (4 * (LED_PIN - 8)));
  
    RCC->LED_TIMER_ENR |= LED_TIMER_CLK_EN;
    LED_TIMER->ARR = ARRVal - 1;
    LED_TIMER->CCR1 = ARRVal / 2;
    LED_TIMER->CR1 |= TIM_CR1_CEN;
  }
}

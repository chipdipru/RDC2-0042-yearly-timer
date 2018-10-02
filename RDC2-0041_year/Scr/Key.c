/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "Key.h"


void (*KeyPressedAction)(void);


void DEBOUNCE_TIMER_ISR(void)
{
  DEBOUNCE_TIMER->SR &= ~TIM_SR_UIF;

  if ((KEY_GPIO->IDR & (1 << KEY_PIN)) != (1 << KEY_PIN)) //кнопка нажата
    KeyPressedAction();
  
  DEBOUNCE_TIMER->CR1 &= ~TIM_CR1_CEN;
  DEBOUNCE_TIMER->CNT = 0;
  RCC->DEBOUNCE_TIMER_ENR &= ~DEBOUNCE_TIMER_CLK_EN;
  EXTI->IMR |= (1 << KEY_PIN);
}
//------------------------------------------------------------------------------
void KEY_ISR(void)
{
  EXTI->IMR &= ~(1 << KEY_PIN);
  EXTI->PR |= 1 << KEY_PIN;
  
  RCC->DEBOUNCE_TIMER_ENR |= DEBOUNCE_TIMER_CLK_EN;
  DEBOUNCE_TIMER->CR1 |= TIM_CR1_CEN;
}
//------------------------------------------------------------------------------
void Key_Init(void (*CallbackAction)(void))
{  
  //таймер, период  мс
  RCC->DEBOUNCE_TIMER_ENR |= DEBOUNCE_TIMER_CLK_EN;
  DEBOUNCE_TIMER->PSC = 1999;
  DEBOUNCE_TIMER->ARR = 1599;
  DEBOUNCE_TIMER->DIER = TIM_DIER_UIE;
  RCC->DEBOUNCE_TIMER_ENR &= ~DEBOUNCE_TIMER_CLK_EN;
  NVIC_EnableIRQ(DEBOUNCE_TIMER_IRQ);
  NVIC_SetPriority(DEBOUNCE_TIMER_IRQ, KEY_PRIORITY);
  
  KeyPressedAction = CallbackAction;
  
  EXTI->FTSR |= 1 << KEY_PIN;
  NVIC_EnableIRQ(KEY_IRQ);
  NVIC_SetPriority(KEY_IRQ, KEY_PRIORITY);
  EXTI->IMR |= 1 << KEY_PIN;
}



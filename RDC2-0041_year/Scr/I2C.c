/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2017, 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "I2C.h"


void I2C_Init(uint8_t Address)
{
  //I2C_GPIO->OTYPER |= (1 << I2C_SCL_PIN) | (1 << I2C_SDA_PIN);
  I2C_GPIO->AFR[0] |= (I2C_SCL_AF << (4 * I2C_SCL_PIN)) | (I2C_SDA_AF << (4 * I2C_SDA_PIN));
  I2C_GPIO->MODER |= (2 << (2 * I2C_SCL_PIN)) | (2 << (2 * I2C_SDA_PIN));
  
  RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  //I2C_PORT->TIMINGR = (uint32_t)0x10420F13; //100 кГц @ 8 MHz
  //I2C_PORT->TIMINGR = (uint32_t)0x00310309; //400 кГц @ 8 MHz
  I2C_PORT->TIMINGR = (uint32_t)0x50330309; //400 кГц @ 48 MHz
  //I2C_PORT->TIMINGR = (uint32_t)0x50100103; //1 МГц @ 48 MHz
  I2C_PORT->CR1 = I2C_CR1_PE;
  
  I2C_PORT->CR2 |= Address << 1;
  
  RCC->I2C_TIMER_ENR |= I2C_TIMER_CLK_EN;
  I2C_TIMER->ARR = 1;
  RCC->I2C_TIMER_ENR &= ~I2C_TIMER_CLK_EN;
}
//------------------------------------------------------------------------------
void I2C_Write(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray)
{
  I2C_PORT->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
  I2C_PORT->CR2 |= ((BytesNum + 1) << 16);
  
  I2C_PORT->TXDR = StartAddress;
  I2C_PORT->CR2 |= I2C_CR2_START;
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //для надежности добавить счетчик
                                         //и выход с ошибкой 
  
  while(BytesNum)
  {
    I2C_PORT->TXDR = *DataArray;
    while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //для надежности добавить счетчик
                                           //и выход с ошибкой
    *DataArray++;
    BytesNum--;
  }
  
  I2C_PORT->CR2 |= I2C_CR2_STOP;
}
//------------------------------------------------------------------------------
void I2C_Read(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray)
{
  I2C_PORT->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
  I2C_PORT->CR2 |= (1 << 16);
  
  I2C_PORT->TXDR = StartAddress;
  I2C_PORT->CR2 |= I2C_CR2_START;
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //для надежности добавить счетчик
                                         //и выход с ошибкой
    
  I2C_PORT->CR2 |= I2C_CR2_RD_WRN;
  I2C_PORT->CR2 &= ~I2C_CR2_NBYTES;
  I2C_PORT->CR2 |= (BytesNum << 16);
  I2C_PORT->CR2 |= I2C_CR2_START;
    
  do
  {
    while(!(I2C_PORT->ISR & I2C_ISR_RXNE)); //для надежности добавить счетчик
                                            //и выход с ошибкой
    *DataArray = I2C_PORT->RXDR;
    *DataArray++;
    BytesNum--;
  }
  while(BytesNum);
  
  I2C_PORT->CR2 |= I2C_CR2_STOP;
  I2C_PORT->ICR |= I2C_ICR_STOPCF;
}
//------------------------------------------------------------------------------
uint8_t I2C_IsReady()
{
  uint8_t Status = IC_DETECTED;
  
  //I2C_PORT->CR2 &= ~I2C_CR2_SADD;
  //I2C_PORT->CR2 |= IC_Address << 1;
  I2C_PORT->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
    
  I2C_PORT->CR2 |= I2C_CR2_START;

  I2C_TIMER->CR1 |= TIM_CR1_CEN;
  while((I2C_TIMER->SR & TIM_SR_UIF) != TIM_SR_UIF);
  I2C_TIMER->SR &= ~TIM_SR_UIF;

  I2C_PORT->CR2 |= I2C_CR2_START;
  while((I2C_TIMER->SR & TIM_SR_UIF) != TIM_SR_UIF);
  I2C_TIMER->SR &= ~TIM_SR_UIF;

  I2C_PORT->CR2 |= I2C_CR2_STOP;
  
  while((I2C_TIMER->SR & TIM_SR_UIF) != TIM_SR_UIF);
  I2C_TIMER->SR &= ~TIM_SR_UIF;
  I2C_TIMER->CR1 &= ~TIM_CR1_CEN;
  
  if ((I2C_PORT->ISR & I2C_ISR_NACKF) == I2C_ISR_NACKF)
  {
    Status = IC_NOT_DETECTED;
    I2C_PORT->ICR |= I2C_ICR_NACKCF;
    I2C_PORT->ISR |= I2C_ISR_TXE;
  }
  
  return Status;
}


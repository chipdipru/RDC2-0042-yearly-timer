/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/



#include "SPI_EEPROM.h"



uint8_t EEPROM_Init()
{
//  SPI_GPIO->AFR[0] &= ~((0xF << (4 * SPI_NSS_PIN)) | (0xF << (4 * SPI_SCK_PIN)) | \
//                        (0xF << (4 * SPI_MISO_PIN)) | (0xF << (4 * SPI_MOSI_PIN)));
  
  SPI_NSS_GPIO->MODER |= (1 << (2 * SPI_NSS_PIN));
  SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);
  
  SPI_GPIO->MODER |= (2 << (2 * SPI_SCK_PIN)) | \
                     (2 << (2 * SPI_MISO_PIN)) | (2 << (2 * SPI_MOSI_PIN));
  
  SPI_GPIO->OSPEEDR |= (1 << (2 * SPI_SCK_PIN)) | (1 << (2 * SPI_MISO_PIN)) | (1 << (2 * SPI_MOSI_PIN));
  
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  
  SPI_PORT->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSI | SPI_CR1_SSM;
  SPI_PORT->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
  SPI_PORT->CR1 |= SPI_CR1_SPE;
  
  return EEPROM_IsPresent();
}
//------------------------------------------------------------------------------
//операции записи и чтения выполнены в пределах размера страницы памяти
void EEPROM_Write(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray)
{
  SPI_NSS_GPIO->ODR &= ~(1 << SPI_NSS_PIN);
  SPI_Send(EEPROM_WRITE_ENABLE);
  SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);  
 
  SPI_NSS_GPIO->ODR &= ~(1 << SPI_NSS_PIN);
  
  SPI_Send(EEPROM_WRITE_TO_ARRAY);
  SPI_Send(StartAddress >> 8);
  SPI_Send(StartAddress);
  
  while(BytesNum)
  {    
    SPI_Send(*DataArray);
    *DataArray++;
    BytesNum--;
  }
  
  SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);
}
//------------------------------------------------------------------------------
void EEPROM_Read(uint8_t BytesNum, uint16_t StartAddress, uint8_t *DataArray)
{
  SPI_NSS_GPIO->ODR &= ~(1 << SPI_NSS_PIN);
    
  SPI_Send(EEPROM_READ_FROM_ARRAY);
  SPI_Send(StartAddress >> 8);
  SPI_Send(StartAddress);
  
  do
  {    
    *DataArray = SPI_Send(0x00);
    *DataArray++;
    BytesNum--;
  }
  while(BytesNum);
  
  SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);
}
//------------------------------------------------------------------------------
void EEPROM_ReadStatus(uint8_t *Status)
{
  SPI_NSS_GPIO->ODR &= ~(1 << SPI_NSS_PIN);
  
  SPI_Send(EEPROM_READ_STATUS);
  *Status = SPI_Send(0x00);
  
  SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);  
}
//------------------------------------------------------------------------------
void EEPROM_WhileNotReady()
{
  uint8_t EepromStatus = EEPROM_STATUS_READY;
  while((EepromStatus & EEPROM_STATUS_READY) == EEPROM_STATUS_READY)
    EEPROM_ReadStatus(&EepromStatus);
}
//------------------------------------------------------------------------------
uint8_t EEPROM_IsPresent()
{
  uint8_t InitStatus;
  uint8_t Status;
  
  EEPROM_ReadStatus(&InitStatus);
  
  SPI_NSS_GPIO->ODR &= ~(1 << SPI_NSS_PIN);
  SPI_Send(EEPROM_WRITE_ENABLE);
  SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);  
  
  EEPROM_ReadStatus(&Status);
  
  if (Status != InitStatus)
  {
    Status = EEPROM_PRESENT;
    
    SPI_NSS_GPIO->ODR &= ~(1 << SPI_NSS_PIN);
    SPI_Send(EEPROM_WRITE_DISABLE);
    SPI_NSS_GPIO->ODR |= (1 << SPI_NSS_PIN);  
  }
  
  else
    Status = 0;
  
  return Status;
}
//------------------------------------------------------------------------------
uint8_t SPI_Send(uint8_t DataToSend)
{
  while(!(SPI_PORT->SR & SPI_SR_TXE));
  SPI1_DR_8BIT = (uint8_t)DataToSend;
  while(!(SPI_PORT->SR & SPI_SR_RXNE));
  return SPI1_DR_8BIT;
}




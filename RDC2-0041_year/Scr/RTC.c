/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/



#include "RTC.h"
#include "I2C.h"


static volatile uint8_t IntStatus = 0;


uint8_t RTC_Init()
{
  I2C_GPIO->OTYPER |= (1 << I2C_SCL_PIN) | (1 << I2C_SDA_PIN);
  I2C_GPIO->MODER |= (1 << (2 * I2C_SCL_PIN)) | (1 << (2 * I2C_SDA_PIN));
  
  uint8_t AttemptCnt = 0;
  
  do
  {
    I2C_GPIO->ODR |= (1 << I2C_SCL_PIN) | (1 << I2C_SDA_PIN);
    
    if((I2C_GPIO->IDR & (1 << I2C_SDA_PIN)) == (1 << I2C_SDA_PIN))
    {
      I2C_GPIO->ODR &= ~(1 << I2C_SDA_PIN);
      I2C_GPIO->ODR |= (1 << I2C_SDA_PIN);
    }
    
    I2C_GPIO->ODR &= ~(1 << I2C_SCL_PIN);
    AttemptCnt++;
  }
  
  while((((I2C_GPIO->IDR & (1 << I2C_SDA_PIN)) != (1 << I2C_SDA_PIN))) && (AttemptCnt < I2C_RST_ATTEMPTS));
  
  if((I2C_GPIO->IDR & (1 << I2C_SDA_PIN)) != (1 << I2C_SDA_PIN))
    return RTC_IS_NOT_READY;
  
  I2C_GPIO->MODER &= ~((3 << (2 * I2C_SCL_PIN)) | (3 << (2 * I2C_SDA_PIN)));
  
  I2C_Init(DS3231_I2C_ADDRESS);
    
  uint8_t InitData[] = {DS3231_INTCN /*| DS3231_A2IE | DS3231_A1IE*/, 0,};
  I2C_Write(2, DS3231_CONTROL_REG, InitData);
  
  EXTI->FTSR |= 1 << RTC_PIN;
  NVIC_EnableIRQ(RTC_IRQ);
  NVIC_SetPriority(RTC_IRQ, RTC_PRIORITY);
  EXTI->IMR |= 1 << RTC_PIN;  
  
  return RTC_IS_READY;
}
//------------------------------------------------------------------------------
void RTC_SetTime(uint8_t *TimeData)
{
  I2C_Write(BYTES_FOR_TIME, DS3231_TIME_BASE_REG, TimeData);
}
//------------------------------------------------------------------------------
void RTC_ReadTime(uint8_t *TimeData)
{
  I2C_Read(BYTES_FOR_TIME, DS3231_TIME_BASE_REG, TimeData);
  
  (*(TimeData + DS3231_HOURS_OFFSET)) &= DS3231_HOUR_MASK;
  (*(TimeData + DISPLAY_HOURS)) = (*(TimeData + DS3231_HOURS_OFFSET)) >> DS3231_TENS_OFFSET;
  (*(TimeData + DISPLAY_HOURS + 1)) = ((*(TimeData + DS3231_HOURS_OFFSET)) & DS3231_UNITS_MASK);
  (*(TimeData + DISPLAY_MINUTES)) = (*(TimeData + DS3231_MINUTES_OFFSET)) >> DS3231_TENS_OFFSET;
  (*(TimeData + DISPLAY_MINUTES + 1)) = (*(TimeData + DS3231_MINUTES_OFFSET)) & DS3231_UNITS_MASK;
  
  (*(TimeData + DS3231_SECONDS_OFFSET)) = BinToDec((*(TimeData + DS3231_SECONDS_OFFSET)));
  (*(TimeData + DS3231_MINUTES_OFFSET)) = BinToDec((*(TimeData + DS3231_MINUTES_OFFSET)));
  (*(TimeData + DS3231_HOURS_OFFSET)) = BinToDec((*(TimeData + DS3231_HOURS_OFFSET)));
  (*(TimeData + DS3231_DATE_OFFSET)) = BinToDec((*(TimeData + DS3231_DATE_OFFSET)));
  (*(TimeData + DS3231_MONTH_OFFSET)) = BinToDec((*(TimeData + DS3231_MONTH_OFFSET)) & DS3231_MONTH_MASK);
  (*(TimeData + DS3231_YEAR_OFFSET)) = BinToDec((*(TimeData + DS3231_YEAR_OFFSET)));
}
//------------------------------------------------------------------------------
void RTC_ISR(void)
{
  EXTI->PR |= 1 << RTC_PIN;
  
  IntStatus = RTC_INT_ACTIVE;
}
//------------------------------------------------------------------------------
uint8_t RTC_IsIntActive()
{  
  return IntStatus;
}
//------------------------------------------------------------------------------
void RTC_ClearIntStatus()
{  
  IntStatus = 0;
}
//------------------------------------------------------------------------------
void RTC_ReadStatus(uint8_t *Status)
{
  I2C_Read(1, DS3231_STATUS_REG, Status);  
  uint8_t NewStatus = (*Status) & (~(DS3231_A1F | DS3231_A2F));
  I2C_Write(1, DS3231_STATUS_REG, &NewStatus);
}
//------------------------------------------------------------------------------
void RTC_SetAlarm1(uint8_t *AlarmData)
{
  RTC_SetAlarm(DS3231_ALARM1_BASE_REG, ALARM_1_SIZE, AlarmData);  
}
//------------------------------------------------------------------------------
void RTC_SetAlarm2(uint8_t *AlarmData)
{
  RTC_SetAlarm(DS3231_ALARM2_BASE_REG, ALARM_2_SIZE, AlarmData);
}
//------------------------------------------------------------------------------
void RTC_SetAlarm(uint8_t AlarmReg, uint8_t AlarmSize, uint8_t *AlarmData)
{
  for (uint8_t i = 0; i < AlarmSize; i++)
  {
    uint8_t AlarmVal = (*(AlarmData + i));
    
    if ((AlarmVal & DS3231_ALARM_MASK) != DS3231_ALARM_MASK)
    {
      AlarmVal = DecToBin(AlarmVal);
      (*(AlarmData + i)) = AlarmVal;
    }
  }
  
  I2C_Write(AlarmSize, AlarmReg, AlarmData);
}
//------------------------------------------------------------------------------
void RTC_EnableAlarm1()
{
  RTC_SetCtrlBit(DS3231_A1IE);
}
//------------------------------------------------------------------------------
void RTC_EnableAlarm2()
{
  RTC_SetCtrlBit(DS3231_A2IE);
}
//------------------------------------------------------------------------------
void RTC_DisableAlarm1()
{
  RTC_ClearCtrlBit(DS3231_A1IE);
}
//------------------------------------------------------------------------------
void RTC_SetCtrlBit(uint8_t BitToSet)
{
  uint8_t CtrlReg;
  I2C_Read(1, DS3231_CONTROL_REG, &CtrlReg);
  CtrlReg |= BitToSet;
  I2C_Write(1, DS3231_CONTROL_REG, &CtrlReg);
}
//------------------------------------------------------------------------------
void RTC_ClearCtrlBit(uint8_t BitToClear)
{
  uint8_t CtrlReg;
  I2C_Read(1, DS3231_CONTROL_REG, &CtrlReg);
  CtrlReg &= ~BitToClear;
  I2C_Write(1, DS3231_CONTROL_REG, &CtrlReg);
}
//------------------------------------------------------------------------------
uint8_t BinToDec(uint8_t BinVal)
{
  uint8_t Tens = (BinVal >> DS3231_TENS_OFFSET) * 10;
  uint8_t Units = BinVal & DS3231_UNITS_MASK;
  
  return (Tens + Units);
  //return ((BinVal >> DS3231_TENS_OFFSET) * 10 + BinVal & DS3231_UNITS_MASK);
}
//------------------------------------------------------------------------------
uint8_t DecToBin (uint8_t DecVal)
{
  uint8_t Tens = (DecVal / 10);
  uint8_t NewValue = (Tens << 4);
  NewValue |= DecVal % 10;
            
  return NewValue;
}
       


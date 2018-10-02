/*
********************************************************************************
* COPYRIGHT(c) ��� ���� � ��ϻ, 2018
* 
* ����������� ����������� ��������������� �� �������� ���� ����� (as is).
* ��� ��������������� �������� ������ �����������.
********************************************************************************
*/



#ifndef __KEY_H
#define __KEY_H


#include "RDC2_0041_board.h"


//������
#define       DEBOUNCE_TIMER               TIM14
#define       DEBOUNCE_TIMER_ENR           APB1ENR
#define       DEBOUNCE_TIMER_CLK_EN        RCC_APB1ENR_TIM14EN
#define       DEBOUNCE_TIMER_IRQ           TIM14_IRQn



void DEBOUNCE_TIMER_ISR(void);

void KEY_ISR(void);

void Key_Init(void (*CallbackAction)(void));


#endif //__KEY_H
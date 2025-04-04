#ifndef __TS_H
#define	__TS_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	����ʪ�ȴ�����h�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.8.12
 * MCU			:	STM32F103ZET6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/

#define TS_READ_TIMES	10  //����ʪ��ADCѭ����ȡ����


/***************�����Լ��������****************/
// TS GPIO�궨��

#define		TS_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	TS_GPIO_PORT							GPIOA
#define 	TS_GPIO_PIN								GPIO_Pin_5
#define   ADC_CHANNEL             	ADC_Channel_5	// ADC ͨ���궨��

/*********************END**********************/


void TS_Init(void);
void TS_GetData(uint8_t*);

#endif /* __ADC_H */


#ifndef __TS_H
#define	__TS_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	土壤湿度传感器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.12
 * MCU			:	STM32F103ZET6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/

#define TS_READ_TIMES	10  //土壤湿度ADC循环读取次数


/***************根据自己需求更改****************/
// TS GPIO宏定义

#define		TS_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	TS_GPIO_PORT							GPIOA
#define 	TS_GPIO_PIN								GPIO_Pin_5
#define   ADC_CHANNEL             	ADC_Channel_5	// ADC 通道宏定义

/*********************END**********************/


void TS_Init(void);
void TS_GetData(uint8_t*);

#endif /* __ADC_H */


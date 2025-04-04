#ifndef __BUMP_H
#define	__BUMP_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	5V水泵模块h文件                   
 * 版本			: V1.0
 * 日期			: 2024.9.22
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/***************根据自己需求更改****************/
// 水泵模块 GPIO宏定义

#define	BUMP_CLK							RCC_APB2Periph_GPIOA

#define BUMP_GPIO_PIN 				GPIO_Pin_7

#define BUMP_GPIO_PROT 				GPIOA

#define BUMP_ON 		GPIO_SetBits(BUMP_GPIO_PROT,BUMP_GPIO_PIN)
#define BUMP_OFF 		GPIO_ResetBits(BUMP_GPIO_PROT,BUMP_GPIO_PIN)

#define FUN_ON			GPIO_SetBits(GPIOA , GPIO_Pin_6); //开启
#define FUN_OFF			GPIO_ResetBits(GPIOA , GPIO_Pin_6); //关闭

/*********************END**********************/

void BUMP_Init(void);
void FUN_Init(void);

#endif




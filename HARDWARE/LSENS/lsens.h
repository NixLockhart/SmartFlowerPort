/**
 ****************************************************************************************************
 * @file        lsens.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       ���������� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 ****************************************************************************************************
 */

#ifndef __LSENS_H
#define __LSENS_H

#include "sys.h"


/******************************************************************************************/
/* ������������ӦADC3���������ź�ͨ�� ���� */

#define LSENS_ADC3_CHX_GPIO_PORT            GPIOF
#define LSENS_ADC3_CHX_GPIO_PIN             SYS_GPIO_PIN8
#define LSENS_ADC3_CHX_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 7; }while(0)   /* PF��ʱ��ʹ�� */


#define LSENS_ADC3_CHX                      ADC_Channel_6       /* ͨ��Y,  0 <= Y <= 17 */ 
#define LSENS_READ_TIMES										10

/******************************************************************************************/
 

void Lsens_Init(void);          /* ��ʼ������������ */
void Lsens_Get_Val(uint8_t*);    /* ��ȡ������������ֵ */
#endif 























/**
 ****************************************************************************************************
 * @file        lsens.c
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

#include "adc.h"
#include "lsens.h"
#include "delay.h"


/**
 * @brief       ��ʼ������������
 * @param       ��
 * @retval      ��
 */
void Lsens_Init(void)
{
    LSENS_ADC3_CHX_GPIO_CLK_ENABLE();   /* IO��ʱ��ʹ�� */

    sys_gpio_set(LSENS_ADC3_CHX_GPIO_PORT, LSENS_ADC3_CHX_GPIO_PIN,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */

    Adc3_Init();   /* ��ʼ��ADC */
}

/**
 * @brief       ��ȡ����������ֵ
 * @param       ��
 * @retval      0~100:0,�;100,����
 */
void Lsens_Get_Val(uint8_t* li)
{
    uint32_t temp_val = 0;
		uint8_t t;
		for(t=0;t<LSENS_READ_TIMES;t++)
		{
				temp_val+=Get_Adc3(LSENS_ADC3_CHX);	//��ȡADCֵ
				delay_ms(5);
		}
		temp_val/=LSENS_READ_TIMES;
    temp_val /= 40;
    if (temp_val > 100)temp_val = 100;
    *li = (uint8_t)(100 - temp_val);
}













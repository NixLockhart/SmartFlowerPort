#include "TS.h"


/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	����ʪ�ȴ�����c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.8.12
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/

extern uint8_t i;
void TS_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (TS_GPIO_CLK, ENABLE);	// �� ADC IO�˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = TS_GPIO_PIN;					// ���� ADC IO ����ģʽ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// ����Ϊģ������
		
		GPIO_Init(TS_GPIO_PORT, &GPIO_InitStructure);				// ��ʼ�� ADC IO

		ADCx_Init();
}

uint16_t TS_ADC_Read(void)
{
	//����ָ��ADC�Ĺ�����ͨ��������ʱ��
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}

void TS_GetData(uint8_t* st)
{
	uint32_t  tempData = 0;
	for (int i = 0; i < TS_READ_TIMES; i++)
	{
		tempData += TS_ADC_Read();
		delay_ms(5);
	}

	tempData /= TS_READ_TIMES;
	*st = 100 - (float)tempData/40.96;
}




/**
 ****************************************************************************************************
 * @file        dht11.c
 * @author      NixStudio(NixLockhart)
 * @version     V1.0
 * @date        2025-01-02
 * @brief       DHT11��ز���
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 *
 ****************************************************************************************************
 */


#include "dht11.h"
#include "delay.h"

//���͸�λ�źŸ� DHT11����������ͨ��
void DHT11_Rst(void)       
{                 
    DHT11_IO_OUT();     //SET OUTPUT
    DHT11_DQ_OUT(0);     //??DQ
    delay_ms(20);        //????18ms
    DHT11_DQ_OUT(1);     //DQ=1 
    delay_us(30);         //????20~40us
}

//��� DHT11 ����Ӧ�źţ��ж����Ƿ�׼����ͨ��
uint8_t DHT11_Check(void)        
{   
    uint8_t retry=0;
    DHT11_IO_IN();//SET INPUT     
    while (DHT11_DQ_IN&&retry<100)//DHT11???40~80us
    {
        retry++;
        delay_us(1);
    };     
    if(retry>=100)return 1;
    else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11????????40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;        
    return 0;
}

//��ȡ DHT11 ����� 1 λ����
uint8_t DHT11_Read_Bit(void)              
{
     uint8_t retry=0;
    while(DHT11_DQ_IN&&retry<100)//???????
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_DQ_IN&&retry<100)//??????
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//??40us
    if(DHT11_DQ_IN)return 1;
    else return 0;           
}

//��ȡ DHT11 ����� 1 �ֽڣ�8 λ������
uint8_t DHT11_Read_Byte(void)    
{        
    uint8_t i,dat;
    dat=0;
    for (i=0;i<8;i++) 
    {
        dat<<=1; 
        dat|=DHT11_Read_Bit();
    }                            
    return dat;
}

// DHT11 ����ʪ�����ݣ����洢��ָ��������
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
     uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if(DHT11_Check()==0)
    {
        for(i=0;i<5;i++)//??40???
        {
            buf[i]=DHT11_Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            *humi=buf[0];
            *temp=buf[2];
        }
    }
    else return 1;
    return 0;        
}

//��ʼ�� DHT11 ���������ź�ͨ��׼��
uint8_t DHT11_Init(void)
{     
     GPIO_InitTypeDef  GPIO_InitStructure;    
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);         //??PG????
     GPIO_InitStructure.GPIO_Pin = DHT_Pin;                             //PG11????
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;              //????
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOG, &GPIO_InitStructure);                         //???IO?
     GPIO_SetBits(GPIOG,DHT_Pin);                                         //PG11 ???
    DHT11_Rst();                                                  //??DHT11
    return DHT11_Check();                                        //??DHT11???
}

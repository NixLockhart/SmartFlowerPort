#ifndef DHT11_H
#define DHT11_H

#include "stm32f10x.h"

#define DHT_Pin GPIO_Pin_11

#define DHT11_IO_IN()  {GPIOG->CRH &= 0XFFFF0FFF; GPIOG->CRH |= 0X00008000;}
#define DHT11_IO_OUT() {GPIOG->CRH &= 0XFFFF0FFF; GPIOG->CRH |= 0X00003000;}

#define    DHT11_DQ_OUT(X)  GPIO_WriteBit(GPIOG, GPIO_Pin_11, X)
#define    DHT11_DQ_IN  GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11)

uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Check(void);
void DHT11_Rst(void);   

#endif

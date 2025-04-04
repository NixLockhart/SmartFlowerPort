/**
 ****************************************************************************************************
 * @file        demo.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ��ԭ��������ʵ��
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
 ****************************************************************************************************
 */

#include "stdint.h"

#ifndef __DEMO_H
#define __DEMO_H

void demo_run(void);
uint8_t wifi_connect();
void atkcld_con(uint8_t *is_atkcld);
void send_data_to_cloud(uint8_t is_atkcld,uint8_t temp,uint8_t humi,uint8_t soil_humi,uint8_t light_ind);
uint8_t execute();

#endif

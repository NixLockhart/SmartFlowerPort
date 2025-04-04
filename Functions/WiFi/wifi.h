/**
 ****************************************************************************************************
 * @file        demo.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266D模块原子云连接实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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

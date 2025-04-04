/**
 ****************************************************************************************************
 * @file        lsens.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       光敏传感器 驱动代码
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
 * 修改说明
 * V1.0 20200424
 * 第一次发布
 ****************************************************************************************************
 */

#include "adc.h"
#include "lsens.h"
#include "delay.h"


/**
 * @brief       初始化光敏传感器
 * @param       无
 * @retval      无
 */
void Lsens_Init(void)
{
    LSENS_ADC3_CHX_GPIO_CLK_ENABLE();   /* IO口时钟使能 */

    sys_gpio_set(LSENS_ADC3_CHX_GPIO_PORT, LSENS_ADC3_CHX_GPIO_PIN,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD采集引脚模式设置,模拟输入 */

    Adc3_Init();   /* 初始化ADC */
}

/**
 * @brief       读取光敏传感器值
 * @param       无
 * @retval      0~100:0,最暗;100,最亮
 */
void Lsens_Get_Val(uint8_t* li)
{
    uint32_t temp_val = 0;
		uint8_t t;
		for(t=0;t<LSENS_READ_TIMES;t++)
		{
				temp_val+=Get_Adc3(LSENS_ADC3_CHX);	//读取ADC值
				delay_ms(5);
		}
		temp_val/=LSENS_READ_TIMES;
    temp_val /= 40;
    if (temp_val > 100)temp_val = 100;
    *li = (uint8_t)(100 - temp_val);
}













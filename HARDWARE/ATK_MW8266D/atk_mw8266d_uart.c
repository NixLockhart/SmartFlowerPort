/**
 ****************************************************************************************************
 * @file        atk_mw8266d_uart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266D模块UART接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 */

#include "atk_mw8266d_uart.h"
#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static struct
{
    uint8_t buf[ATK_MW8266D_UART_RX_BUF_SIZE];              /* 帧接收缓冲 */
    struct
    {
        uint16_t len    : 15;                               /* 帧接收长度，sta[14:0] */
        uint16_t finsh  : 1;                                /* 帧接收完成标志，sta[15] */
    } sta;                                                  /* 帧状态信息 */
} g_uart_rx_frame = {0};                                    /* ATK-MW8266D UART接收帧缓冲信息结构体 */

static uint8_t g_uart_tx_buf[ATK_MW8266D_UART_TX_BUF_SIZE]; /* ATK-MW8266D UART发送缓冲 */

/**
 * @brief       ATK-MW8266D UART printf
 * @param       fmt: 待打印的数据
 * @retval      无
 */
void atk_mw8266d_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);

    len = strlen((const char *)g_uart_tx_buf);

    for (uint16_t i = 0; i < len; i++)
    {
        while (USART_GetFlagStatus(ATK_MW8266D_UART_INTERFACE, USART_FLAG_TXE) == RESET)
            ; /* 等待发送缓冲区空 */
        USART_SendData(ATK_MW8266D_UART_INTERFACE, g_uart_tx_buf[i]);
    }
}

/**
 * @brief       ATK-MW8266D UART重新开始接收数据
 * @param       无
 * @retval      无
 */
void atk_mw8266d_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len = 0;
    g_uart_rx_frame.sta.finsh = 0;
}

/**
 * @brief       获取ATK-MW8266D UART接收到的一帧数据
 * @param       无
 * @retval      NULL: 未接收到一帧数据
 *              其他: 接收到的一帧数据
 */
uint8_t *atk_mw8266d_uart_rx_get_frame(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       获取ATK-MW8266D UART接收到的一帧数据的长度
 * @param       无
 * @retval      0   : 未接收到一帧数据
 *              其他: 接收到的一帧数据的长度
 */
uint16_t atk_mw8266d_uart_rx_get_frame_len(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        return g_uart_rx_frame.sta.len;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       ATK-MW8266D UART初始化
 * @param       baudrate: UART通讯波特率
 * @retval      无
 */
void atk_mw8266d_uart_init(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能USART和GPIO的时钟 */
    RCC_APB2PeriphClockCmd(ATK_MW8266D_UART_TX_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(ATK_MW8266D_UART_RX_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(ATK_MW8266D_UART_CLK, ENABLE);

    /* 初始化UART的TX引脚 */
    GPIO_InitStructure.GPIO_Pin = ATK_MW8266D_UART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(ATK_MW8266D_UART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* 初始化UART的RX引脚 */
    GPIO_InitStructure.GPIO_Pin = ATK_MW8266D_UART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(ATK_MW8266D_UART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* 配置USART参数 */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(ATK_MW8266D_UART_INTERFACE, &USART_InitStructure);

    /* 使能USART */
    USART_Cmd(ATK_MW8266D_UART_INTERFACE, ENABLE);

    /* 使能USART接收中断 */
    USART_ITConfig(ATK_MW8266D_UART_INTERFACE, USART_IT_RXNE, ENABLE);
    USART_ITConfig(ATK_MW8266D_UART_INTERFACE, USART_IT_IDLE, ENABLE);

    /* 配置中断优先级 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = ATK_MW8266D_UART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief       ATK-MW8266D UART中断回调函数
 * @param       无
 * @retval      无
 */
void ATK_MW8266D_UART_IRQHandler(void)
{
    uint8_t tmp;

    /* 接收数据中断 */
    if (USART_GetITStatus(ATK_MW8266D_UART_INTERFACE, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(ATK_MW8266D_UART_INTERFACE);

        if (g_uart_rx_frame.sta.len < (ATK_MW8266D_UART_RX_BUF_SIZE - 1))
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len++] = tmp;
        }
        else
        {
            g_uart_rx_frame.sta.len = 0; /* 溢出时覆盖缓冲 */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len++] = tmp;
        }
    }

    /* 空闲中断（帧结束） */
    if (USART_GetITStatus(ATK_MW8266D_UART_INTERFACE, USART_IT_IDLE) != RESET)
    {
        (void)USART_ReceiveData(ATK_MW8266D_UART_INTERFACE); /* 清除中断标志 */
        g_uart_rx_frame.sta.finsh = 1;
    }
}

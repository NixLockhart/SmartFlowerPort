/**
 ****************************************************************************************************
 * @file        atk_mw8266d_uart.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ��UART�ӿ���������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 */

#include "atk_mw8266d_uart.h"
#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static struct
{
    uint8_t buf[ATK_MW8266D_UART_RX_BUF_SIZE];              /* ֡���ջ��� */
    struct
    {
        uint16_t len    : 15;                               /* ֡���ճ��ȣ�sta[14:0] */
        uint16_t finsh  : 1;                                /* ֡������ɱ�־��sta[15] */
    } sta;                                                  /* ֡״̬��Ϣ */
} g_uart_rx_frame = {0};                                    /* ATK-MW8266D UART����֡������Ϣ�ṹ�� */

static uint8_t g_uart_tx_buf[ATK_MW8266D_UART_TX_BUF_SIZE]; /* ATK-MW8266D UART���ͻ��� */

/**
 * @brief       ATK-MW8266D UART printf
 * @param       fmt: ����ӡ������
 * @retval      ��
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
            ; /* �ȴ����ͻ������� */
        USART_SendData(ATK_MW8266D_UART_INTERFACE, g_uart_tx_buf[i]);
    }
}

/**
 * @brief       ATK-MW8266D UART���¿�ʼ��������
 * @param       ��
 * @retval      ��
 */
void atk_mw8266d_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len = 0;
    g_uart_rx_frame.sta.finsh = 0;
}

/**
 * @brief       ��ȡATK-MW8266D UART���յ���һ֡����
 * @param       ��
 * @retval      NULL: δ���յ�һ֡����
 *              ����: ���յ���һ֡����
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
 * @brief       ��ȡATK-MW8266D UART���յ���һ֡���ݵĳ���
 * @param       ��
 * @retval      0   : δ���յ�һ֡����
 *              ����: ���յ���һ֡���ݵĳ���
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
 * @brief       ATK-MW8266D UART��ʼ��
 * @param       baudrate: UARTͨѶ������
 * @retval      ��
 */
void atk_mw8266d_uart_init(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��USART��GPIO��ʱ�� */
    RCC_APB2PeriphClockCmd(ATK_MW8266D_UART_TX_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(ATK_MW8266D_UART_RX_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(ATK_MW8266D_UART_CLK, ENABLE);

    /* ��ʼ��UART��TX���� */
    GPIO_InitStructure.GPIO_Pin = ATK_MW8266D_UART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(ATK_MW8266D_UART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* ��ʼ��UART��RX���� */
    GPIO_InitStructure.GPIO_Pin = ATK_MW8266D_UART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(ATK_MW8266D_UART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* ����USART���� */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(ATK_MW8266D_UART_INTERFACE, &USART_InitStructure);

    /* ʹ��USART */
    USART_Cmd(ATK_MW8266D_UART_INTERFACE, ENABLE);

    /* ʹ��USART�����ж� */
    USART_ITConfig(ATK_MW8266D_UART_INTERFACE, USART_IT_RXNE, ENABLE);
    USART_ITConfig(ATK_MW8266D_UART_INTERFACE, USART_IT_IDLE, ENABLE);

    /* �����ж����ȼ� */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = ATK_MW8266D_UART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief       ATK-MW8266D UART�жϻص�����
 * @param       ��
 * @retval      ��
 */
void ATK_MW8266D_UART_IRQHandler(void)
{
    uint8_t tmp;

    /* ���������ж� */
    if (USART_GetITStatus(ATK_MW8266D_UART_INTERFACE, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(ATK_MW8266D_UART_INTERFACE);

        if (g_uart_rx_frame.sta.len < (ATK_MW8266D_UART_RX_BUF_SIZE - 1))
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len++] = tmp;
        }
        else
        {
            g_uart_rx_frame.sta.len = 0; /* ���ʱ���ǻ��� */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len++] = tmp;
        }
    }

    /* �����жϣ�֡������ */
    if (USART_GetITStatus(ATK_MW8266D_UART_INTERFACE, USART_IT_IDLE) != RESET)
    {
        (void)USART_ReceiveData(ATK_MW8266D_UART_INTERFACE); /* ����жϱ�־ */
        g_uart_rx_frame.sta.finsh = 1;
    }
}

/**
 ****************************************************************************************************
 * @file        wifi.c
 * @author      NixStudio(NixLockhart)
 * @version     V1.0
 * @date        2025-01-02
 * @brief       ATK-MW8266Dģ����ԭ����ͨ��
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 *
 ****************************************************************************************************
 */

#include <string.h>
#include "wifi.h"
#include "atk_mw8266d.h"
#include "atk_mw8266d_uart.h" x
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "lcd.h"

#define DEMO_WIFI_SSID          "Nix_Wifi"
#define DEMO_WIFI_PWD           "12345678"
#define DEMO_ATKCLD_DEV_ID      "31366612859960558514"
#define DEMO_ATKCLD_DEV_PWD     "12345678"


// JSON��ʽ��������
void send_data_to_cloud(uint8_t atkcld,uint8_t temp,uint8_t humi,uint8_t soil_humi,uint8_t light_ind){
    if (atkcld == 1){ // ȷ�������ӵ�ԭ����
        char json_buffer[128]; // JSON�ַ���������
        // ����JSON��ʽ������
        snprintf(json_buffer, sizeof(json_buffer), "{\"temperature\": %d, \"humidity\": %d, \"Soil Humi\": %d, \"Light\": %d}", temp, humi, soil_humi, light_ind);
        // �������ݵ�ԭ����
        atk_mw8266d_uart_printf("%s\r\n", json_buffer);
    }
    else{
        printf("Not connected to ALIENTEK cloud!\r\n");
    }
}

// ����ԭ����
void atkcld_con(uint8_t *is_atkcld)
{
    uint8_t ret;
    
    if (*is_atkcld == 0)
    {
        /* ����ԭ���� */
        ret = atk_mw8266d_connect_atkcld(DEMO_ATKCLD_DEV_ID, DEMO_ATKCLD_DEV_PWD);
        if (ret == 0)
        {
            *is_atkcld = 1;
            printf("Connect to ALIENTEK cloud!\r\n");
        }
        else
        {
            printf("Error to connect ALIENTEK cloud!\r\n");
        }
    }
    else
    {
        /* �Ͽ�ԭ�������� */
        atk_mw8266d_disconnect_atkcld();
        *is_atkcld = 0;
        printf("Disconnect to ALIENTEK cloud!\r\n");
    }
}

//������Ϣ�����ض�Ӧ������555555++rrr
uint8_t execute()
{
    uint8_t *buf;
        /* ��������ATK-MW8266D UART��һ֡���� */
		buf = atk_mw8266d_uart_rx_get_frame();
		if (buf != NULL){
			printf("%s", buf);
			
			// ����ַ�������
			if (strstr((char *)buf, "light on") != NULL){//����1
					atk_mw8266d_uart_rx_restart();
					return 1;
			}
			else if (strstr((char *)buf, "light off") != NULL){//�ص�2
					atk_mw8266d_uart_rx_restart();
					return 2;
			}
			else if (strstr((char *)buf, "water on") != NULL){//��ˮ3
					atk_mw8266d_uart_rx_restart();
					return 3;
			}
			else if (strstr((char *)buf, "water off") != NULL){//��ˮ4
					atk_mw8266d_uart_rx_restart();
					return 4;
			}
			else if (strstr((char *)buf, "fun on") != NULL){//�Զ�
					atk_mw8266d_uart_rx_restart();
					return 5;
			}
			else if (strstr((char *)buf, "fun off") != NULL){//�Զ�
					atk_mw8266d_uart_rx_restart();
					return 6;
			}
			else if (strstr((char *)buf, "auto") != NULL){//�ֶ�
					atk_mw8266d_uart_rx_restart();
					return 7;
			}
			else if (strstr((char *)buf, "manual") != NULL){//�Զ�
					atk_mw8266d_uart_rx_restart();
					return 8;
			}
			
			// ���¿�ʼ������һ֡
			atk_mw8266d_uart_rx_restart();
    }
    return 0; // ���û��ƥ����ַ���������0��ʾ�޲���
}

//����Wifi
uint8_t wifi_connect(){
	uint8_t ret=0;
	char ip_buf[16];
	printf("WiFi Connecting\r\n");
	ret  = atk_mw8266d_restore();                               /* �ָ��������� */
	ret += atk_mw8266d_at_test();                               /* AT���� */
	ret += atk_mw8266d_set_mode(1);                             /* Stationģʽ */
	ret += atk_mw8266d_sw_reset();                              /* �����λ */
	ret += atk_mw8266d_ate_config(0);                           /* �رջ��Թ��� */
	ret += atk_mw8266d_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);  /* ����WIFI */
	ret += atk_mw8266d_get_ip(ip_buf);                          /* ��ȡIP��ַ */
	if (ret != 0)
	{
			printf("Error to connect WiFi!\r\n");
			return ret;
	}
	printf("Connected!\r\n");
	atk_mw8266d_uart_rx_restart();
	return ret;
}

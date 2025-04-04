/**
 ****************************************************************************************************
 * @file        wifi.c
 * @author      NixStudio(Nix Lockhart)
 * @version     V1.1
 * @date        2025-01-03
 * @brief       Smart Flower Pot ���ܻ���v1.1
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 *
 ****************************************************************************************************
 */

#include "adc.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "timer.h"
#include "usart3.h"
#include "lsens.h"
#include "lcd.h"
#include "dht11.h"
#include "tpad.h"
#include "ts.h"
#include "wifi.h"
#include "atk_mw8266d.h"
#include "atk_mw8266d_uart.h"
#include "bump.h"
#include "lvgl/lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"

typedef enum {
    SCREEN_MAIN,
    SCREEN_MENU,
    SCREEN_MANUAL,
		SCREEN_LIMIT
} screen_t;

typedef struct{
	uint8_t temp_upper;
	uint8_t temp_lower;
	uint8_t humi_upper;
	uint8_t humi_lower;
	uint8_t shumi_upper;
	uint8_t shumi_lower;
	uint8_t light_upper;
	uint8_t light_lower;
}limits;

limits lim_value;

static screen_t current_screen = SCREEN_MAIN;

/****ȫ�ֱ���****/


//�������

static lv_obj_t *scr_main;  					//������
static lv_obj_t *scr_menu;  					//�˵�����
static lv_obj_t *scr_manual; 					//�ֶ���������

static lv_obj_t *scr_limit; 					//�¶������޽���

static lv_obj_t *label_temp;  					//�¶ȱ�ǩ
static lv_obj_t *label_humi;  					//ʪ�ȱ�ǩ
static lv_obj_t *label_soil_humi;  			//����ʪ�ȱ�ǩ
static lv_obj_t *label_light;  					//����ǿ�ȱ�ǩ
static lv_obj_t *label_mode;  					//ģʽ��ǩ
static lv_obj_t *label_menu;  					//�˵���ǩ
static lv_obj_t *label_light_status; 		//��ˮ״̬��ǩ
static lv_obj_t *label_water_status; 		//����״̬��ǩ

static lv_obj_t *label_lower_value; 		//���ޱ�ǩ
static lv_obj_t *label_upper_value; 		//���ޱ�ǩ

static uint8_t menu_index = 0; 					//�˵�ѡ������

// ���嵯������
static lv_obj_t *popup_container;
static lv_obj_t *popup_label;

static uint8_t light_status=0;			//��ˮ״̬
static uint8_t water_status=0;			//����״̬

static uint8_t menu_item=4;					//�˵�ѡ������

static uint8_t temp = 25;						//�¶�ֵ
static uint8_t humi = 60;						//ʪ��ֵ
static uint8_t soil_humi = 40;			//����ʪ��ֵ
static uint8_t light_intensity = 80;//������ǿ��

static uint8_t mode = 0;						//����ģʽ{�Զ����ֶ�}
static uint8_t limit_page = 1;

static uint8_t temp_max = 50;				//�����¶���ֵ�����
static uint8_t temp_min = 0;				//�����¶���ֵ����С��
static uint8_t humi_max = 70;				//����ʪ����ֵ�����
static uint8_t humi_min = 40;				//����ʪ����ֵ����С��
static uint8_t shumi_max = 65;			//����ʪ����ֵ�����
static uint8_t shumi_min = 40;			//����ʪ����ֵ����С��
static uint8_t light_max = 100;			//������ǿ��ֵ�����
static uint8_t light_min = 30;			//������ǿ��ֵ����С��
static uint8_t wifi_sta=0; 					//Wifi����״̬
//wifi_sta 0: �Ͽ�
//         1: ������
//				 2: ģ���ʼ��ʧ��
static uint8_t atkcld_sta=0;				//ԭ��������״̬
//atkcld_sta	0:�Ͽ�
//						1:����

void System_Init(){
	lim_value.temp_upper=30;
	lim_value.temp_lower=10;
	lim_value.humi_upper=70;
	lim_value.humi_lower=40; 
	lim_value.shumi_upper=65;
	lim_value.shumi_lower=40;
	lim_value.light_upper=90;
	lim_value.light_lower=50;
	uint8_t ret;
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
 	LED_Init();	 							//��ʼ��LED	
 	Adc_Init();								//��ʼ��Adc
	KEY_Init();								//��ʼ������
	tpad_init(6);							//��ʼ�����ذ���
	Lsens_Init();							//��ʼ������������
	TS_Init();								//��ʼ������ʪ�ȴ�����(PA5)
	tp_dev.init();						//��ʼ��������
	TIM3_Int_Init(71,999);
	lv_init();								//��ʼ��lvgl
	lv_port_disp_init();
	lv_port_indev_init();
	BUMP_Init();							//��ʼ��ˮ�ü̵���(PA7)
	FUN_Init();								//��ʼ�����ȼ̵���(PA6)
	while(DHT11_Init())				 //��ʼ��������ʪ�ȴ�����(PG11)
	{
		delay_us(500);
	}
	ret = atk_mw8266d_init(115200);
	if (ret != 0)
	{
		wifi_sta=2;
		printf("ATK-MW8266D Init Failed!\r\n");
	}
}

//���´�������ֵ
void Get_Monitor_Value(void){
		DHT11_Read_Data(&temp,&humi);   		//������ʪ��
		TS_GetData(&soil_humi);         		//����ʪ��
		Lsens_Get_Val(&light_intensity);		//��������
}

//ɾ�������ӽ���
void destroy_all_children(lv_obj_t *parent) {
    lv_obj_t *child = lv_obj_get_child(parent, NULL);
    while (child) {
        lv_obj_del(child);
        child = lv_obj_get_child(parent, NULL);
    }
}

//ɾ���ҳ��
void destory_active_screen(){
	lv_obj_t *active_screen = lv_scr_act();
	destroy_all_children(active_screen);
}

// ����������
void create_main_screen(void) {
    scr_main = lv_scr_act();
    lv_obj_set_style_bg_color(scr_main, lv_color_hex(0xADD8E6), 0);  //����Ϊǳ��ɫ
    lv_obj_set_style_bg_opa(scr_main, LV_OPA_COVER, 0);

    //���������
    lv_obj_t *title_container = lv_obj_create(scr_main);
    lv_obj_set_size(title_container, 320, 40);  // ���ñ�����С����Ļ���Ϊ320��
    lv_obj_align(title_container, LV_ALIGN_TOP_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(title_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(title_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(title_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *title_label = lv_label_create(title_container);
    lv_label_set_text(title_label, "Smart Flower Pot");
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);  // �����ֺ�����
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);  // �������

    //�������򣺷��鲼��
    //ʹ�ø��Ӳ��֣������ݷֲ�Ϊ4������

    //�¶�
    lv_obj_t *temp_container = lv_obj_create(scr_main);
    lv_obj_set_size(temp_container, 140, 60);  // �����С
    lv_obj_align(temp_container, LV_ALIGN_TOP_LEFT, 10, 50);  // ���Ϸ�����
    lv_obj_set_style_bg_color(temp_container, lv_color_hex(0xFF7F7F), 0);  // ��ɫ����
    lv_obj_set_style_bg_opa(temp_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(temp_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *temp_label = lv_label_create(temp_container);
    lv_label_set_text(temp_label, "Temp");
    lv_obj_set_style_text_color(temp_label, lv_color_white(), 0);
    lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, -5);  // ��ǩλ��

    label_temp = lv_label_create(temp_container);
    lv_label_set_text(label_temp, "25 C");
    lv_obj_set_style_text_color(label_temp, lv_color_white(), 0);
    lv_obj_align(label_temp, LV_ALIGN_BOTTOM_MID, 0, 5);  // ��ֵλ��

    //ʪ��
    lv_obj_t *humi_container = lv_obj_create(scr_main);
    lv_obj_set_size(humi_container, 140, 60);  // �����С
    lv_obj_align(humi_container, LV_ALIGN_TOP_RIGHT, -10, 50);  // ���Ϸ�����
    lv_obj_set_style_bg_color(humi_container, lv_color_hex(0x00AA00), 0);  // ��ɫ����
    lv_obj_set_style_bg_opa(humi_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(humi_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *humi_label = lv_label_create(humi_container);
    lv_label_set_text(humi_label, "Humi");
    lv_obj_set_style_text_color(humi_label, lv_color_white(), 0);
    lv_obj_align(humi_label, LV_ALIGN_TOP_MID, 0, -5);  // ��ǩλ��

    label_humi = lv_label_create(humi_container);
    lv_label_set_text(label_humi, "60 %");
    lv_obj_set_style_text_color(label_humi, lv_color_white(), 0);
    lv_obj_align(label_humi, LV_ALIGN_BOTTOM_MID, 0, 5);  // ��ֵλ��

    //����ʪ��
    lv_obj_t *soil_humi_container = lv_obj_create(scr_main);
    lv_obj_set_size(soil_humi_container, 140, 60);  // �����С
    lv_obj_align(soil_humi_container, LV_ALIGN_BOTTOM_LEFT, 10, -50);  // ���·�����
    lv_obj_set_style_bg_color(soil_humi_container, lv_color_hex(0xFFD700), 0);  // ��ɫ����
    lv_obj_set_style_bg_opa(soil_humi_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(soil_humi_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *soil_humi_label = lv_label_create(soil_humi_container);
    lv_label_set_text(soil_humi_label, "Soil Humi");
    lv_obj_set_style_text_color(soil_humi_label, lv_color_white(), 0);
    lv_obj_align(soil_humi_label, LV_ALIGN_TOP_MID, 0, -5);  // ��ǩλ��

    label_soil_humi = lv_label_create(soil_humi_container);
    lv_label_set_text(label_soil_humi, "40 %");
    lv_obj_set_style_text_color(label_soil_humi, lv_color_white(), 0);
    lv_obj_align(label_soil_humi, LV_ALIGN_BOTTOM_MID, 0, 5);  // ��ֵλ��

    //����ǿ��
    lv_obj_t *light_container = lv_obj_create(scr_main);
    lv_obj_set_size(light_container, 140, 60);  // �����С
    lv_obj_align(light_container, LV_ALIGN_BOTTOM_RIGHT, -10, -50);  // ���·�����
    lv_obj_set_style_bg_color(light_container, lv_color_hex(0x00BFFF), 0);  // ��ɫ����
    lv_obj_set_style_bg_opa(light_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(light_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *light_label = lv_label_create(light_container);
    lv_label_set_text(light_label, "Light");
    lv_obj_set_style_text_color(light_label, lv_color_white(), 0);
    lv_obj_align(light_label, LV_ALIGN_TOP_MID, 0, -5);  // ��ǩλ��

    label_light = lv_label_create(light_container);
    lv_label_set_text(label_light, "80 %");
    lv_obj_set_style_text_color(label_light, lv_color_white(), 0);
    lv_obj_align(label_light, LV_ALIGN_BOTTOM_MID, 0, 5);  // ��ֵλ��

    //�ײ�ģʽ��ʾ
		lv_obj_t *mode_container = lv_obj_create(scr_main);
		lv_obj_set_size(mode_container, 320, 30);  // �����С
		lv_obj_align(mode_container, LV_ALIGN_BOTTOM_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(mode_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(mode_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(mode_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������
		
    lv_obj_t *mode_label = lv_label_create(mode_container);
    lv_label_set_text(mode_label, "Mode: ");
    lv_obj_set_style_text_color(mode_label, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(mode_label, LV_ALIGN_BOTTOM_MID, mode?-30:-20, 6);  // �ײ����ж���
		
		label_mode = lv_label_create(mode_container);
    lv_label_set_text(label_mode, "Auto");
    lv_obj_set_style_text_color(label_mode, lv_color_black(), 0);
    lv_obj_align(label_mode, LV_ALIGN_BOTTOM_MID, mode?23:18, 6);  // ��ֵλ��
}

//��������������
void update_main_screen() {
    lv_label_set_text_fmt(label_temp, "%d C", temp);
    lv_label_set_text_fmt(label_humi, "%d %%", humi);
    lv_label_set_text_fmt(label_soil_humi,"%d %%", soil_humi);
    lv_label_set_text_fmt(label_light, "%d %%", light_intensity);
		lv_label_set_text_fmt(label_mode, mode==1 ?"Manual":"Auto");
}

//�����˵�����
void create_menu_screen(void) {
    scr_menu = lv_obj_create(NULL);  // �����µ���Ļ
    lv_obj_set_style_bg_color(scr_menu, lv_color_hex(0xADD8E6), 0);  // ���ñ���Ϊǳ��ɫ
    lv_obj_set_style_bg_opa(scr_menu, LV_OPA_COVER, 0);

    //���������
    lv_obj_t *title_container = lv_obj_create(scr_menu);
    lv_obj_set_size(title_container, 320, 40);  // ���ñ�����С����Ļ���Ϊ320��
    lv_obj_align(title_container, LV_ALIGN_TOP_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(title_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(title_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(title_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *title_label = lv_label_create(title_container);
    lv_label_set_text(title_label, "Menu");
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);  // �ֺ�
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);  // �������

    //�˵�ѡ��
    //ʹ�ö��б�ǩ��ʾ�˵�ѡ��
    label_menu = lv_label_create(scr_menu);
	if(mode)
		lv_label_set_text(label_menu, "> 1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode\n  5. Manual Control");
	else
		lv_label_set_text(label_menu, "> 1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode");
    lv_obj_set_style_text_color(label_menu, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_menu, &lv_font_montserrat_16, 0);  // �ֺ�
    lv_obj_align(label_menu, LV_ALIGN_TOP_LEFT, 10, 60);  // �˵������ڶ����·�
	
		//�ײ�������ʾ
		lv_obj_t *tip_container = lv_obj_create(scr_menu);
		lv_obj_set_size(tip_container, 320, 30);  // �����С
		lv_obj_align(tip_container, LV_ALIGN_BOTTOM_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(tip_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(tip_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(tip_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������
		
    lv_obj_t *mode_label = lv_label_create(tip_container);
		lv_label_set_text(mode_label, "KEY_UP:Confrim\tKEY0: Down\tKEY1:Up");
    lv_obj_set_style_text_color(mode_label, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(mode_label, LV_ALIGN_BOTTOM_MID, 0, 6);  // �ײ����ж���
}

//���²˵�����
void update_menu_screen() {
    switch (menu_index) {
        case 0:
            if(mode)
							lv_label_set_text(label_menu, "> 1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode\n  5. Manual Control");
						else
							lv_label_set_text(label_menu, "> 1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode");
            break;
        case 1:
						if(mode)
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n> 2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode\n  5. Manual Control");
						else
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n> 2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode");
            break;
        case 2:
            if(mode)
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n  2. Set Humi Limits\n> 3. Set Light Limits\n  4. Set Mode\n  5. Manual Control");
						else
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n  2. Set Humi Limits\n> 3. Set Light Limits\n  4. Set Mode");
            break;
				case 3:
						if(mode)
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n> 4. Set Mode\n  5. Manual Control");
						else
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n> 4. Set Mode");
            break;
				case 4:
						if(mode)
							lv_label_set_text(label_menu, "  1. Set Temp Limits\n  2. Set Humi Limits\n  3. Set Light Limits\n  4. Set Mode\n> 5. Manual Control");
						break;
    }
}

void create_limit_screen() {
    // ����ҳ�棬�����ñ���Ϊǳ��ɫ
    scr_limit = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_limit, lv_color_hex(0xADD8E6), 0);  // ǳ��ɫ����
    lv_obj_set_style_bg_opa(scr_limit, LV_OPA_COVER, 0);

    // === ��������� ===
    lv_obj_t *title_container = lv_obj_create(scr_limit);
    lv_obj_set_size(title_container, 320, 40);  // ������С����320����40��
    lv_obj_align(title_container, LV_ALIGN_TOP_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(title_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(title_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(title_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������

    lv_obj_t *title_label = lv_label_create(title_container);
    lv_label_set_text(title_label, limit_page == 1 ? "Temperature Limits" :
                                     (limit_page == 2 ? "Soil Humidity Limit" : "Light Intensity Limits"));
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);  // �����ֺ�
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);  // ���з���

    // === ���ް�� ("Upper") ===
    lv_obj_t *upper_container = lv_obj_create(scr_limit);
    lv_obj_set_size(upper_container, 200, 60);  // ����С����200����60��
    lv_obj_align(upper_container, LV_ALIGN_TOP_MID, 0, 60);  // �ڶ����·����ж���
    lv_obj_set_style_bg_color(upper_container, lv_color_hex(0x87CEEB), 0);  // ǳ��ɫ����
    lv_obj_set_style_bg_opa(upper_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(upper_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù���

    // �����ʾ "Upper" ��ǩ
    lv_obj_t *label_upper = lv_label_create(upper_container);
    lv_label_set_text(label_upper, "Upper");  // ���ޱ�ǩ
    lv_obj_set_style_text_color(label_upper, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_upper, &lv_font_montserrat_16, 0);  // �ֺ�
    lv_obj_align(label_upper, LV_ALIGN_LEFT_MID, 10, 0);  // �����룬����10px

    // �Ҳ���ʾ��ǰ����ֵ
    label_upper_value = lv_label_create(upper_container);
    lv_label_set_text_fmt(label_upper_value, limit_page == 1 ? "%d C" : "%d %%", 
                          limit_page == 1 ? lim_value.temp_upper :
                          (limit_page == 2 ? lim_value.humi_upper : lim_value.light_upper));  // Ĭ������ֵ
    lv_obj_set_style_text_color(label_upper_value, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_upper_value, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(label_upper_value, LV_ALIGN_RIGHT_MID, -10, 0);  // �Ҳ���룬����10px

    // === ���ް�� ("Lower") ===
    lv_obj_t *lower_container = lv_obj_create(scr_limit);
    lv_obj_set_size(lower_container, 200, 60);  // ����С����200����60��
    lv_obj_align(lower_container, LV_ALIGN_TOP_MID, 0, 130);  // �����ް���·�����
    lv_obj_set_style_bg_color(lower_container, lv_color_hex(0xFFEC8B), 0);  // ǳ��ɫ����
    lv_obj_set_style_bg_opa(lower_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(lower_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù���

    // �����ʾ "Lower" ��ǩ
    lv_obj_t *label_lower = lv_label_create(lower_container);
    lv_label_set_text(label_lower, "Lower");  // ���ޱ�ǩ
    lv_obj_set_style_text_color(label_lower, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_lower, &lv_font_montserrat_16, 0);  // �ֺ�
    lv_obj_align(label_lower, LV_ALIGN_LEFT_MID, 10, 0);  // �����룬����10px

    // �Ҳ���ʾ��ǰ����ֵ
    label_lower_value = lv_label_create(lower_container);
    lv_label_set_text_fmt(label_lower_value, limit_page == 1 ? "%d C" : "%d %%", 
                          limit_page == 1 ? lim_value.temp_lower :
                          (limit_page == 2 ? lim_value.humi_lower : lim_value.light_lower));  // Ĭ������ֵ
    lv_obj_set_style_text_color(label_lower_value, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_lower_value, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(label_lower_value, LV_ALIGN_RIGHT_MID, -10, 0);  // �Ҳ���룬����10px
		
		//�ײ�������ʾ
		lv_obj_t *tip_container = lv_obj_create(scr_limit);
		lv_obj_set_size(tip_container, 320, 30);  // �����С
		lv_obj_align(tip_container, LV_ALIGN_BOTTOM_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(tip_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(tip_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(tip_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������
		
    lv_obj_t *mode_label = lv_label_create(tip_container);
		lv_label_set_text(mode_label, "KEY0: Lower+\tKEY1:Upper+");
    lv_obj_set_style_text_color(mode_label, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(mode_label, LV_ALIGN_BOTTOM_MID, 0, 6);  // �ײ����ж���
}


//���������޽�������
void update_limit_value() {	
		lv_label_set_text_fmt(label_upper_value,limit_page==1?"%d C":"%d %%",limit_page==1?lim_value.temp_upper:(limit_page==2?lim_value.shumi_upper:lim_value.light_upper));  // ��������ֵ
		lv_label_set_text_fmt(label_lower_value,limit_page==1?"%d C":"%d %%",limit_page==1?lim_value.temp_lower:(limit_page==2?lim_value.shumi_lower:lim_value.light_lower));  // ��������ֵ
}



// �����ֶ���������
void create_manual_screen(void) {
     // �����ֶ�������Ļ�������ñ�����ɫΪǳ��ɫ
    scr_manual = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_manual, lv_color_hex(0xADD8E6), 0);  // ǳ��ɫ����
    lv_obj_set_style_bg_opa(scr_manual, LV_OPA_COVER, 0);

    // === ��������� ===
    lv_obj_t *title_container = lv_obj_create(scr_manual);
    lv_obj_set_size(title_container, 320, 40);  // ������С����320����40��
    lv_obj_align(title_container, LV_ALIGN_TOP_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(title_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(title_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(title_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù���

    lv_obj_t *title_label = lv_label_create(title_container);
    lv_label_set_text(title_label, "Manual Operation");
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);  // �����ֺ�
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);  // ���з���

    // === ��ˮ��� ("Water") ===
    lv_obj_t *water_container = lv_obj_create(scr_manual);
    lv_obj_set_size(water_container, 140, 100);  // ����С
    lv_obj_align(water_container, LV_ALIGN_LEFT_MID, 20, 0);  // �����ж���
    lv_obj_set_style_bg_color(water_container, lv_color_hex(0x87CEEB), 0);  // ��ɫ����
    lv_obj_set_style_bg_opa(water_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(water_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù���

    // ��ʾ "Water" ��ǩ
    lv_obj_t *label_water = lv_label_create(water_container);
    lv_label_set_text(label_water, "Water");
    lv_obj_set_style_text_color(label_water, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_water, &lv_font_montserrat_16, 0);  // �ֺ�
    lv_obj_align(label_water, LV_ALIGN_TOP_MID, 0, 10);  // �����ڰ�鶥��

    // ��ʾ״̬ "Open" �� "Close"
    label_water_status = lv_label_create(water_container);
    lv_label_set_text(label_water_status, water_status?"Open":"Close");  // Ĭ��״̬Ϊ Close
    lv_obj_set_style_text_color(label_water_status, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_water_status, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(label_water_status, LV_ALIGN_BOTTOM_MID, 0, -10);  // �����ڰ��ײ�

    // === ������ ("Light") ===
    lv_obj_t *light_container = lv_obj_create(scr_manual);
    lv_obj_set_size(light_container, 140, 100);  // ����С
    lv_obj_align(light_container, LV_ALIGN_RIGHT_MID, -20, 0);  // �Ҳ���ж���
    lv_obj_set_style_bg_color(light_container, lv_color_hex(0xFFEC8B), 0);  // ��ɫ����
    lv_obj_set_style_bg_opa(light_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(light_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù���

    // ��ʾ "Light" ��ǩ
    lv_obj_t *label_light = lv_label_create(light_container);
    lv_label_set_text(label_light, "Light");
    lv_obj_set_style_text_color(label_light, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_light, &lv_font_montserrat_16, 0);  // �ֺ�
    lv_obj_align(label_light, LV_ALIGN_TOP_MID, 0, 10);  // �����ڰ�鶥��

    // ��ʾ״̬ "Open" �� "Close"
    label_light_status = lv_label_create(light_container);
    lv_label_set_text(label_light_status, light_status?"Open":"Close");  // Ĭ��״̬Ϊ Close
    lv_obj_set_style_text_color(label_light_status, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(label_light_status, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(label_light_status, LV_ALIGN_BOTTOM_MID, 0, -10);  // �����ڰ��ײ�
		
				//�ײ�������ʾ
		lv_obj_t *tip_container = lv_obj_create(scr_limit);
		lv_obj_set_size(tip_container, 320, 30);  // �����С
		lv_obj_align(tip_container, LV_ALIGN_BOTTOM_MID, 0, 0);  // �������ж���
    lv_obj_set_style_bg_color(tip_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(tip_container, LV_OPA_COVER, 0);
    lv_obj_clear_flag(tip_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������
		
    lv_obj_t *mode_label = lv_label_create(tip_container);
		lv_label_set_text(mode_label, "KEY0: Water\tKEY1:Light");
    lv_obj_set_style_text_color(mode_label, lv_color_black(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(mode_label, LV_ALIGN_BOTTOM_MID, 0, 6);  // �ײ����ж���
}

// �����ֶ�����
void update_manual_screen() {
		lv_label_set_text(label_light_status, light_status?"Open":"Close");
		lv_label_set_text(label_water_status, water_status?"Open":"Close");
}

//���ٵ���
void hide_and_destroy_popup(lv_timer_t *timer)
{
    if (popup_container) {
        lv_obj_del(popup_container);  // ���ٵ������������Ӷ���
        popup_container = NULL;      // ����Ϊ NULL����ֹ�ظ�����
        popup_label = NULL;          // ͬ����ձ�ǩָ��
    }
    lv_timer_del(timer);  // ���ٶ�ʱ��
}

// ��������
void create_popup(void) {
		if (popup_container) {
        return;
    }
    // ������������
    popup_container = lv_obj_create(current_screen==SCREEN_MAIN?scr_main:(current_screen==SCREEN_MENU)?scr_menu:(current_screen==SCREEN_MANUAL)?scr_manual:scr_limit);  // ��ӵ���
    lv_obj_set_size(popup_container, 200, 40);  // ���õ�����С (��200����40)
    lv_obj_align(popup_container, LV_ALIGN_BOTTOM_RIGHT, -10, -10);  // ��Ļ���½ǣ�����ײ����Ҳ��Ե10����
    lv_obj_set_style_bg_color(popup_container, lv_color_hex(0x4682B4), 0);  // ����ɫ����
    lv_obj_set_style_bg_opa(popup_container, LV_OPA_COVER, 0);  // ��͸������
    lv_obj_set_style_radius(popup_container, 8, 0);  // Բ����ʽ
    lv_obj_clear_flag(popup_container, LV_OBJ_FLAG_SCROLLABLE);  // ���ù�������
    lv_obj_add_flag(popup_container, LV_OBJ_FLAG_HIDDEN);  // Ĭ�����ص���

    // ���������ı���ǩ
    popup_label = lv_label_create(popup_container);
    lv_label_set_text(popup_label, "");  // Ĭ��������
    lv_obj_set_style_text_color(popup_label, lv_color_white(), 0);  // ��ɫ����
    lv_obj_set_style_text_font(popup_label, &lv_font_montserrat_14, 0);  // �ֺ�
    lv_obj_align(popup_label, LV_ALIGN_CENTER, 0, 0);  // ���ж���
}

//���ٵ���
void hide_popup(lv_timer_t *timer) {
    lv_obj_add_flag(popup_container, LV_OBJ_FLAG_HIDDEN);  // ���ص���
    lv_timer_del(timer);  // ɾ����ʱ��
}

// ��ʾ����
void show_popup(const char *message, uint32_t duration_ms) {
		lv_label_set_text(popup_label, message);  // ���õ�������
		lv_obj_clear_flag(popup_container, LV_OBJ_FLAG_HIDDEN);  // ��ʾ����
		// ���ö�ʱ�����Զ����ص���
		lv_timer_t *timer = lv_timer_create(hide_and_destroy_popup, duration_ms, NULL);
		lv_timer_set_repeat_count(timer, 1);  // ֻ����һ��
}


// ����˵�ѡ��
void handle_menu_selection(int index) {
    switch (index) {
			case 0:
					// ���������¶������޽���
					limit_page=1;
					destory_active_screen();
					create_limit_screen();
					current_screen = SCREEN_LIMIT;
					lv_scr_load(scr_limit);
					break;
			case 1:
					// ��������ʪ�������޽���
					limit_page=2;
					destory_active_screen();
					create_limit_screen();
					current_screen = SCREEN_LIMIT;
					lv_scr_load(scr_limit);
					break;
			case 2:
					// �������ù�ǿ�����޽���
					limit_page=3;
					destory_active_screen();
					create_limit_screen();
					current_screen = SCREEN_LIMIT;
					lv_scr_load(scr_limit);
					break;
			case 3:
					//�л�����ģʽ
					mode=!mode;
					menu_item=mode?5:4;
					menu_index=0;
					destory_active_screen();
					create_menu_screen();
					current_screen = SCREEN_MENU;
					lv_scr_load(scr_menu);
					create_popup();
					show_popup(mode?"Manual Mode":"Auto Mode", 3000);
					break;
			case 4:
					//�����ֶ���������
					destory_active_screen();
					create_manual_screen();
					current_screen = SCREEN_MANUAL;
					lv_scr_load(scr_manual);
					break;
    }
}


void UI_Switch(uint8_t key){
	if(current_screen==SCREEN_MAIN){	//�������°�ť����
		switch(key){
			case WKUP_PRES:								//����Key_up��������˵�
				destory_active_screen();
				create_menu_screen();
				current_screen = SCREEN_MENU;
				lv_scr_load(scr_menu);
				break;
			case KEY0_PRES:
				break;
			case KEY1_PRES:
				break;
			case 10:
				break;
		}
	}else if(current_screen==SCREEN_MENU){	//�˵������°�ť����
		switch(key){
			case WKUP_PRES:											//����Key_up���뵱ǰѡ�й���
				handle_menu_selection(menu_index);
				menu_index=0;
				break;
			case KEY0_PRES:											//����Key0�˵�����
				menu_index = (menu_index + 1) % menu_item; 
				update_menu_screen();
				break;
			case KEY1_PRES:											//����Key1�˵�����
				menu_index = (menu_index - 1 + menu_item) % menu_item;
				update_menu_screen();
				break;
			case 10:														//����TPAD������һҳ�棨��ҳ�棩
				menu_index=0;
				destory_active_screen();
				create_main_screen();
				lv_scr_load(scr_main);
				current_screen = SCREEN_MAIN;
		}
	}else if(current_screen==SCREEN_MANUAL){ //�ֶ�ҳ���°�ť����
		switch(key){
			case WKUP_PRES:
				break;
			case KEY0_PRES:											//����Key1���ؽ�ˮ
				if(water_status){
					water_status=0;
					BUMP_OFF;
				}
				else{
					water_status=1;
					BUMP_ON;
				}
				update_manual_screen();
				break;
			case KEY1_PRES:											//����Key1���ز���
				if(light_status){
					light_status=0;
					LED1 = 1;
				}
				else{
					light_status=1;
					LED1 = 0;
				}
				update_manual_screen();
				break;
			case 10:														//����TPAD������һҳ�棨�˵���
				destory_active_screen();
				create_menu_screen();
				lv_scr_load(scr_menu);
				current_screen = SCREEN_MENU;
		}
	}else if(current_screen==SCREEN_LIMIT){	//�����޵���ҳ���°�ť����
		switch(key){
			case WKUP_PRES:
				break;
			case KEY1_PRES:	
				if(limit_page==1){				//����Key1��������
					lim_value.temp_upper=(lim_value.temp_upper+10)<=temp_max?lim_value.temp_upper+10:temp_min;
				}else if(limit_page==2){
					lim_value.shumi_upper=(lim_value.shumi_upper+10)<=shumi_max?lim_value.shumi_upper+10:shumi_min;
				}else{
					lim_value.light_upper=(lim_value.light_upper+10)<=light_max?lim_value.light_upper+10:light_min;
				}
				update_limit_value();
				break;
			case KEY0_PRES:							//����Key0��������
				if(limit_page==1){
					lim_value.temp_lower=(lim_value.temp_lower+10)<=temp_max?lim_value.temp_lower+10:temp_min;
				}else if(limit_page==2){
					lim_value.shumi_lower=(lim_value.shumi_lower+10)<=shumi_max?lim_value.shumi_lower+10:shumi_min;
				}else{
					lim_value.light_lower=(lim_value.light_lower+10)<=light_max?lim_value.light_lower+10:light_min;
				}
				update_limit_value();
				break;
			case 10:										//����TPAD������һҳ�棨�˵���
				destory_active_screen();
				create_menu_screen();
				lv_scr_load(scr_menu);
				current_screen = SCREEN_MENU;
				break;
		}
	}
}

//��������
void Warn_function(void){
	if(current_screen!=SCREEN_MAIN)
		return;
	if(temp>lim_value.temp_upper){
		//��������
			FUN_ON;
			lv_obj_set_style_text_color(label_temp, lv_color_hex(0xFF0000), 0);
	}else if(temp<lim_value.temp_lower){
			FUN_OFF;
			lv_obj_set_style_text_color(label_temp, lv_color_hex(0x0000FF), 0);
	}else{
			FUN_OFF;
			lv_obj_set_style_text_color(label_temp, lv_color_white(), 0);
	}
	if(humi>lim_value.humi_upper){
			lv_obj_set_style_text_color(label_humi, lv_color_hex(0xFF0000), 0);
	}else if(humi<lim_value.humi_lower){
			lv_obj_set_style_text_color(label_humi, lv_color_hex(0x0000FF), 0);
	}else{
			lv_obj_set_style_text_color(label_humi, lv_color_white(), 0);
	}
	if(soil_humi>lim_value.shumi_upper){
			if(!mode&&water_status){
				BUMP_OFF;
				water_status=0;
			}
			lv_obj_set_style_text_color(label_soil_humi, lv_color_hex(0xFF0000), 0);
	}else if(soil_humi<lim_value.shumi_lower){
			if(!mode&&!water_status){
				BUMP_ON;
				water_status=1;
			}
			lv_obj_set_style_text_color(label_soil_humi, lv_color_hex(0x0000FF), 0);
	}else{
			lv_obj_set_style_text_color(label_soil_humi, lv_color_white(), 0);
	}
	if(light_intensity>lim_value.light_upper){
			lv_obj_set_style_text_color(label_light, lv_color_hex(0xFF0000), 0);
			if(!mode&&light_status){
				LED1=1;
				light_status=0;
			}
	}else if(light_intensity<lim_value.light_lower){
			lv_obj_set_style_text_color(label_light, lv_color_hex(0x0000FF), 0);
			if(!mode&&!light_status){
				LED1=0;
				light_status=1;
			}
	}else{
			lv_obj_set_style_text_color(label_light, lv_color_white(), 0);
	}
}

void wireless_control(){
	uint8_t exe=execute();
	if(!mode&&exe!=8){
		return;
	}
	if(exe==1){
		light_status=1;
		LED1=0;
		create_popup();
		show_popup("Light ON", 2000);
	}else if(exe==2){
		light_status=0;
		LED1=1;
		create_popup();
		show_popup("Light OFF", 2000);
	}else if(exe==3){
		water_status=1;
		BUMP_ON;
		create_popup();
		show_popup("Water ON", 2000);
	}else if(exe==4){
		water_status=0;
		BUMP_OFF;
		create_popup();
		show_popup("Water OFF", 2000);
	}else if(exe==5){
		FUN_ON;
		create_popup();
		show_popup("Fun ON", 2000);
	}else if(exe==6){
		FUN_OFF;
		create_popup();
		show_popup("Fun OFF", 2000);
	}else if(exe==7){
		mode=0;
		menu_item=mode?5:4;
		menu_index=0;
		if(current_screen==SCREEN_MAIN){
			update_main_screen();
		}else if(current_screen==SCREEN_MENU){
			update_menu_screen();
		}
		create_popup();
		show_popup(mode?"Manual Mode":"Auto Mode", 2000);
	}else if(exe==8){
		mode=1;
		menu_item=mode?5:4;
		menu_index=0;
		if(current_screen==SCREEN_MAIN){
			update_main_screen();
		}else if(current_screen==SCREEN_MENU){
			update_menu_screen();
		}
		create_popup();
		show_popup(mode?"Manual Mode":"Auto Mode", 2000);
	}else{
		return;
	}
	if(current_screen==SCREEN_MANUAL)
		update_manual_screen();
}


int main(void)
{
		uint8_t t=0;
		System_Init();
		create_main_screen();
		if(wifi_sta==2||wifi_connect()){
			create_popup();
			show_popup("WiFi Connect Failed!", 3000);
			wifi_sta=0;
		}else{
			wifi_sta=1;
			create_popup();
			show_popup("WiFi Connected!", 3000);
		}
		if(wifi_sta){
			atkcld_con(&atkcld_sta);
		}
		while(1)
		{
			uint8_t key = KEY_Scan(0);
			UI_Switch(key);
      // ���ذ�ť����
			if(tpad_scan(0)){
				UI_Switch(10);
			}
			Get_Monitor_Value();
			if(t==4){
				Warn_function();
				if(atkcld_sta)
					send_data_to_cloud(atkcld_sta,temp,humi,soil_humi,light_intensity);
				t=0;
			}
			wireless_control();
			if(current_screen == SCREEN_MAIN) {
				update_main_screen();
			}
			lv_timer_handler();
			delay_ms(50);
			t++;
		}
}



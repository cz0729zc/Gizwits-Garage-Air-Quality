#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "timer4.h"
#include "OLED.h"
#include "dht11.h"
#include "Beep.h"
#include "fan.h"
#include "PM25.h"
#include "AD.h"
#include "keys.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"


// 全局变量
u8 g_temp;
u8 g_humi;
float g_PM25;
float g_MQ2;
float g_MQ135;
int threshold = 100;  // 默认阈值
u8 in_threshold_menu = 0;  // 是否在阈值设置菜单

#define V0_ADC 190   // 洁净空气ADC值
#define REF_ADC 2700 // 1000ppm时的ADC值（需标定）
#define REF_PPM 1000.0f

// 函数声明
float adc_to_ppm(uint16_t adc_value);
void check_alarm_conditions(void);
void handle_threshold_menu(void);
void update_main_display(void);
void update_threshold_display(void);

int main(void)
{
    int ret = 0;
    int AD_MQ2, AD_MQ135;
    
    OLED_Init();
    OLED_Clear();
    DHT11_Init();
    AD_Init();           // AD需要放在串口前初始化不然串口失灵
    Usart1_Init(9600);
	Usart2_Init(9600);

	TIM4_Init(1000-1, 72-1);  // 初始化1ms定时为机智云协议提供时基
	TIM_Cmd(TIM4, ENABLE);    // 启动定时器
	
    FAN_Init();
    BEEP_Init();
    PM25_Init();
    Key_Init();          // 初始化按键
    
    // 显示静态信息
    update_main_display();

    //机智云初始化
    userInit();
    delay_ms(300);
    gizwitsInit();
    delay_ms(300);

    gizwitsSetMode(WIFI_SOFTAP_MODE);
    
    while(1) {
        ret = DHT11_Read_Data(&g_temp, &g_humi);
        
        PM25_Data PM25 = PM25_GetCurrentData();
        g_PM25 = PM25.pm25_value;
        
        AD_MQ2 = AD_Value[0];    // 原始模拟量
        AD_MQ135 = AD_Value[1];
        
        g_MQ2 = adc_to_ppm(AD_MQ2);
        g_MQ135 = adc_to_ppm(AD_MQ135);

        // 机智云协议处理

        
        // 处理按键
        Key_Value key = Key_Scan();
        if(key == KEY_SETTING_PRESSED) {
            in_threshold_menu = !in_threshold_menu;
            if(in_threshold_menu) {
                OLED_Clear();
                update_threshold_display();
            } else {
                OLED_Clear();
                update_main_display();
            }
        }
        
		if(in_threshold_menu) {
			// 处理阈值调整
			if(key == KEY_INCREASE_PRESSED) {
				threshold += 5;
				if(threshold > 999) threshold = 999;
				update_threshold_display();
			}
			else if(key == KEY_DECREASE_PRESSED) {
				threshold -= 5;
				if(threshold < 0) threshold = 0;
				update_threshold_display();
			}
		} else {
			update_main_display();
			check_alarm_conditions();
		}
        

        userHandle();
        gizwitsHandle((dataPoint_t *)&currentDataPoint);

        //u1_printf("Temp: %d Humi: %d PM25: %0.2f MQ2: %0.2f MQ135: %0.2f\r\n", 
        //         g_temp, g_humi, g_PM25, g_MQ2, g_MQ135);
        delay_ms(100);
    }
}

float adc_to_ppm(uint16_t adc_value)
{
    float V0 = (V0_ADC * 3.3f) / 4096.0f;       // 基准电压
    float V_ref = (REF_ADC * 3.3f) / 4096.0f;   // 参考电压
    float V_out = (adc_value * 3.3f) / 4096.0f; // 当前电压

    float ppm = ((V_out - V0) / (V_ref - V0)) * REF_PPM;
    return (ppm > 0) ? ppm : 0; // 浓度不为负
}

void check_alarm_conditions(void)
{
    static u8 alarm_active = 0;
    static u32 alarm_start_time = 0;
    
    // 检查是否有传感器超过阈值
    if(g_PM25 > threshold || g_MQ2 > threshold || g_MQ135 > threshold) {
        if(!alarm_active) {
            alarm_active = 1;
            alarm_start_time = 0;
            
            BEEP_On();
            FAN_On();
        }
    }
    
    // 处理报警持续时间
    if(alarm_active) {
        alarm_start_time += 100; // 每次循环增加100ms
        
        // 蜂鸣器响3秒
        if(alarm_start_time >= 3000) {
            BEEP_Off();
        }
        
        // 风扇转5秒
        if(alarm_start_time >= 5000) {
            FAN_Off();
            alarm_active = 0;
        }
    }
}


void update_main_display(void)
{
    OLED_ShowString(1, 1, "T: ");
    OLED_ShowString(1, 6, "C");
    OLED_ShowString(1, 9, "H: ");
    OLED_ShowString(1, 14, "%");
    OLED_ShowString(2, 1, "MQ2:  ");
    OLED_ShowString(3, 1, "MQ135: ");
    OLED_ShowString(4, 1, "PM25: ");
    OLED_ShowString(4, 12, "ug/m3");
    
    OLED_ShowNum(1, 4, g_temp, 2);
    OLED_ShowNum(1, 12, g_humi, 2);
    OLED_ShowNum(4, 7, g_PM25, 4);
    OLED_ShowNum(2, 8, g_MQ2, 3);
    OLED_ShowNum(3, 8, g_MQ135, 3);
}

void update_threshold_display(void)
{
    OLED_ShowString(1, 1, "Thre Setting");
    OLED_ShowString(2, 1, "Current:");
    OLED_ShowNum(2, 10, threshold, 3);
    OLED_ShowString(3, 1, "KEY2: +  KEY3: -");
    OLED_ShowString(4, 1, "KEY1: Exit");
}


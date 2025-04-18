#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"

#include "OLED.h"
#include "dht11.h"
#include "Beep.h"
#include "fan.h"
#include "PM25.h"
//#include "MQ2.h"
//#include "MQ135.h"
#include "AD.h"

//全局变量
u8 g_temp;
u8 g_humi;
float g_PM25;
float g_MQ2;
float g_MQ135;

#define V0_ADC 210   // 洁净空气ADC值
#define REF_ADC 2700 // 1000ppm时的ADC值（需标定）
#define REF_PPM 1000.0f

//函数声明
float adc_to_ppm(uint16_t adc_value);
int main(void)
{
	int ret = 0;
	int AD_MQ2,AD_MQ135;
	
	OLED_Init();
    OLED_Clear();
	DHT11_Init();
	
//	MQ2_Init();
//	MQ135_Init();
	AD_Init();           //AD需要放在串口前初始化不然串口失灵
    Usart1_Init(9600);
    FAN_Init();
    BEEP_Init();
	PM25_Init();
	
    //显示静态信息
    OLED_ShowString(1, 1, "T: ");
    OLED_ShowString(1, 6, "C");
    OLED_ShowString(1, 9, "H: ");
	OLED_ShowString(1, 14, "%");
    OLED_ShowString(2, 1, "MQ2:  ");
    OLED_ShowString(3, 1, "MQ135: ");
    while(1) {
		
		//u1_printf("hello\r\n");
		
		ret = DHT11_Read_Data(&g_temp, &g_humi);
//        if(ret == 0) {
//            OLED_ShowString(4, 1, "DHT11 OK");
//        } else {
//            OLED_ShowString(4, 1, "DHT11 Error");
//        }
		
        PM25_Data PM25 = PM25_GetCurrentData();

//        if(PM25.data_valid == 1) {
//            OLED_ShowString(4, 1, "PM25 OK    ");
//        } else {
//            OLED_ShowString(4, 1, "PUT ON PM25");
//        }

//		g_PM25 = PM25.pm25_value;
		
		AD_MQ2 =AD_Value[0];    //原始模拟量
        AD_MQ135 = AD_Value[1];
		
		g_MQ2 = adc_to_ppm(AD_MQ2);
		g_MQ135 = adc_to_ppm(AD_MQ135);
		
        OLED_ShowNum(1, 4, g_temp, 2);
        OLED_ShowNum(1, 12, g_humi, 2);
//        OLED_ShowString(3, 1, "PM25: ");
//        OLED_ShowNum(3, 7, g_PM25, 4);
//        OLED_ShowString(3, 12, "ug/m3");

        OLED_ShowNum(2, 8, g_MQ2, 3);
        OLED_ShowNum(3, 8, g_MQ135, 3);
		


        u1_printf("Temp: %d Humi: %d PM25: %0.2f MQ2: %0.2f MQ135: %0.2f\r\n", g_temp, g_humi, g_PM25, g_MQ2, g_MQ135);
        delay_ms(1000);
    
		
//		BEEP_On();
//        delay_ms(1000);
//        BEEP_Off();
//        FAN_On();
//        delay_ms(1000);
//        FAN_Off();
		
		
		
		
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
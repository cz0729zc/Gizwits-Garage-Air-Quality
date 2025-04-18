#include "MQ135.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include <math.h>

#define ADC1_DR_Address    ((uint32_t)0x4001244C)  // ADC1���ݼĴ�����ַ

static volatile uint16_t ADC_ConvertedValue = 0;

void MQ135_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // 1. ʹ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 2. ����PA5Ϊģ������
    GPIO_InitStructure.GPIO_Pin = MQ135_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(MQ135_GPIO_PORT, &GPIO_InitStructure);

    // 3. ����DMA��ͨ��1����ADC1��
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 4. ����ADC1��ͨ��5��ӦPA5��
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, MQ135_ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // 5. ADCУ׼
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    // 6. ����ת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t MQ135_ReadValue(void) {
    return ADC_ConvertedValue;
}

float ConvertToAirQuality(uint16_t adc_value) {
    // ��ADCֵת��Ϊ��ѹ��3.3V�ο���ѹ��
    float voltage = (adc_value * 3.3f) / 4096.0f;
    
    // ���㴫��������RS�����踺�ص���RL=10k����
    float RS = (3.3f - voltage) * 10.0f / voltage;
    
    // ����RS/R0��ֵ����Ҫ�ڸɾ�������У׼R0��
    static float R0 = 35.0f; // ʾ��ֵ����ʵ��У׼
    
    // ʹ��CO?���Ƽ��㹫ʽ����Ҫ���������ֲ����������
    float ratio = RS / R0;
    float ppm = 116.602f * powf(ratio, -2.769f); // ���CO?�ľ��鹫ʽ
    
    return ppm;
}


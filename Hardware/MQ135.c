#include "MQ135.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include <math.h>

#define ADC1_DR_Address    ((uint32_t)0x4001244C)  // ADC1数据寄存器地址

static volatile uint16_t ADC_ConvertedValue = 0;

void MQ135_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 2. 配置PA5为模拟输入
    GPIO_InitStructure.GPIO_Pin = MQ135_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(MQ135_GPIO_PORT, &GPIO_InitStructure);

    // 3. 配置DMA（通道1用于ADC1）
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

    // 4. 配置ADC1（通道5对应PA5）
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

    // 5. ADC校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    // 6. 启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t MQ135_ReadValue(void) {
    return ADC_ConvertedValue;
}

float ConvertToAirQuality(uint16_t adc_value) {
    // 将ADC值转换为电压（3.3V参考电压）
    float voltage = (adc_value * 3.3f) / 4096.0f;
    
    // 计算传感器电阻RS（假设负载电阻RL=10kΩ）
    float RS = (3.3f - voltage) * 10.0f / voltage;
    
    // 计算RS/R0比值（需要在干净空气中校准R0）
    static float R0 = 35.0f; // 示例值，需实际校准
    
    // 使用CO?近似计算公式（需要根据数据手册调整参数）
    float ratio = RS / R0;
    float ppm = 116.602f * powf(ratio, -2.769f); // 针对CO?的经验公式
    
    return ppm;
}


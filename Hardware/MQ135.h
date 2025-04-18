#ifndef __MQ135_H
#define __MQ135_H

#include "stm32f10x.h"

#define MQ135_GPIO_PORT    GPIOA
#define MQ135_GPIO_PIN     GPIO_Pin_5
#define MQ135_ADC_CHANNEL  ADC_Channel_5  // PA5对应ADC1通道5

void MQ135_Init(void);
uint16_t MQ135_ReadValue(void);
float ConvertToAirQuality(uint16_t adc_value);

#endif
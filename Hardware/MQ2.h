#ifndef __MQ2_H
#define __MQ2_H

#include "stm32f10x.h"

#define MQ2_GPIO_PORT    GPIOA
#define MQ2_GPIO_PIN     GPIO_Pin_4
#define MQ2_ADC_CHANNEL  ADC_Channel_4  // PA4对应ADC1通道4

void MQ2_Init(void);
uint16_t MQ2_ReadValue(void);
float ConvertToSmokeLevel(uint16_t adc_value);

#endif


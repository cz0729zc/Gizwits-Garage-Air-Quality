#ifndef __FAN_H
#define __FAN_H

#include "stm32f10x.h"

// ??????
#define FAN_PIN     GPIO_Pin_11     // ??PA11??
#define FAN_GPIO    GPIOA

void FAN_Init(void);               // ?????
void FAN_On(void);                 // ????
void FAN_Off(void);                // ????
void FAN_Control(uint8_t flag);    // ?????????

#endif


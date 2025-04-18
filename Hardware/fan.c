#include "fan.h"
#include "delay.h"

// 初始化风扇引脚（PA11）
void FAN_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置PA11为推挽输出（假设风扇高电平驱动）
    GPIO_InitStructure.GPIO_Pin = FAN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FAN_GPIO, &GPIO_InitStructure);

    // 默认关闭风扇
    FAN_Off();
}

// 开启风扇（PA11输出高电平）
void FAN_On(void) {
    GPIO_SetBits(FAN_GPIO, FAN_PIN);
}

// 关闭风扇（PA11输出低电平）
void FAN_Off(void) {
    GPIO_ResetBits(FAN_GPIO, FAN_PIN);
}

// 根据标志位控制风扇
void FAN_Control(uint8_t flag) {
    if (flag) {
        FAN_On();   // 开启
    } else {
        FAN_Off();  // 关闭
    }
}


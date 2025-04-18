#ifndef __PM25_H
#define __PM25_H

#include "stm32f10x.h"

// 数据帧格式定义
#define FRAME_HEADER      0xFF    // 帧头标识
#define DATA_LENGTH       9       // 数据帧长度
#define CHECKSUM_POS      8       // 校验和位置


// PM2.5数据结构体
typedef struct {
    float duty_cycle;      // 占空比（0.5%~50%）
    float pm25_value;      // PM2.5浓度
    uint8_t data_valid;    // 数据有效性标志
} PM25_Data;

// 函数声明
void PM25_Init(void);
void PM25_ReceiveHandler(uint8_t data);
PM25_Data PM25_GetCurrentData(void);

#endif


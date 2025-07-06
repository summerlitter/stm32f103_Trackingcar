#ifndef __SPEED_H
#define __SPEED_H

#include "stm32f10x.h"

// 编码器初始化函数
void Encoder_Init_TIM2_Left(void);
void Encoder_Init_TIM3_Right(void);

// 读取编码器速度
short Read_Encoder(uint8_t TIMX);

// 发送速度数据
void Send_Encoder_Speeds(void);

#endif
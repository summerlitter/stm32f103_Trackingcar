#ifndef __TIME_H
#define __TIME_H

#include "stm32f10x.h"  // 根据实际使用的芯片头文件调整

// 初始化延时函数所需的定时器
void time_init(void);

// 微秒级延时函数
void mydelay_us(uint32_t us);

#endif /* __TIME_H */
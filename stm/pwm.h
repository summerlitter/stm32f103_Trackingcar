#ifndef _PWM_H
#define _PWM_H

#include "stm32f10x.h"

// 初始化函数
void pwm_motor_init(void);

// 设置左右轮PWM占空比
void set_left_pwm(int value);   // value: 0 ~ 1000
void set_right_pwm(int value);

// 控制方向
void set_left_direction(uint8_t dir);   // 1: 正转，0: 反转
void set_right_direction(uint8_t dir);

#endif /* __PWM_MOTOR_H */
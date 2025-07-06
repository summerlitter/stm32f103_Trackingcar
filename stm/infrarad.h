#ifndef __INFRARAD_H
#define __INFRARAD_H

#include "stm32f10x.h"

// 定义红外传感器（循迹模块）的引脚
#define IR_PINA_1_DO GPIO_Pin_4    // track_L (左循迹), 连接到 GPIOA 的 Pin4
#define IR_PINA_2_DO GPIO_Pin_5    // track_M (中循迹), 连接到 GPIOA 的 Pin5
#define IR_PINB_1_DO GPIO_Pin_1    // track_R (右循迹), 连接到 GPIOB 的 Pin1

#define IR_GPIO_PORT_A GPIOA       // 左和中循迹传感器的 GPIO 端口
#define IR_GPIO_PORT_B GPIOB       // 右循迹传感器的 GPIO 端口

// 初始化红外传感器（循迹模块）
void infrarad_init(void);

// 获取红外传感器（循迹模块）的状态
uint8_t get_ir_state(uint8_t sensor_num);

#endif /* __INFRARAD_H */
#include "infrarad.h"

// 初始化红外传感器
void infrarad_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 GPIOA 和 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 配置 GPIOA 的 Pin4 和 Pin5 为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = IR_PINA_1_DO | IR_PINA_2_DO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入模式
    GPIO_Init(IR_GPIO_PORT_A, &GPIO_InitStructure);

    // 配置 GPIOB 的 Pin1 为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = IR_PINB_1_DO;
    GPIO_Init(IR_GPIO_PORT_B, &GPIO_InitStructure);
}

// 获取红外传感器的状态
uint8_t get_ir_state(uint8_t sensor_num)
{
    uint8_t state = 0;
    switch (sensor_num) {
        case 1: 
            state = GPIO_ReadInputDataBit(IR_GPIO_PORT_A, IR_PINA_1_DO); 
            break;
        case 2: 
            state = GPIO_ReadInputDataBit(IR_GPIO_PORT_A, IR_PINA_2_DO); 
            break;
        case 3: 
            state = GPIO_ReadInputDataBit(IR_GPIO_PORT_B, IR_PINB_1_DO); 
            break;
        default: 
            break;
    }
    return state;  // 返回值说明：0=检测到反射（有障碍）白，1=未检测到hei
}
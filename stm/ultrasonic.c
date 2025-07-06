#include "ultrasonic.h"
#include "stm32f10x.h"
#include "time.h"

void ultrasonic_init()
{
    // 1. 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 引脚配置   A11T 输出     A12E 输入
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置 GPIOA 的 GPIO_Pin_11 为输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 GPIOA 的 GPIO_Pin_12 为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

int ultrasonic_get_value()
{
    // 输入一个启动信号
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
    // 延时 10us 以上
    mydelay_us(30);  // 这里延时 30us，确保触发信号有效
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    // 读取长度
    while (0 == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12));  // 等待高电平到来（即 Echo 引脚变为高电平）
    int i = 0;
    while (1 == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12))  // 记录高电平持续时间
    {
        mydelay_us(1);  // 每次循环延时 1us
        i++;
    }
    return (i * 17) / 100; // 厘米换算：i(us)*340m/s/2 = i*0.017cm  // 微米（根据公式：距离 = 时间 * 声速 / 2，这里假设 mydelay_us(1) 实际为 1us）
}
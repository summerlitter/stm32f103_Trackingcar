#include "pwm.h"

void pwm_motor_init(void)
{
    // 1. 使能相关时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 2. 配置 PWM 输出引脚 PB8(TIM4_CH3), PB9(TIM4_CH4)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. 配置方向控制引脚 PB12、PB13、PB14、PB15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 普通推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 4. 配置定时器 TIM4，周期为 100ms（10Hz）
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;             // 自动重载值
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;          // 分频系数，系统时钟72MHz -> 10kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 5. 配置 PWM 通道3和4（PB8, PB9）
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                       // 初始占空比为0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 配置 CH3 (PB8)
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    //TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    // 配置 CH4 (PB9)
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    //TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    // 6. 启动定时器
    TIM_Cmd(TIM4, ENABLE);
    //TIM_CtrlPWMOutputs(TIM4, ENABLE);
}

void set_left_pwm(int value)
{
    if (value < 0) value = 0;
    if (value > 1000) value = 1000;
    TIM_SetCompare3(TIM4, value);  // CH3
}

void set_right_pwm(int value)
{
    if (value < 0) value = 0;
    if (value > 1000) value = 1000;
    TIM_SetCompare4(TIM4, value);  // CH4
}

void set_left_direction(uint8_t dir)
{
    if (dir) {
        GPIO_SetBits(GPIOB, GPIO_Pin_12);  // PB12 = 1
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); // PB13 = 0
    } else {
        GPIO_ResetBits(GPIOB, GPIO_Pin_12); // PB12 = 0
        GPIO_SetBits(GPIOB, GPIO_Pin_13);   // PB13 = 1
    }
}

void set_right_direction(uint8_t dir)
{
    if (dir) {
		GPIO_SetBits(GPIOB, GPIO_Pin_14);   // PB14 = 1
        GPIO_ResetBits(GPIOB, GPIO_Pin_15); // PB15 = 0
//        GPIO_ResetBits(GPIOB, GPIO_Pin_14); // PB14 = 0
//        GPIO_SetBits(GPIOB, GPIO_Pin_15);   // PB15 = 1
    } else {
		GPIO_ResetBits(GPIOB, GPIO_Pin_14); // PB14 = 0
        GPIO_SetBits(GPIOB, GPIO_Pin_15);   // PB15 = 1
//        GPIO_SetBits(GPIOB, GPIO_Pin_14);   // PB14 = 1
//        GPIO_ResetBits(GPIOB, GPIO_Pin_15); // PB15 = 0
    }
}
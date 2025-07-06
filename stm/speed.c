#include "stm32f10x.h"
#include "stdio.h"
#include "bluetooth.h"
#include "uart.h"

#define ENCODER_TIM_PERIOD 0xFFFF  // 编码器计数最大值（可调整）

// 全局变量保存当前计数值
volatile short left_encoder_count = 0;
volatile short right_encoder_count = 0;

/**************************************************************************
 * Function: 初始化 TIM2 为编码器接口模式
 * 使用 PA0 (TIM2_CH1) 作为 A 相，PA1 (TIM2_CH2) 作为 B 相
 *************************************************************************/
void Encoder_Init_TIM2_Left(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置 PA0 和 PA1 为浮空输入（编码器信号输入）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化 TIM2
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置编码器接口模式（通道1和2）
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 启用更新中断
    TIM_SetCounter(TIM2, 0);                   // 清零计数器
    TIM_Cmd(TIM2, ENABLE);                     // 启动定时器
}

/**************************************************************************
 * Function: 初始化 TIM3 为编码器接口模式
 * 使用 PA6 (TIM3_CH1) 和 PA7 (TIM3_CH2) 作为 A/B 相（右轮）
 *************************************************************************/
void Encoder_Init_TIM3_Right(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置 PA6 和 PA7 为浮空输入（编码器信号输入）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化 TIM3
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置编码器接口模式（通道1和2）
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

/**************************************************************************
 * Function: 读取指定定时器的编码器计数值并清零
 * Input   : TIMX - 定时器编号（2 或 3）
 * Output  : short - 当前速度值（带方向）
 *************************************************************************/
short Read_Encoder(uint8_t TIMX)
{
    short encoder_val = 0;

    switch (TIMX)
    {
        case 2:
            encoder_val = (short)(TIM2->CNT);
            TIM2->CNT = 0;
            break;
        case 3:
            encoder_val = (short)(TIM3->CNT);
            TIM3->CNT = 0;
            break;
        default:
            encoder_val = 0;
    }

    return encoder_val;
}

/**************************************************************************
 * Function: 读取并发送编码器速度
 * Format  : Left: %d, Right: %d\n
 *************************************************************************/
void Send_Encoder_Speeds(void)
{
    short left_speed = Read_Encoder(2);   // 左轮来自 TIM2
    short right_speed = Read_Encoder(3);  // 右轮来自 TIM3

    char buffer[100];

    // 构造简洁字符串格式
    sprintf(buffer, "Left: %d, Right: %d\n", left_speed, right_speed);
    uart2_wstring(buffer);
    
}
#include "time.h"
#include <assert.h>
#include "stm32f10x.h"

#define DELAY_TIMER               TIM2
#define TIMER_CLOCK_FREQ          1000000  // 1MHz (每微秒计数一次)
#define TIMER_MAX_COUNT           0xFFFF

// 定时器初始化（增加计数器清零）
void time_init(void) {
    // 1. 使能 TIM2 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 2. 配置定时器参数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct = {
        .TIM_Period = TIMER_MAX_COUNT,
        .TIM_Prescaler = (SystemCoreClock / TIMER_CLOCK_FREQ) - 1,
        .TIM_ClockDivision = TIM_CKD_DIV1,
        .TIM_CounterMode = TIM_CounterMode_Up
    };
    TIM_TimeBaseInit(DELAY_TIMER, &TIM_TimeBaseStruct);

    // 3. 清空计数器并启动
    TIM_SetCounter(DELAY_TIMER, 0); // 关键初始化步骤
    TIM_Cmd(DELAY_TIMER, ENABLE);
}

// 优化后的微秒级延时函数（支持任意时长）
void mydelay_us(uint32_t us) {
    assert_param(us <= (TIMER_MAX_COUNT * 1000)); // 防止不合理参数
    
    uint32_t start = DELAY_TIMER->CNT;
    uint32_t elapsed = 0;

    while (elapsed < us) {
        uint32_t current = DELAY_TIMER->CNT;
        
        // 处理溢出情况
        if (current < start) {
            elapsed += (TIMER_MAX_COUNT - start) + current + 1;
        } else {
            elapsed += current - start;
        }
        
        start = current; // 更新基准点
        
        // 防止极端情况下的无限循环
        if (elapsed > (TIMER_MAX_COUNT * 2)) break;
    }
}
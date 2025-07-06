
#include "stm32f10x.h"  // 根据你实际使用的芯片头文件调整
//#include "pwm.h"
#include "time.h"
#include "turn.h"
#include "pwm.h"
#include "infrarad.h"
// 前进
void go(void)
{
    set_left_direction(1);   // 左轮正转
    set_right_direction(1);	// 右轮正转
	set_left_pwm(75);
	set_right_pwm(75);
}

// 后退
void back(void)
{
    set_left_direction(0);   // 左轮反转
    set_right_direction(0);  // 右轮反转
	set_left_pwm(60);
	set_right_pwm(60);
}

// 左转：左轮不动，右轮正转
void left(void)
{
    set_left_direction(0);   // 左轮停止（或保持不动）
    set_right_direction(1);  // 右轮正转
    set_left_pwm(60);
	set_right_pwm(62);
}

// 右转：右轮不动，左轮正转
void right(void)
{
    set_left_direction(1);   // 左轮正转
    set_right_direction(0);  // 右轮停止（或保持不动）
    set_left_pwm(60);
	set_right_pwm(62);
}
//停止
void stop(void)
{
	set_left_pwm(0);
	set_right_pwm(0);
}
	
//模式转换
void exchange(void)
{
    uint8_t s1 = get_ir_state(1); // 传感器1
    uint8_t s2 = get_ir_state(2); // 传感器2
    uint8_t s3 = get_ir_state(3); // 传感器3

    if (s1 == 0 && s2 == 1 && s3 == 1) 
	{
		set_left_direction(1);   // 左轮正转
        set_right_direction(0);  // 右轮停止（或保持不动）
        set_left_pwm(60);
	    set_right_pwm(62);
		
	}
    if (s1 == 0 && s2 == 0 && s3 == 1) 
	{	
		set_left_direction(1);   // 左轮正转
        set_right_direction(0);  // 右轮停止（或保持不动）
        set_left_pwm(60);
	    set_right_pwm(62);
	}
    if (s1 == 1 && s2 == 0 && s3 == 1) 
	{
		go();
	}
    if (s1 == 1 && s2 == 0 && s3 == 0) 
	{
		set_left_direction(1);   // 左轮正转
        set_right_direction(0);  // 右轮停止（或保持不动）
        set_left_pwm(60);
	    set_right_pwm(62);
	}
    if (s1 == 1 && s2 == 1 && s3 == 0) 
	{
		set_left_direction(1);   // 左轮正转
        set_right_direction(0);  // 右轮停止（或保持不动）
        set_left_pwm(60);
	    set_right_pwm(62);
	}
}
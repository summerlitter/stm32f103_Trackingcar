#include "pwm.h"
#include "bluetooth.h"
#include "infrarad.h"
#include "ultrasonic.h"
#include "time.h"
#include "stm32f10x.h"
#include <string.h>
#include "turn.h"
#include "speed.h"

#define CMD_BUF_SIZE 64
#define BASE_SPEED      65    // 基础速度与turn.c一致
#define KP              35    // 比例系数
#define KD              20    // 微分系数
#define MAX_ADJUST      40    // 最大调整量

// 状态机定义
typedef enum {
    STATE_INIT,         // 初始化状态
    STATE_MANUAL,       // 手动模式
    STATE_TRACKING      // 循迹模式
} SystemState;

// 全局变量
static SystemState current_state = STATE_INIT;
static int prev_error = 0;
static uint32_t system_tick = 0;  // 自定义系统时基
static char order[64]={0};

/**************** 系统初始化函数 ****************/
void system_init(void) {
    // 基础模块初始化
    time_init();            // 必须最先初始化
    ultrasonic_init();      // 超声波传感器
    infrarad_init();        // 红外传感器
    pwm_motor_init();       // 电机驱动
    uart2_init();           // 蓝牙模块
    pwm_motor_init();
		uart_init();
    
//    // 设置电机初始方向
//    set_right_direction(1);
//		set_left_direction(1);
//		set_left_pwm(0);
//		set_right_pwm(0);
//		Encoder_Init_TIM2_Left();
//		Encoder_Init_TIM3_Right();
}

/**************** 手动模式处理函数 ****************/
void handle_manual_mode(char* cmd) {
    uart2_readline(cmd, 64);  // 读取一行命令
    // ✅ 使用 cmd[0] 判断是否有有效输入
    if (cmd[0] != '\0') {
        uart2_wstring(cmd);

        if (strcmp(cmd, "go") == 0) {
            go();
        } else if (strcmp(cmd, "back") == 0) {
            back();
        } else if (strcmp(cmd, "stop") == 0) {
            stop();
        } else if (strcmp(cmd, "left") == 0) {
            left();
        } else if (strcmp(cmd, "right") == 0) {
            right();
        } else if (strcmp(cmd, "exchange") == 0) {
						current_state = STATE_TRACKING;
				}
    }
		memset(cmd, 0, sizeof(cmd));  
}

void getorder(char* order){
		uart2_readline(order,64);
		if (order[0] != '\0') {
				//if (strcmp(order, "out") == 0) {
            current_state = STATE_MANUAL;
						stop();
						uart2_wstring(order);
        //}
		}
		memset(order, 0, sizeof(order));
}

/**************** 循迹模式处理函数 ****************///黑亮
void handle_tracking_mode(char* order) {
    // 正确传感器映射（根据infrarad.h定义）：
    // get_ir_state(1)=左传感器, get_ir_state(2)=中传感器, get_ir_state(3)=右传感器
    uint8_t s1 = get_ir_state(1); // 左：0=白线，1=黑线（取反后）
    uint8_t s2 = get_ir_state(2); // 中
    uint8_t s3 = get_ir_state(3); // 右

    // 状态判断与转向控制
    if(s2) { // 中间检测到黑线（正常路线）
        if(s1 && !s3) {      // 左黑右白 → 右转修正
					while(current_state == STATE_TRACKING){
            right();
						getorder(order);
            //mydelay_us(100000); // 持续100ms右转 ✅
						if(s2||(current_state = STATE_MANUAL))break;
					}
        } 
        else if(!s1 && s3) { // 左白右黑 → 左转修正
					while(current_state == STATE_TRACKING){
            left();
						getorder(order);
            //mydelay_us(100000); // 持续100ms右转 ✅
						if(s2||(current_state = STATE_MANUAL))break;
					}
        }
        else {               // 直行
            go();
						getorder(order);						
        }
    } 
    else {  // 中间未检测到黑线（偏离路线）
        if(s1) right();      // 左侧有黑线 → 右转
        else if(s3) left();  // 右侧有黑线 → 左转
        else go();           // 全白时保持前进
    }
    // 全白线停止策略
    static uint8_t lost_count = 0;
    if(!(s1||s2||s3)) {
        if(++lost_count > 10) stop(); // 持续10周期全白才停止
    } else {
        lost_count = 0;
    }
    // 调试输出
//    static uint32_t last_debug = 0;
//    if(system_tick - last_debug > 300) {
//        char buf[64];
//        sprintf(buf, "L:%d M:%d R:%d\r\n", s1, s2, s3);
//        uart2_wstring(buf);
//        last_debug = system_tick;
//    }
}

/**************** 主函数 ****************/
int main(void) {
    system_init();

    while(1) {
        switch(current_state) {
            case STATE_INIT:
                current_state = STATE_MANUAL;
                break;
                
            case STATE_MANUAL:
								char cmd[64]={0};
                handle_manual_mode(cmd);
								mydelay_us(1000000);  // 可选延时，防止 CPU 占用过高
                break;
                
            case STATE_TRACKING:									
                handle_tracking_mode(order);
								//uart2_wstring("success");
                break;
        }
        
        // 系统时基更新（每循环+50ms）
        system_tick += 50;
        mydelay_us(500);
    }
}
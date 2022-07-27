#ifndef __motor_task_h__
#define __motor_task_h__
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "sys.h"
//开始任务
extern TaskHandle_t motor_handler ; 				//定义开始任务句柄
void motor_task(void *pvParameters);//声明任务函数
#define motor_size  512   					//任务堆栈大小
#define motor_task_priority 8 			//任务优先级

void motor_task_create(void);


enum PID_Mode {
	is_No = 0,  //关闭所有操作
	is_Free,   //保留切换前的状态
	is_Line,   //循迹
	is_Turn,   //转弯
	is_Gyro   //自平衡
};

extern uint32_t const *TIM6_tick;

void pid_mode_switch(uint8_t target_mode);
void get_motor_speed(void);
#endif

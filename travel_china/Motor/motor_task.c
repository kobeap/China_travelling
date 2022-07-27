#include "motor_task.h"
#include  "encoder.h"
#include "motor.h"
#include "uart.h"
#include "speed_ctrl.h"
#include "pid.h"
#include "turn.h"
#include "scaner.h"
#include "bsp_linefollower.h"
#include "sin_generate.h"
TaskHandle_t motor_handler;
int dirct[4] = {-1,-1,1,1};  
volatile uint8_t PIDMode;
uint8_t line_gyro_switch = 0;
#define PI  3.1415926535

#define Speed_Bias_Up 10
#define Speed_Bias_Down 10


void motor_task(void *pvParameters){
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();   //获取系统节拍
	static uint8_t mouse = 0;    //小灯鼠
	while(1){
			get_motor_speed();
		
//			Turn_Angle(45);	
			motor_L0.target = motor_L1.target = motor_all.Lspeed;
			motor_R0.target = motor_R1.target = motor_all.Rspeed;
							mouse++;
					if(mouse>100){
						mouse =0;
//						LED_twinkle();
				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
			}
//			motor_L0.target = motor_L1.target = 200;
//			motor_R0.target = motor_R1.target = 200;
//			incremental_PID(&motor_L0, &motor_pid_param);
//			incremental_PID(&motor_L1, &motor_pid_param);
//			incremental_PID(&motor_R0, &motor_pid_param);
//			incremental_PID(&motor_R1, &motor_pid_param);
//			
//			motor_set_pwm(1, (int32_t)motor_L0.output);
//			motor_set_pwm(2, (int32_t)motor_L1.output);
//			motor_set_pwm(3, (int32_t)motor_R0.output);
//	  	    motor_set_pwm(4, (int32_t)motor_R1.output);
 
//		printf("%5d  %5d %5d %5d\r\n",(int)motor_L0.measure,(int)motor_L1.measure,(int)motor_R0.measure,(int)motor_R1.measure);
//		printf("%d,%d\r\n",(int)motor_R1.measure,(int)motor_R1.target);
//		printf("%5d  %5d %5d %5d\r\n",(int)direction[0],(int)high_time[1],(int)direction[2],(int)direction[3]);
		vTaskDelayUntil(&xLastWakeTime, (5/portTICK_RATE_MS));//绝对休眠5ms // INCLUDE_vTaskDelayUntil 1
	
   }
}

//PID任务创建
void motor_task_create(void){
	  xTaskCreate((TaskFunction_t ) motor_task,//任务函数
	               (const char *)"motor_task",	  //任务名字
								 (uint32_t) motor_size,    //任务堆栈大小
								 (void* )NULL,                  //传递给任务参数的指针参数
								 (UBaseType_t) motor_task_priority, //任务的优先级
								(TaskHandle_t *)&motor_handler ); //任务句柄
							 }
void pid_mode_switch(uint8_t target_mode)
{
	switch (target_mode)
	{
		case is_Turn: {
			gyroG_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
			TG_speed = (struct Gradual){0,0,0};
			line_pid_obj = (struct P_pid_obj){0,0,0,0,0,0,0};
			TC_speed = (struct Gradual){0,0,0};
			gyroT_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
			break;
		}
		
		case is_Line: {
			if (PIDMode == is_Gyro)  //从自平衡切换到循线
			{
				line_gyro_switch = 1;
			}
			else
			{
				gyroT_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
			}
			break;
		}
		
		case is_Gyro: {
			if (PIDMode == is_Line)  //从循线切换到自平衡
			{
				line_gyro_switch = 2;
			}
			else
			{
				gyroT_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
			}
			break;
		}
		
		case is_Free: {
			break;
		}
		
		case is_No: {
			line_pid_obj = (struct P_pid_obj){0,0,0,0,0,0,0};
			TC_speed = (struct Gradual){0,0,0};
			gyroG_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
			TG_speed = (struct Gradual){0,0,0};
			gyroT_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
			break;
		}
	}
	PIDMode = target_mode;
}

void get_motor_speed()
{
	    if(high_time[0]==0)
			motor_L0.measure=0;
		else
			motor_L0.measure = (float)(100000/high_time[0]) * dirct[0] * direction[0] ;
		if(high_time[1]==0)
			motor_L1.measure=0;
		else
			motor_L1.measure = (float)(100000/high_time[1]) * dirct[1] * direction[1] ;
		if(high_time[2]==0)
			motor_R0.measure=0;
		else
			motor_R0.measure = (float)(100000/high_time[2]) * dirct[2] * direction[2] ;
		if(high_time[3]==0)
			motor_R1.measure=0;
		else
			motor_R1.measure = (float)(100000/high_time[3]) * dirct[3] * direction[3] ;
}

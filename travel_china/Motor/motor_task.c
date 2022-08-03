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
#include "bsp_buzzer.h"
TaskHandle_t motor_handler;
int dirct[4] = {-1,-1,1,1};  
volatile uint8_t PIDMode;
uint8_t line_gyro_switch = 0;
#define PI  3.1415926535

#define Speed_Bias_Up 10
#define Speed_Bias_Down 10

int motor1;
void motor_task(void *pvParameters){
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();   //获取系统节拍
	static uint8_t mouse = 0;    //小灯鼠

	while(1){
			get_motor_speed();
		    motor_all.encoder_avg = (float)((float)(pulse_num[0]*dirct[0] + pulse_out[0]*MAX_pulse*dirct[0]) + (float)(pulse_num[1]*dirct[1] + pulse_out[1]*MAX_pulse*dirct[1]) + (float)(pulse_num[2]*dirct[2] + pulse_out[2]*MAX_pulse*dirct[2]) + (float)(pulse_num[3]*dirct[3] + pulse_out[3]*MAX_pulse*dirct[3])) / 4.0f;
			//轮径6.8cm
//			printf("num=%d,out=%d\r\n",pulse_num[0],pulse_out[0]);
//			printf("%f\r\n",motor_all.encoder_avg);
			motor_all.Distance =((motor_all.encoder_avg * 6.8f * PI)/(572.0f));//转换为1米
//            printf("pid=%d\r\n",PIDMode);//
		
//			printf("%f\r\n",imu.pitch);
		//陀螺仪自平衡->循迹
			if (line_gyro_switch == 1)    //这里的line_gyro_switch是在PIDMODE切换情况下所产生的标志位
			{
				line_pid_obj = gyroG_pid;
				TC_speed = TG_speed;
				gyroG_pid = (struct P_pid_obj){0,0,0,0,0,0,0};
				TG_speed = (struct Gradual){0,0,0};
				line_gyro_switch = 0;
			}
			//循迹->陀螺仪自平衡
			else if (line_gyro_switch == 2)
			{
				gyroG_pid = line_pid_obj;
				TG_speed = TC_speed;
				line_pid_obj = (struct P_pid_obj){0,0,0,0,0,0,0};
				TC_speed = (struct Gradual){0,0,0};
				line_gyro_switch = 0;
			}
			else
			{
				if (PIDMode == is_Line)
				{
					getline_error();
					gradual_cal(&TC_speed, motor_all.Cspeed, motor_all.Cincrement);
					Go_Line(TC_speed.Now);
				}
				else
					motor_all.Cspeed = 0;
				
				//转弯PID控制
				if (PIDMode == is_Turn)	
				{
					if (Turn_Angle(angle.AngleT))
					{
						gyroT_pid = (struct P_pid_obj){0,0,0,0,0,0};
					}
				}

				//自平衡PID控制
				if (PIDMode == is_Gyro)
				{ 
					gradual_cal(&TG_speed, motor_all.Gspeed, motor_all.Gincrement);	
					runWithAngle(angle.AngleG, TG_speed.Now);
				}
				else
					motor_all.Gspeed = 0;
			}
			
			
//    		runWithAngle(0,300);	
			motor_L0.target = motor_L1.target = motor_all.Lspeed;
			motor_R0.target = motor_R1.target = motor_all.Rspeed;
			mouse++;
			if(mouse>100){
						mouse =0;
//						LED_twinkle();
				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
			}


			incremental_PID(&motor_L0, &motor_pid_paramL0);
			incremental_PID(&motor_L1, &motor_pid_paramL1);
			incremental_PID(&motor_R0, &motor_pid_paramR0);
			incremental_PID(&motor_R1, &motor_pid_paramR1);
			
			
//			TIM1->CCR2 = 0; TIM1->CCR4 = 5000;
//			TIM1->CCR1 = 0; TIM1->CCR3 = 5000;
//			TIM2->CCR2 = 0; TIM2->CCR4 = 5000;
//			TIM2->CCR3 = 0; TIM2->CCR1 = 5000;
			motor_set_pwm(1, (int32_t)motor_L0.output);
			motor_set_pwm(2, (int32_t)motor_L1.output);
			motor_set_pwm(3, (int32_t)motor_R0.output);
	  	    motor_set_pwm(4, (int32_t)motor_R1.output);

//		printf("%d,%d,%d,%d\r\n",(int)motor_L0.measure,(int)motor_L1.measure,(int)motor_R0.measure,(int)motor_R1.measure);
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

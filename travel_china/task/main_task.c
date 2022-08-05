#include "main_task.h"
#include "rudder_control.h"
#include "uart.h"
#include "imu_task.h"
#include "uart.h"
#include "turn.h"
#include "map.h"
#include "barrier.h"
#include "bsp_linefollower.h"
#include "scaner.h"
#include "speed_ctrl.h"
#include "encoder.h"
#include "barrier.h"
#include "motor_task.h"
void main_task(void *pvParameters){
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();   //获取系统节拍
//平台的下车代码
	{
		runWithAngle(nodesr.nowNode.angle,300);
			while(imu.pitch>Down_pitch)
			{
				vTaskDelay(2);
			}//下桥
			while(imu.pitch<After_down)
			{
				vTaskDelay(2);
			}//下桥完毕
				//路程记录清零
			encoder_clear();
			pid_mode_switch(is_Line); 
	        motor_all.Cspeed = 500;
      }
//	Barrier_Bridge(0,0);
	while(1){
//		Barrier_Bridge(0,0);
//		getline_error();
//		Go_Line(500);
//        vTaskDelay(5);

	
	    Cross();	
	vTaskDelayUntil(&xLastWakeTime, (5/portTICK_RATE_MS));//绝对休眠5ms // INCLUDE_vTaskDelayUntil 1

	}
}

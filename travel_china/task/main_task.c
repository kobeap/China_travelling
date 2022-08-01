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
void main_task(void *pvParameters){
	static int i=0;
	while(1){
//		Barrier_Bridge(0,0);
//		getline_error();
//		Go_Line(500);
//        vTaskDelay(5);
	
		vTaskDelay(5);
//		Barrier_Bridge(0,0);
//    	Sword_Mountain();
//		printf("pitch=%f\r\n",imu.pitch);
		
//		 printf("roll=%f,pitch=%f,yaw=%f \r\n",imu.roll,(imu.pitch-imu.compensatePitch),getAngleZ());
//		 vTaskDelay(10);
////		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9))
////		{
////			printf("ok1");
////		}
////		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13))
////		{
////			printf("ok2");
////		}
////		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15))
////		{
////			printf("ok3");
////		}
////		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8))
////		{
////			printf("ok4");
////		}
////		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14))
////		{
////			printf("ok5");
////		}
////		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10))
////		{
////			printf("ok6");
////		}
	}
}

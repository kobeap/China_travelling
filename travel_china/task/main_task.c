#include "main_task.h"
#include "rudder_control.h"
#include "uart.h"
#include "imu_task.h"
#include "uart.h"
void main_task(void *pvParameters){
	while(1){
//		 Rudder_control(2000,0);

//		 printf("111");
//		 printf("roll=%f,pitch=%f,yaw=%f \r\n",imu.roll,imu.pitch,imu.yaw);
		 vTaskDelay(10);
//		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9))
//		{
//			printf("ok1");
//		}
//		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13))
//		{
//			printf("ok2");
//		}
//		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15))
//		{
//			printf("ok3");
//		}
//		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8))
//		{
//			printf("ok4");
//		}
//		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14))
//		{
//			printf("ok5");
//		}
//		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10))
//		{
//			printf("ok6");
//		}
	}
}

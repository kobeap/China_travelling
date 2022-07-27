#include "main_task.h"
#include "rudder_control.h"
#include "uart.h"
#include "imu_task.h"
void main_task(void *pvParameters){
	while(1){
//		 Rudder_control(2000,0);

//		 printf("111");
		 printf("roll=%f,pitch=%f,yaw=%f \r\n",imu.roll,imu.pitch,imu.yaw);
		 vTaskDelay(10);
	}
}

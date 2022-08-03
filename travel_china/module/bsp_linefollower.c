#include "bsp_linefollower.h"
#include "usart.h"
#include "tim.h"
#include "scaner.h"
#include "stdio.h"
#include "main.h"
#include  "FreeRTOS.h"
#include  "task.h"
//此文件用于红外走
#define FRAME_HEAD 0XAC
#define RECEIVE_LEN 5

volatile struct Infrared_Sensor infrared;

void show_line(SCANER *gray);
void infrared_open()
{
	//内左
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13))
		{
			infrared.inside_left=1;
		}
		else{
			infrared.inside_left=0;
		}
	//内右
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15))
		{
			infrared.inside_right=1;
		}
		else{
			infrared.inside_right=0;
		}
		//外右
		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9))
		{
			infrared.outside_right=1;
		}else{
			infrared.outside_right=0;
		}
		//外左
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14))
		{
			infrared.outside_left=1;
		}else{
			infrared.outside_left=0;
		}
}


void show_line(SCANER *gray)
{
	printf("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\r\n", 
		(gray->detail&0X01), (gray->detail&0X02)>>1, (gray->detail&0X04)>>2, (gray->detail&0X08)>>3, 
		(gray->detail&0X10)>>4, (gray->detail&0X20)>>5, (gray->detail&0X40)>>6, (gray->detail&0X80)>>7,
		(gray->detail&0X100)>>8, (gray->detail&0X200)>>9, (gray->detail&0X400)>>10, (gray->detail&0X800)>>11,
		(gray->detail&0X1000)>>12, (gray->detail&0X2000)>>13, (gray->detail&0X4000)>>14, (gray->detail&0X8000)>>15);
}









#include "bsp_linefollower.h"
#include "usart.h"
#include "tim.h"
#include "scaner.h"
#include "stdio.h"
#include "main.h"
#include  "FreeRTOS.h"
#include  "task.h"
//此文件用于红外走


volatile struct Infrared_Sensor infrared;


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










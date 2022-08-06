#ifndef __BSP_LINEFOLLOWER_H
#define __BSP_LINEFOLLOWER_H

#include "sys.h"

#define LFB_SENSOR_NUM 16   //ѭ���崫��������
#define LFB_HALF_SENSOR_NUM 8

//��ǰ�ĺ���
#define Infrared_ahead  (uint8_t)!HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)



struct Infrared_Sensor    //���⴫����
{
	uint8_t outside_left;
	uint8_t outside_right;
	uint8_t inside_left;
	uint8_t inside_right;
};


extern volatile struct Infrared_Sensor infrared;


void LFB_send(void);
void infrared_open(void);

#endif


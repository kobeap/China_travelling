#ifndef __Encoder_h__
#define __Encoder_h__
#include "main.h"
#include "tim.h"
#define FORWARD 1
#define BACKWARD -1
//����С�����Ӷ�Ӧ�����벶��ʱ��
typedef struct{
	TIM_HandleTypeDef TIM;
	GPIO_TypeDef* GPIO_2;//B��
  uint32_t GPIO_PORT_2;		
}wheel;
extern wheel wheel_1,wheel_2,wheel_3,wheel_4;
/*
wheel_1     wheel_2
wheel_3     wheel_4
*/
extern int pulse_num[4];
extern uint32_t high_time[4];        			//�ߵ�ƽʱ��
extern int  direction[4];                //����תʶ��
void Encoder_init(void); 
void Encoder_task(void *pvParameters);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

extern uint8_t dog[4];   //�����

#endif

#ifndef __motor_task_h__
#define __motor_task_h__
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "sys.h"
//��ʼ����
extern TaskHandle_t motor_handler ; 				//���忪ʼ������
void motor_task(void *pvParameters);//����������
#define motor_size  512   					//�����ջ��С
#define motor_task_priority 8 			//�������ȼ�

void motor_task_create(void);


enum PID_Mode {
	is_No = 0,  //�ر����в���
	is_Free,   //�����л�ǰ��״̬
	is_Line,   //ѭ��
	is_Turn,   //ת��
	is_Gyro   //��ƽ��
};

extern uint32_t const *TIM6_tick;

void pid_mode_switch(uint8_t target_mode);
void get_motor_speed(void);
#endif

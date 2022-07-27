#ifndef __task_create_h__
#define __task_create_h__
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

//��ʼ����
extern TaskHandle_t Start_handler ; //���忪ʼ������
void Start_task(void *pvParameters);//����������
#define Start_size  128   //�����ջ��С
#define Start_task_priority 32  //�������ȼ�


//��������
extern TaskHandle_t main_handler ; //��������������
void main_task(void *pvParameters);//����������
#define main_size  128  //�����ջ��С
#define main_task_priority 10  //�������ȼ�

////ѭ��������
//extern TaskHandle_t track_handler ; //����������������
//void track_task(void *pvParameters);//����������
//#define track_size  256  //�����ջ��С
//#define track_task_priority 8  //�������ȼ�

////PID����
//extern TaskHandle_t motor_handler ; //����������������
//void motor_task(void *pvParameters);//����������
//#define motor_size  128  //�����ջ��С
//#define motor_task_priority 8  //�������ȼ�

void Start_task_create(void);
void main_task_create(void);
//void track_task_create(void);
//void motor_task_create(void);
#endif

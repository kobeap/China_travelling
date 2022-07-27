#include "task_create.h"
TaskHandle_t Start_handler; //���忪ʼ������
TaskHandle_t PWM_TASK_handler; //����PWM������
TaskHandle_t main_handler; //��������������
//TaskHandle_t motor_handler; //����PID������
//TaskHandle_t track_handler; //����ѭ����������
/*
��ʼ���񴴽�����
*/
void Start_task_create(void){
	  xTaskCreate((TaskFunction_t ) Start_task,//������
	               (const char *)"Start_task",	  //��������
								 (uint32_t) Start_size,    //�����ջ��С
								 (void* )NULL,                  //���ݸ����������ָ�����
								 (UBaseType_t) Start_task_priority, //��������ȼ�
								(TaskHandle_t *)&Start_handler ); //������
}


//�������񴴽�
void main_task_create(void){
	  xTaskCreate((TaskFunction_t ) main_task,//������
	               (const char *)"main_task",	  //��������
								 (uint32_t) main_size,    //�����ջ��С
								 (void* )NULL,                  //���ݸ����������ָ�����
								 (UBaseType_t) main_task_priority, //��������ȼ�
								(TaskHandle_t *)&main_handler ); //������
}
////ѭ�������񴴽�
//void track_task_create(void){
//	  xTaskCreate((TaskFunction_t ) track_task,//������
//	               (const char *)"track_task",	  //��������
//								 (uint32_t) track_size,    //�����ջ��С
//								 (void* )NULL,                  //���ݸ����������ָ�����
//								 (UBaseType_t) track_task_priority, //��������ȼ�
//								(TaskHandle_t *)&track_handler ); //������
//							 }
////PID���񴴽�
//void motor_task_create(void){
//	  xTaskCreate((TaskFunction_t ) motor_task,//������
//	               (const char *)"motor_task",	  //��������
//								 (uint32_t) motor_size,    //�����ջ��С
//								 (void* )NULL,                  //���ݸ����������ָ�����
//								 (UBaseType_t) motor_task_priority, //��������ȼ�
//								(TaskHandle_t *)&motor_handler ); //������
//							 }

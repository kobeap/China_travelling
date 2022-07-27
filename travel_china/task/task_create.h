#ifndef __task_create_h__
#define __task_create_h__
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

//开始任务
extern TaskHandle_t Start_handler ; //定义开始任务句柄
void Start_task(void *pvParameters);//声明任务函数
#define Start_size  128   //任务堆栈大小
#define Start_task_priority 32  //任务优先级


//主控任务
extern TaskHandle_t main_handler ; //定义主控任务句柄
void main_task(void *pvParameters);//声明任务函数
#define main_size  128  //任务堆栈大小
#define main_task_priority 10  //任务优先级

////循迹板任务
//extern TaskHandle_t track_handler ; //定义陀螺仪任务句柄
//void track_task(void *pvParameters);//声明任务函数
//#define track_size  256  //任务堆栈大小
//#define track_task_priority 8  //任务优先级

////PID任务
//extern TaskHandle_t motor_handler ; //定义陀螺仪任务句柄
//void motor_task(void *pvParameters);//声明任务函数
//#define motor_size  128  //任务堆栈大小
//#define motor_task_priority 8  //任务优先级

void Start_task_create(void);
void main_task_create(void);
//void track_task_create(void);
//void motor_task_create(void);
#endif

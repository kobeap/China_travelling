#include "bsp_linefollower.h"
#include "usart.h"
#include "tim.h"
#include "scaner.h"
#include "stdio.h"
//此文件用于红外走
#define FRAME_HEAD 0XAC
#define RECEIVE_LEN 5

volatile struct Infrared_Sensor infrared;

void show_line(SCANER *gray);



//int left[8]={0};
//int right[8]={0};
//#define GPIO_Left1 GPIOD
//#define GPIO_Left1_P GPIO_PIN_0
//#define GPIO_Left2 GPIOD
//#define GPIO_Left2_P GPIO_PIN_1
//#define GPIO_Left3 GPIOD
//#define GPIO_Left3_P GPIO_PIN_2
//#define GPIO_Left4 GPIOD
//#define GPIO_Left4_P GPIO_PIN_3
//#define GPIO_Left5 GPIOD
//#define GPIO_Left5_P GPIO_PIN_4
//#define GPIO_Left6 GPIOD
//#define GPIO_Left6_P GPIO_PIN_7
//#define GPIO_Left7 GPIOB
//#define GPIO_Left7_P GPIO_PIN_3
//#define GPIO_Left8 GPIOB
//#define GPIO_Left8_P GPIO_PIN_4

//#define GPIO_Right1 GPIOB
//#define GPIO_Right1_P GPIO_PIN_5
//#define GPIO_Right2 GPIOB
//#define GPIO_Right2_P GPIO_PIN_6
//#define GPIO_Right3 GPIOB
//#define GPIO_Right3_P GPIO_PIN_7
//#define GPIO_Right4 GPIOE
//#define GPIO_Right4_P GPIO_PIN_2
//#define GPIO_Right5 GPIOE
//#define GPIO_Right5_P GPIO_PIN_3
//#define GPIO_Right6 GPIOE
//#define GPIO_Right6_P GPIO_PIN_4
//#define GPIO_Right7 GPIOA
//#define GPIO_Right7_P GPIO_PIN_5
//#define GPIO_Right8 GPIOE
//#define GPIO_Right8_P GPIO_PIN_6

//float line_weightleft[8] = {-3.1,-2.4,-1.8,-1.3,-0.9,-0.6,-0.4,-0.3};
//float line_weightright[8]={3.1,2.4,1.8,1.3,0.9,0.6,0.4,0.3};
//float line_mea=0;//循迹值
//void line_measure()
//{
//	if(HAL_GPIO_ReadPin(GPIO_Left1,GPIO_Left1_P))
//	{
//		left[0]=1;
//		printf("1");
//	}
//	else{
//		left[0]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Left2,GPIO_Left2_P))
//	{
//		left[1]=1;
//		printf("2");
//	}
//	else{
//		left[1]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Left3,GPIO_Left3_P))
//	{
//		left[2]=1;
//		printf("3");
//	}
//	else{
//		left[2]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Left4,GPIO_Left4_P))
//	{
//		left[3]=1;
//		printf("4");
//	}
//	else{
//		left[3]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Left5,GPIO_Left5_P))
//	{
//		left[4]=1;
//		printf("5");
//	}
//	else{
//		left[4]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Left6,GPIO_Left6_P))
//	{
//		left[5]=1;
//		printf("6");
//	}
//	else{
//		left[5]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Left7,GPIO_Left7_P))
//	{
//		left[6]=1;
//		printf("7");
//	}
//	else{
//		left[6]=0;
//	}
//		if(HAL_GPIO_ReadPin(GPIO_Left8,GPIO_Left8_P))
//	{
//		left[7]=1;
//		printf("8");
//	}
//	else{
//		left[7]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right1,GPIO_Right1_P))
//	{
//		right[0]=1;
//		printf("0");
//	}
//	else{
//		right[0]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right2,GPIO_Right2_P))
//	{
//		right[1]=1;
//		printf("1");
//	}
//	else{
//		right[1]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right3,GPIO_Right3_P))
//	{
//		right[2]=1;
//		printf("2");
//	}
//	else{
//		right[2]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right4,GPIO_Right4_P))
//	{
//		right[3]=1;
//		printf("3");
//	}
//	else{
//		right[3]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right5,GPIO_Right5_P))
//	{
//		right[4]=1;
//		printf("4");
//	}
//	else{
//		right[4]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right6,GPIO_Right6_P))
//	{
//		right[5]=1;
//		printf("5");
//	}
//	else{
//		right[5]=0;
//	}
//	if(HAL_GPIO_ReadPin(GPIO_Right7,GPIO_Right7_P))
//	{
//		right[6]=1;
//		printf("6");
//	}
//	else{
//		right[6]=0;
//	}
//		if(HAL_GPIO_ReadPin(GPIO_Right8,GPIO_Right8_P))
//	{
//		right[7]=1;
//		printf("7");
//	}
//	else{
//		right[7]=0;
//	}
//	for(int i=0;i<8;i++)
//	{
//		line_mea=left[i]*line_weightleft[i]+right[i]*line_weightright[i];
//	}
//}
//void UART4_IRQHandler(void)
//{
//	uint8_t Res;
//	static uint8_t RxBuffer[20];
//	static uint8_t data_cnt;
//	static uint8_t state;
//	register uint8_t sum = 0, i = 0;
//	
//	if ((__HAL_UART_GET_FLAG(&huart4,UART_FLAG_RXNE)!=RESET))
//	{
//		Res = huart4.Instance->DR;
//		
//		if(state == 1)
//		{
//			RxBuffer[data_cnt++] = Res;
//			if (data_cnt >= RECEIVE_LEN)
//			{
//				for (i=0; i<RECEIVE_LEN-1; i++)
//					sum += RxBuffer[i];
//				
//				if (sum == RxBuffer[RECEIVE_LEN-1])
//				{
//					__HAL_UART_DISABLE_IT(&huart4, UART_IT_RXNE);  //关闭接收中断
//					
//					Scaner.detail = (RxBuffer[2])|(RxBuffer[1]<<8);

//					infrared.outside_left = RxBuffer[3]&0X01;
//					infrared.outside_right = (RxBuffer[3]&0X02)>>1;
//					infrared.inside_left = (RxBuffer[3]&0X04)>>2;
//					infrared.inside_right = (RxBuffer[3]&0X08)>>3;
//					
//					powCal(&Scaner, LFB_SENSOR_NUM, scaner_set.EdgeIgnore);
//					//show_line(&Scaner);    //检查循迹灯的返回值
//					__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);  //开启接收中断
//				}
//				state = 0;
//			}
//		}
//		else if (state==0 && Res==FRAME_HEAD)
//		{
//			state=1;
//			data_cnt = 0;
//			RxBuffer[data_cnt++] = Res;
//		}
//		else
//			state=0;
//	}
//}


void show_line(SCANER *gray)
{
	printf("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\r\n", 
		(gray->detail&0X01), (gray->detail&0X02)>>1, (gray->detail&0X04)>>2, (gray->detail&0X08)>>3, 
		(gray->detail&0X10)>>4, (gray->detail&0X20)>>5, (gray->detail&0X40)>>6, (gray->detail&0X80)>>7,
		(gray->detail&0X100)>>8, (gray->detail&0X200)>>9, (gray->detail&0X400)>>10, (gray->detail&0X800)>>11,
		(gray->detail&0X1000)>>12, (gray->detail&0X2000)>>13, (gray->detail&0X4000)>>14, (gray->detail&0X8000)>>15);
}









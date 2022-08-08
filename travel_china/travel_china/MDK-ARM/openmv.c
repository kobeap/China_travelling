#include "openmv.h"

UART_HandleTypeDef mv;//UART句柄
uint8_t color;
void mv_init(uint32_t bound)
{	
	//UART 初始化设置
	mv.Instance=UART7;					  
	mv.Init.BaudRate=bound;				    //波特率
	mv.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	mv.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	mv.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	mv.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	mv.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&mv);					    //HAL_UART_Init()会使能UART3
}
void open_mv()
{
	__HAL_UART_ENABLE_IT(&mv, UART_IT_RXNE);
}
void close_mv()
{
	__HAL_UART_DISABLE_IT(&mv,UART_IT_RXNE);
}
void UART7_IRQHandler(void)
{
	static int temp=0;
	static uint8_t flag=0;
	if(__HAL_UART_GET_FLAG(&mv,UART_FLAG_RXNE)!=0){
		temp=mv.Instance->RDR;
		if(flag==2)
		{
			color=temp;
			flag=0;
		}
		if(temp==0xff&flag==1)
		{
			flag=2;
		}
		if(temp==0xff&flag==0)
		{
			flag=1;
		}
	}
	mv.Instance->ISR = 0;   //清除SR标志位
}

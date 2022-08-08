#include "openmv.h"

UART_HandleTypeDef mv;//UART���
uint8_t color;
void mv_init(uint32_t bound)
{	
	//UART ��ʼ������
	mv.Instance=UART7;					  
	mv.Init.BaudRate=bound;				    //������
	mv.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	mv.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	mv.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	mv.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	mv.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&mv);					    //HAL_UART_Init()��ʹ��UART3
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
	mv.Instance->ISR = 0;   //���SR��־λ
}

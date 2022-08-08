#include "Rudder_control.h"
#include "uart.h"
//#include  "uart.h"
// iic
void Rudder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	IIC_Init();//IIC��ʼ��
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);//OEʹ��
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	Rudder_WriteOneByte(0x0,0x1);//��Ӧ0x70ͨ��iic��ַ���ҹر�sleepģʽ
//	����Ƶ��
	uint8_t prescale,oldmode,newmode;
	oldmode = Rudder_ReadOneByte(0x0);
	newmode = (oldmode&0x7F) | 0x10; // sleep
	Rudder_WriteOneByte(0x0, newmode); // go to sleep
    Rudder_WriteOneByte(PRE_SCALE, 132); // set the prescaler
	Rudder_WriteOneByte(0x0,0x1);//��Ӧ0x70ͨ��iic��ַ���ҹر�sleepģʽ
	
//	int temp =1;
//	temp = Rudder_ReadOneByte(PRE_SCALE);
//	printf("%d\r\n",temp);
}

//��ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void Rudder_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
	IIC_Start();  	
	IIC_Send_Byte(Rudder<<1);    //����������ַ0x55,д���� 	 
	IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr);   //����Ŀ���ַ
	IIC_Wait_Ack();	
	IIC_Send_Byte(DataToWrite);//�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}
//ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
uint8_t Rudder_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(Rudder<<1);   //����������ַ0X70,д���� 	   
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);   //����Ŀ���ַ
	IIC_Wait_Ack();	
	IIC_Start();  	 	   
	IIC_Send_Byte((Rudder<<1)+1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();					//����һ��ֹͣ����	    
	return temp;
}
/*****************************************************************************
��������  Rudder_control()
�������ܣ��������
�βΣ� aim---Ŀ��Ƕ�    ID�����id
��ע��0�ȶ�Ӧ350    270�ȶ�Ӧ2100  
*******************************************************************************/
void Rudder_control(uint16_t aim,uint8_t id){
	//printf("%d,%d",(int)aim,(int)id);
	if(id==0){
		Rudder_WriteOneByte(LED0_ON_L,0x0);
		Rudder_WriteOneByte(LED0_ON_H,0x0);
		
		Rudder_WriteOneByte(LED0_OFF_L,(uint8_t)(aim&0xFF));
		Rudder_WriteOneByte(LED0_OFF_H,(uint8_t)(aim>>8));
	}
	if(id==1){
		Rudder_WriteOneByte(LED1_ON_L,0x0);
		Rudder_WriteOneByte(LED1_ON_H,0x0);
		
		Rudder_WriteOneByte(LED1_OFF_L,(uint8_t)(aim&0xFF));
		Rudder_WriteOneByte(LED1_OFF_H,(uint8_t)(aim>>8));
	}
	if(id==2){
		Rudder_WriteOneByte(LED2_ON_L,0x0);
		Rudder_WriteOneByte(LED2_ON_H,0x0);
		
		Rudder_WriteOneByte(LED2_OFF_L,(uint8_t)(aim&0xFF));
		Rudder_WriteOneByte(LED2_OFF_H,(uint8_t)(aim>>8));
	}
	if(id==3){
		Rudder_WriteOneByte(LED3_ON_L,0x0);
		Rudder_WriteOneByte(LED3_ON_H,0x0);
		
		Rudder_WriteOneByte(LED3_OFF_L,(uint8_t)(aim&0xFF));
		Rudder_WriteOneByte(LED3_OFF_H,(uint8_t)(aim>>8));
	}
	if(id==4){
		Rudder_WriteOneByte(LED4_ON_L,0x0);
		Rudder_WriteOneByte(LED4_ON_H,0x0);
		
		Rudder_WriteOneByte(LED4_OFF_L,(uint8_t)(aim&0xFF));
		Rudder_WriteOneByte(LED4_OFF_H,(uint8_t)(aim>>8));
	}
}

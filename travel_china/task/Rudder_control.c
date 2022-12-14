#include "Rudder_control.h"
#include "uart.h"
//#include  "uart.h"
// iic
void Rudder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	IIC_Init();//IIC初始化
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);//OE使能
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	Rudder_WriteOneByte(0x0,0x1);//响应0x70通用iic地址并且关闭sleep模式
//	设置频率
	uint8_t prescale,oldmode,newmode;
	oldmode = Rudder_ReadOneByte(0x0);
	newmode = (oldmode&0x7F) | 0x10; // sleep
	Rudder_WriteOneByte(0x0, newmode); // go to sleep
    Rudder_WriteOneByte(PRE_SCALE, 132); // set the prescaler
	Rudder_WriteOneByte(0x0,0x1);//响应0x70通用iic地址并且关闭sleep模式
	
//	int temp =1;
//	temp = Rudder_ReadOneByte(PRE_SCALE);
//	printf("%d\r\n",temp);
}

//在指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void Rudder_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
	IIC_Start();  	
	IIC_Send_Byte(Rudder<<1);    //发送器件地址0x55,写数据 	 
	IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr);   //发送目标地址
	IIC_Wait_Ack();	
	IIC_Send_Byte(DataToWrite);//发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}
//指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
uint8_t Rudder_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
	IIC_Start();  
	IIC_Send_Byte(Rudder<<1);   //发送器件地址0X70,写数据 	   
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);   //发送目标地址
	IIC_Wait_Ack();	
	IIC_Start();  	 	   
	IIC_Send_Byte((Rudder<<1)+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();					//产生一个停止条件	    
	return temp;
}
/*****************************************************************************
函数名：  Rudder_control()
函数功能：舵机控制
形参： aim---目标角度    ID：舵机id
备注：0度对应350    270度对应2100  
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

#ifndef __IIC_H__
#define __IIC_H__
#include "main.h"
#include "delay.h"
//B0 B2
#define IIC_SDA_OUT  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
#define IIC_SDA_DOWN  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
#define READ_SDA      HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_13)

#define IIC_SCK_OUT  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
#define IIC_SCK_DOWN  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);

void SDA(uint8_t param);
void IIC_Init(void);
void SDA(uint8_t param);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
#endif

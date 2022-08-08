#include "scaner.h"
#include "map.h"
#include "math.h"
#include "turn.h"
#include "stdio.h"
#include "pid.h"
#include "speed_ctrl.h"
#include "bsp_linefollower.h"
#include "motor.h"

#define LINE_SPEED_MAX 50
#define Speed_Compensate   5
#define BLACK 0								//循黑线
#define WHLITE 1							//循白线
#define Lamp_Max 16   //循迹灯最大数

const float line_weight[16] = {-3.1,-2.4,-1.8,-1.3,-0.9,-0.6,-0.4,-0.3,0.3,0.4,0.6,0.9,1.3,1.8,2.4,3.1};//0.3

struct Scaner_Set scaner_set = {0,0};

//float CatchsensorNum = 0;	//寻迹时的目标传感器位置，一般情况下为0，微微会用到
SCANER Scaner;


#define Line_color WHLITE
//循迹板 1234578 87654321

void Go_Line(float speed){
	float Fspeed;				//经过PID运算后的结果
					
	line_pid_obj.measure = Scaner.error;  							//当前循迹板所在的位置，从左到右-7到0到0到7
	line_pid_obj.target = scaner_set.CatchsensorNum;		//目标
	
	Fspeed = positional_PID(&line_pid_obj, &line_pid_param); //进行位置PID运算
	
	if (Fspeed>=LINE_SPEED_MAX) 
		Fspeed = LINE_SPEED_MAX;
	else if (Fspeed<=-LINE_SPEED_MAX) 
		Fspeed = -LINE_SPEED_MAX;
	
	Fspeed *= fabsf(speed)/50;
	
	motor_all.Lspeed = speed-Fspeed;
	motor_all.Rspeed = speed+Fspeed;
}
uint8_t getline_error()//获得更新不同巡线模式下的误差值
{
	get_detail();//获取巡线值
	if(Line_Scan(&Scaner, Lamp_Max, scaner_set.EdgeIgnore))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void get_detail()
{
	uint32_t data = 0;
	//遇白亮灯返回0
		//左边的灯
	//从左往右   1 2 3 4 5 6 7 8
	if(Line_color){
	data = 0xffff;//白线
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left1,GPIO_Left1_P)<<15);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left2,GPIO_Left2_P)<<14);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left3,GPIO_Left3_P)<<13);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left4,GPIO_Left4_P)<<12);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left5,GPIO_Left5_P)<<11);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left6,GPIO_Left6_P)<<10);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left7,GPIO_Left7_P)<<9);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left8,GPIO_Left8_P)<<8);
	//右边
	//从右到左   8 7 6 5 4 3 2 1
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right1,GPIO_Right1_P)<<0);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right2,GPIO_Right2_P)<<1);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right3,GPIO_Right3_P)<<2);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right4,GPIO_Right4_P)<<3);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right5,GPIO_Right5_P)<<4);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right6,GPIO_Right6_P)<<5);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right7,GPIO_Right7_P)<<6);
	data^=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right8,GPIO_Right8_P)<<7);
	}
	else{
	data = 0x0;//黑线
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left1,GPIO_Left1_P)<<15);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left2,GPIO_Left2_P)<<14);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left3,GPIO_Left3_P)<<13);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left4,GPIO_Left4_P)<<12);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left5,GPIO_Left5_P)<<11);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left6,GPIO_Left6_P)<<10);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left7,GPIO_Left7_P)<<9);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Left8,GPIO_Left8_P)<<8);
	//右边
	//从右到左   8 7 6 5 4 3 2 1
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right1,GPIO_Right1_P)<<0);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right2,GPIO_Right2_P)<<1);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right3,GPIO_Right3_P)<<2);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right4,GPIO_Right4_P)<<3);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right5,GPIO_Right5_P)<<4);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right6,GPIO_Right6_P)<<5);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right7,GPIO_Right7_P)<<6);
	data|=((uint16_t)HAL_GPIO_ReadPin(GPIO_Right8,GPIO_Right8_P)<<7);
	}
	Scaner.detail = data;
}
//循迹扫描
uint8_t Line_Scan(SCANER *scaner, unsigned char sensorNum, int8_t edge_ignore)
{
	float error = 0;
	u8 linenum=0;//记录线的数目
	u8 lednum=0;
	int8_t lednum_tmp = 0;
							//获得二进制巡线值
	for(uint8_t i=0;i<sensorNum;i++) 		//从小车方向从左往右数亮灯数和引导线数
	{								//linenum用来记录有多少条线，line用来记录第几条线。
		if((scaner->detail&(0x1<<i))) 
		{
			lednum++;
			if(!(scaner->detail&(1<<(i+1)))) 
				++linenum;			//先读取亮灯数和引导线数，检测到从1变为0认为一条线
		}
	}
	scaner->lineNum = linenum;			
	scaner->ledNum=lednum;
////		for (uint8_t i=0; i<sensorNum; i++)
////		{
////			lednum_tmp += (scaner->detail>>(sensorNum-i-1))&0X01;//记录点灯数
////			error += ((scaner->detail>>(sensorNum-i-1))&0X01) * line_weight[i];
////		}
	if ((nodesr.nowNode.flag & LEFT_LINE) == LEFT_LINE)  //左循线    
	{
		for (uint8_t i=0; i<sensorNum; i++)
		{
			lednum_tmp += (scaner->detail>>(sensorNum-i-1))&0X01;//记录点灯数
			error += ((scaner->detail>>(sensorNum-i-1))&0X01) * line_weight[i];
			if ((scaner->detail>>(sensorNum-i-1)) & 0X01)		//如果是白线
				if (!((scaner->detail>>((sensorNum-i-2)))&0x01))		//下一个灯不是白
					break;		//退出
		}
	}
	else if ((nodesr.nowNode.flag & RIGHT_LINE) == RIGHT_LINE)  //右循线
	{
		for (uint8_t i=0; i<sensorNum; i++)
		{
			lednum_tmp += (scaner->detail>>i)&0X01;
			error += ((scaner->detail>>i)&0X01) * line_weight[sensorNum-i-1];
			if ((scaner->detail>>i) & 0X01)
				if (!((scaner->detail>>(i+1))&0x01))
					break;
		}
	}
	else//后面再完善
	{
		for(uint8_t i= edge_ignore; i<sensorNum - edge_ignore; i++) 
		{
				lednum_tmp += (scaner->detail>>(sensorNum-1-i))&0X01;
				error += ((scaner->detail>>(sensorNum-1-i))&0X01) * line_weight[i];
		}	
	}
	if(lednum==0)
	{
		error=0;
	}
//	if(lednum_tmp >= 7 ||scaner->ledNum>=7)//目标灯数过多
//	{    
//			return 1;   //换陀螺仪
//	}
//	else if(lednum_tmp==0)//无灯
//	{  
//			return 1;		//换陀螺仪
//	}	
	else
	{
		error/=(float)lednum_tmp;		//取平均
	}
	Scaner.error = error;
	return 0;
}


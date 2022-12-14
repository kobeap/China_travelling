#include "barrier.h"
#include "sys.h"
#include "delay.h"
#include "speed_ctrl.h"
#include "motor.h"
#include "pid.h"
#include "imu_task.h"
#include "scaner.h"
#include "turn.h"
#include "map.h"
#include "motor_task.h"
#include "pid.h"
#include "math.h"
#include "bsp_buzzer.h"
#include "bsp_led.h"
#include "stdio.h"
#include "motor_task.h"
#include "QR.h"
#include "motion.h"
#include "bsp_linefollower.h"



#define BACK_SPEED  -300
#define BACK_SPEED1 -100
#define GOSPEED 10

extern int BW_num[];
int value;//openmv接口
uint8_t special_arrive=0;
//防抖动系列函数
//1._shake在平台识别过程中，防止因一些意外因素导致车提前停止,同时完成撞板的目的
//2._Rshake用于判断其是否经过波浪板
//3._pshake识别处于下坡状态
void Anti_shake(int Uneed_time)
{
	uint32_t i = 0;  //循环变量设置成unit32_t避免for循环最后的奇怪指令
	for(i = 0; i < Uneed_time ; i++)
	{
		
		while(Infrared_ahead == 1)
		{
			vTaskDelay(1);
		}
	}
}

int AI_shake(int Uneed_time)
{
//	uint32_t i = 0, cnt = 0;  //循环变量设置成unit32_t避免for循环最后的奇怪指令
//	for(i = 0; i < Uneed_time ; i++)
//	{
//		HAL_Delay(1);
//		if(AI == 1)
//		{
//			cnt++;
//		}
//	}
//	if(cnt >= Uneed_time*0.5)  return 1;
	return 0;
}

//void Anti_encoder(int Uneed_time)
//{
//	uint32_t i = 0;  //循环变量设置成unit32_t避免for循环最后的奇怪指令
//	for(i = 0; i < Uneed_time ; i++)
//	{
//		HAL_Delay(1);
//		while(read_encoder(2) == 0);
//	}
//}

//void Anti_Rshake()
//{
//	uint32_t i = 0;
//	uint32_t cnt = 0;
//	while(i < 148)
//	{
//		for(i = 0; i < 150 ; i++)
//		{
//			HAL_Delay(1);
//			if(fabs(imu.roll) > 2) cnt++;
//			if(cnt == 30)
//			{
//				i = 0;
//				cnt = 0;
//				break;
//			}
//			//while(fabs(imu.roll) > 2);
//		}
//	}
//}

int Anti_Pshake()
{
	int sum = 0;
	for(uint32_t i = 0; i < 5; i++)
	{
		HAL_Delay(1);
		sum += imu.pitch;
	}
	if(sum/5 > 3) return 1;
	else return 0;
}

/************************************************/

void Stage()		//flag==1时取绝度角度，flag==0时取相对角度
{
	float num = 0; //distan = 0;
	

	
	if (nodesr.nowNode.nodenum == P2)
	{
		Stage_P2();
		return;
	}
	if ((Scaner.detail & 0X0180) == 0X0180) //如果在最中间位置
	{
		angle.AngleG = getAngleZ();//自平衡走的角度
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 300;
	}
	else{
		angle.AngleG = nodesr.nowNode.angle;//自平衡走的角度
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 300;
	}
    while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//开始上平台

	while(imu.pitch>After_up)
	{
		vTaskDelay(5);
	}//上完桥

	while(Infrared_ahead == 0)
	{
		vTaskDelay(5);
	}  //撞挡板

	num = motor_all.Distance;				
	while(motor_all.Distance - num < 30)//前进一段距离
	{
		vTaskDelay(5);
	}
	
	//delay_ms(300);
	CarBrake();
	
	//HAL_Delay(1000);
	mpuZreset(imu.yaw, nodesr.nowNode.angle);  //陀螺仪校正
	num = motor_all.Distance;				//后退一段距离
	pid_mode_switch(is_No);
//	motor_L0.target = motor_L1.target = BACK_SPEED;
//	motor_R0.target = motor_R1.target = BACK_SPEED;
	motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;
	
	while (num-motor_all.Distance < 15)
	{
		vTaskDelay(5);
	}
	CarBrake();
	
	buzzer_on();
	vTaskDelay(500);
	buzzer_off();

	motor_pid_clear();

	Turn_Angle_Relative(179);
	while (fabs(angle.AngleT-getAngleZ())>2)
	{
		vTaskDelay(5);
	}
	motor_pid_clear();
    pid_mode_switch(is_Gyro);
	motor_all.Gspeed=300;
	angle.AngleG=nodesr.nowNode.angle-179;
	while(imu.pitch>Down_pitch)
	{
		vTaskDelay(2);
	}
	motor_pid_clear();
	pid_mode_switch(is_Line);
	motor_all.Cspeed=300;
	nodesr.nowNode.function=0;	//清除障碍标志
	nodesr.flag|=0x04;	//到达路口
}
//特殊结点
void Special_Node()
{
	
	if(((nodesr.nowNode.flag&DRIGHT)==DRIGHT)&((nodesr.nowNode.flag&CRIGHT)==CRIGHT)&(nodesr.nowNode.nodenum==N5))//N4-N5右循迹 左循迹 边缘忽略
	{
		nodesr.nowNode.flag&=(~RIGHT_LINE);//取消右循迹标志位
		nodesr.nowNode.flag|=LEFT_LINE;//左循迹
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}//右分岔
		nodesr.nowNode.flag&=(~CRIGHT);//取消右分岔标志位
	    while(deal_arrive()!=1)//右半边天
		{
			vTaskDelay(2);
			scaner_set.EdgeIgnore=6;
			special_arrive=1;
		}
	}
	else if((nodesr.nowNode.nodenum==N4)&((nodesr.nowNode.flag&CRIGHT)==CRIGHT))//N5-N4
	{
		float num=0;
		nodesr.nowNode.flag&=(~RIGHT_LINE);
		nodesr.nowNode.flag|=LEFT_LINE;//左循迹
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
		num=motor_all.Distance;
		while(motor_all.Distance-num<10)//第一个左分岔路口再走10厘米
		{
			vTaskDelay(2);
		}
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
	}
	else if((nodesr.nowNode.nodenum==N4)&((nodesr.nowNode.flag&CLEFT)==CLEFT))//N3-N4
	{
		float num=0;
		nodesr.nowNode.flag&=(~LEFT_LINE);
		nodesr.nowNode.flag|=RIGHT_LINE;//右循迹
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
		num=motor_all.Distance;
		while(motor_all.Distance-num<10)//第一个左分岔路口再走10厘米
		{
			vTaskDelay(2);
		}
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
	}
	else if(((nodesr.nowNode.nodenum==N13)&((nodesr.nowNode.flag&CLEFT)==CLEFT))&(((nodesr.nowNode.flag&CRIGHT)==CRIGHT)&((nodesr.nowNode.flag&DLEFT)==DLEFT))&((nodesr.nowNode.flag&DRIGHT)==DRIGHT))
/*P6-N13*/	{
		angle.AngleG=getAngleZ();
		motor_all.Gspeed=300;
		pid_mode_switch(is_Gyro);
	}
	else
	{
		angle.AngleG=getAngleZ();
		motor_all.Gspeed=600;
		pid_mode_switch(is_Gyro);
	}
//	if(((nodesr.nowNode.flag&CRIGHT)==CRIGHT)&((nodesr.nowNode.flag&CLEFT)==CLEFT))//N5-N6  P4-N6先左循迹后右循迹 
//	{
//		angle.AngleT=getAngleZ();
//		pid_mode_switch(is_Gyro);
////		nodesr.nowNode.flag|=LEFT_LINE;//左循迹
////		while(deal_arrive()!=1)
////		{
////			vTaskDelay(2);
////		}//检测到右分岔
////		nodesr.nowNode.flag&=(~CRIGHT);//取消右分岔标志位
////		while(deal_arrive()!=1)
////		{
////			vTaskDelay(2);
////		}//检测到左分岔
////		nodesr.nowNode.flag&=(~LEFT_LINE);//取消左循迹
////		nodesr.nowNode.flag|=RIGHT_LINE;//右循迹
////		special_arrive=1;
//	}
}

//平台二的动作
void Stage_P2()		//flag==1时取绝度角度，flag==0时取相对角度
{
	//float num = 0;
	while(imu.pitch > Up_pitch); //上斜坡时认为上了平台
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=40;
	pid_mode_switch(is_Gyro);//上平台后用陀螺仪
	while (Infrared_ahead == 0);//红外亮

	CarBrake();
	
	Turn_Angle_Relative(182);		//转>=180度，
	while (fabs(angle.AngleT - getAngleZ())>5);//矫正角度
	motor_pid_clear();
	nodesr.nowNode.function=0;	//清除障碍标志
	nodesr.flag|=0x04;	//到达路口
}

/***************************************************
过长桥
************************************************/

void Barrier_Bridge(float step,float speed)	//过长桥
{
	float num=0;
	num=motor_all.Distance;
	motor_all.Gspeed = 300;    //自平衡速度
	if ((Scaner.detail & 0X0180) == 0X0180) //如果在最中间位置
	{
		angle.AngleG = getAngleZ();//自平衡走的角度
		pid_mode_switch(is_Gyro);
	}
	else{
		angle.AngleG = nodesr.nowNode.angle;//自平衡走的角度
		pid_mode_switch(is_Gyro);
	}
	struct PID_param origin_param = gyroG_pid_param;
	gyroG_pid_param.kp = 4.8;//原来3.7
	
	while(imu.pitch <= Up_pitch)	//还在平地,出循环就是上桥中
	{
		vTaskDelay(2);
	}
	//is_Up = false;
    while(imu.pitch>Up_pitch)
	{
		vTaskDelay(2);
	}//上桥完毕
	motor_all.Gspeed = 500;   //原来65   90
	while(imu.pitch > Down_pitch)          
	{
		infrared_open();
		if ((infrared.inside_left == 1 && infrared.inside_right == 0))
		{
			angle.AngleG = getAngleZ() + 2;
            buzzer_on();
		}			
		else if ((infrared.inside_right == 1 && infrared.inside_left == 0))
		{		angle.AngleG = getAngleZ() - 2; 
				buzzer_on();
		}
		else
		{
			angle.AngleG = getAngleZ();//正确的角度
			buzzer_off();
		}
		if(infrared.outside_right==0)
		{
			angle.AngleG = getAngleZ() + 3;
            buzzer_on();
		}
		else if(infrared.outside_left==0)
		{
				angle.AngleG = getAngleZ() - 3; 
				buzzer_on();
		}
		else{
			angle.AngleG = getAngleZ();//正确的角度
			buzzer_off();
		}
		if (motor_all.Distance-num < 200)
			motor_all.Gspeed = 500;     //原来是80  90
		else
			motor_all.Gspeed = 500;      //原来是65   75
		vTaskDelay(2);
	}
//	while(imu.pitch<Down_pitch)
//	{
//		angle.AngleG = nodesr.nowNode.angle;
//		motor_all.Cspeed = 300;
//		vTaskDelay(2);
//	}//下桥中
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);	
	}//出循环下完在平地
	pid_mode_switch(is_Line);
		//路程记录清零
	motor_all.Distance = 0;
	motor_all.encoder_avg = 0;
	nodesr.nowNode.function = 0;
	nodesr.flag |= 0X04;  //到达路口
	gyroG_pid_param=origin_param;
}

//过楼梯
void Barrier_Hill(uint8_t order)  //楼梯数量
{
	angle.AngleG = getAngleZ();
	struct PID_param origin_param = line_pid_param;
	motor_all.Cspeed = 300;   //原来是400
	pid_mode_switch(is_Line);
	while ( imu.pitch >10+basic_p )	
	{
		vTaskDelay(2);
	}//开始上楼梯
	while(imu.pitch>-10+basic_p)
	{
		vTaskDelay(2);
	}//开始下楼梯
    while(imu.pitch<basic_p-3)
	{
		vTaskDelay(2);
	}//下完楼梯

	line_pid_param = origin_param;
	nodesr.nowNode.function=0;//清除障碍标志
	nodesr.flag|=0x04;	//到达路口
}


//刀山
void Sword_Mountain()
{
	float num;
	struct PID_param origin_param = line_pid_param;
	struct PID_param origin_param1 = gyroG_pid_param;
	num = motor_all.Distance;
	motor_all.Cspeed = 300; 
	line_pid_param.kp = 50;
	
	
	//mpuZreset(imu.yaw, nodesr.nowNode.angle);  //陀螺仪校正
	while(motor_all.Distance - num < 25)//强矫正循迹位置
	{
		vTaskDelay(2);
	}
	angle.AngleG = getAngleZ();
	
	gyroG_pid_param.kp = 2.4;   //拉大刀山的自平衡kp
	motor_all.Gspeed = 200;
	pid_mode_switch(is_Gyro);

	while(imu.pitch < After_up)//出循环上刀山
	{
		vTaskDelay(2);
	}
	buzzer_on();
	

	while(imu.pitch > After_down)//出循环下刀山
	{
		vTaskDelay(2);
	}
	gyroG_pid_param = origin_param1;
	line_pid_param = origin_param;
	buzzer_off();
	
	 
	nodesr.nowNode.function=0;//清除障碍标志
	nodesr.flag|=0x04;	//到达路口
}

/*****************************************************************************
上珠峰，包含下珠峰
*****************************************************************************/
void Barrier_HighMountain()
{
	float origin_turnM=motor_all.GyroT_speedMax;
	float num = 0;
	struct PID_param origin_param = line_pid_param;
	struct PID_param origin_param1=gyroG_pid_param;
	line_pid_param.kp = 25;
	line_pid_param.kd = 15;
	gyroG_pid_param.kp=10;
	motor_all.Cspeed = 400; //最开始还没上坡的速度 90
	motor_all.Cincrement = 20;
	pid_mode_switch(is_Line);
	while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//刚上桥
    num=motor_all.Distance;
	motor_all.Cincrement = 20; //上坡的速度  2
	motor_all.Cspeed = 400;//80
	while(motor_all.Distance-num<90)//上坡走循迹
	{
		vTaskDelay(2);
	}
	motor_all.Gspeed = 400; //第一个平地自平衡	
	angle.AngleG=getAngleZ();
    pid_mode_switch(is_Gyro);
    buzzer_on();
	while(imu.pitch>After_up)
	{
		vTaskDelay(2);
	}
	while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//第二个坡
	buzzer_off();
	
	
	motor_all.Cspeed = 400; 
	motor_all.Cincrement = 20;
	pid_mode_switch(is_Line);
	num=motor_all.Distance;
	while(motor_all.Distance-num<90)//上坡走循迹
	{
		vTaskDelay(2);
	}
	motor_all.Gspeed = 400; //第二次自平衡	
	angle.AngleG=getAngleZ();
    pid_mode_switch(is_Gyro);
	buzzer_on();
    while(imu.pitch>After_up)
	{
		vTaskDelay(2);
	}//第二次上平台
	buzzer_off();
	while(Infrared_ahead == 0)
	{
		vTaskDelay(5);
	}  //撞挡板
	num = motor_all.Distance;				
	while(motor_all.Distance - num < 30)//前进一段距离
	{
		vTaskDelay(5);
	}
	CarBrake();
	mpuZreset(imu.yaw, nodesr.nowNode.angle);  //陀螺仪校正
   	
	
	num = motor_all.Distance;
	pid_mode_switch(is_No);
	motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED1;
	while(num-motor_all.Distance < 15)
	{
		vTaskDelay(2);
	}
	CarBrake();
	motor_all.GyroT_speedMax=300;
	Turn_Angle_Relative(179);
	while(fabs(angle.AngleT - getAngleZ())>2) //判断误差
	{
		vTaskDelay(2);
	}
	vTaskDelay(300);
	Turn_Angle360();
	 
	motor_pid_clear();
	pid_mode_switch(is_No);
	

    
    Barrier_Down_HighMountain(400);

	line_pid_param = origin_param;
	gyroG_pid_param=origin_param1;
    motor_all.GyroT_speedMax=origin_turnM;
	nodesr.nowNode.function=0;//清除障碍标志
	nodesr.flag|=0x04;	//到达路口
}

/*****************************************************************************
函数功能：下珠峰，在转完180后调用
*****************************************************************************/
void Barrier_Down_HighMountain(float speed)
{
	float num=0;
	pid_mode_switch(is_Gyro);
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=speed;
	while(imu.pitch>Down_pitch)
	{
		vTaskDelay(2);
	}//下坡
	pid_mode_switch(is_Line);
	motor_all.Cspeed=speed;
	num=motor_all.Distance;
	while(motor_all.Distance-num<80)
	{
		vTaskDelay(2);
	}
	pid_mode_switch(is_Gyro);
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=speed;
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);
	}//第一个平地
	while(Scaner.ledNum<=12)
	{
		getline_error();
		vTaskDelay(2);
	}//照到红线
	while(Scaner.ledNum>4)
	{
		getline_error();
		vTaskDelay(2);
	}//照到白线了
	pid_mode_switch(is_Turn);
	if((Scaner.detail&0xF000)>0)//最左边随意4个灯亮
	{
		angle.AngleT=getAngleZ()+10;
		buzzer_on();
	}
	if((Scaner.detail&0xF)>0)//最左边随意4个灯亮
	{
		angle.AngleT=getAngleZ()-10;
		buzzer_on();
	}
	pid_mode_switch(is_Line);
	motor_all.Cspeed=speed;
	num=motor_all.Distance;
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);
	}//在地面
	buzzer_off();
}


void view()//打景点	
{	
	float num = 0;
	while(Infrared_ahead == 0);		//撞挡板
	delay_ms(200);
	num=motor_all.Distance;
	pid_mode_switch(is_No);
	
	motor_L0.target = motor_L1.target = BACK_SPEED;
	motor_R0.target = motor_R1.target = BACK_SPEED;
	
	while(num-motor_all.Distance<3);
	CarBrake();
	delay_ms(100);
	Turn_Angle_Relative(181);
	while(fabs(angle.AngleT-getAngleZ())>5);
	motor_pid_clear();
	motor_all.Cspeed=30;
	pid_mode_switch(is_Line);
	delay_ms(200);
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}

void view1()//打景点	
{	
	pid_mode_switch(is_Line);
	while(Infrared_ahead == 0);		//撞挡板
	delay_ms(100);
	//mpuZreset(gyro.yaw,nodesr.nowNode.angle-10);
	CarBrake();
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}

void back()
{
	pid_mode_switch(is_No);
	
	motor_L0.target = motor_L1.target = BACK_SPEED;
	motor_R0.target = motor_R1.target = BACK_SPEED;
	
	while(infrared.outside_left == 0 && infrared.outside_right == 0);
	CarBrake();
	//转绝对角度
	angle.AngleT = nodesr.nextNode.angle;
	pid_mode_switch(is_Turn);
	while(fabs(angle.AngleT - getAngleZ())>5);
	
	motor_pid_clear();
	pid_mode_switch(is_Line);
	//motor_all.Cspeed=15;
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}
/*****************************************************************************
过波浪板
速度   ，  波浪板长度（cm）
长度主要是防止误判提前进入波浪板，然后提前放下造成严重失误。
先走这一段长度，保证能进入波浪板，且避免提前误判成已经走去波浪板。
*****************************************************************************/
void Barrier_WavedPlate(float lenght)//波浪板长度
{
//	buzzer_on();
	float num = 0;
	motor_all.Cspeed = 200;   
	pid_mode_switch(is_Line);
	scaner_set.EdgeIgnore=4;
	num = motor_all.Distance;
	while( motor_all.Distance-num <120)
	{
		getline_error();
		vTaskDelay(5);
	}
//    buzzer_off();	
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}


void South_Pole(float length)
{
	float num = 0;
	struct PID_param origin_param = line_pid_param;
	struct PID_param origin_param1=gyroT_pid_param;
	motor_all.Cspeed = 300;
	pid_mode_switch(is_Line);
	line_pid_param.kp = 25;
	line_pid_param.kd = 15;
	gyroT_pid_param.kp=10;
	while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//上坡
	
	motor_all.Cspeed = 350;
	num = motor_all.Distance;
	while(motor_all.Distance - num < 90)
	{
		vTaskDelay(2);
	}		
	
	angle.AngleG = getAngleZ();
	pid_mode_switch(is_Gyro);
	motor_all.Gspeed = 300;

	while(Infrared_ahead == 0)
	{
		vTaskDelay(5);
	}  //撞挡板

	num = motor_all.Distance;				
	while(motor_all.Distance - num < 30)//前进一段距离
	{
		vTaskDelay(5);
	}
	

	CarBrake();
	
	//myAction(); //机器人动作+寻宝检测
	
	mpuZreset(imu.yaw, nodesr.nowNode.angle);  //陀螺仪校正
	
	num = motor_all.Distance;
	pid_mode_switch(is_No);

	motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED1;

	while(num - motor_all.Distance < 13)
	{
		vTaskDelay(2);
	}
	CarBrake();
	

	Turn_Angle_Relative(179);
	while(fabs(angle.AngleT - getAngleZ())>3)
	{
		vTaskDelay(2);
	}	
	motor_pid_clear();
	
	motor_all.Cspeed = 300;
	pid_mode_switch(is_Line);
	while(imu.pitch>Down_pitch)
	{
		vTaskDelay(2);
	}
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);
	}	
	line_pid_param = origin_param;  //恢复原来的PID参数
	gyroT_pid_param=origin_param1;
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}

#if 0
void QQB_1(void)
{
	float num = 0;
	CatchsensorNum=-3.5;			//改变循迹中心点，在判断路口函数改回来
	PIDMode=Catch;
	motor_all.Cspeed=20;
	while(Scaner.detail!=0);	//循迹板上跷跷板
	PIDMode=Free;					
	L0speed=20;L1speed=20;R0speed=20;R1speed=20;
	while(imu.pitch>-15);		//检测到大角度，车子上跷跷板
	delay_ms(500);
	num=motor_all.Distance;
	PIDMode=GYRO;				//等车子走上跷跷板再走自平衡
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=15;
	while(motor_all.Distance-num<700);			//读编码器走固定距离
	CarBrake();					//停止
	while(imu.pitch<15);		//等待跷跷板向下
	delay_ms(600);				//等待跷跷板停下
	PIDMode=Free;
	L0speed=5;L1speed=5;R0speed=30;R1speed=30;
	delay_ms(200);
	CarBrake();	
	if(Scaner.detail==0)
	{
		Turn_Angle(30,30);			//逆时针转40°
		while(fabs(Turn_Angle_Targe-getAngleZ())>5);
	}
	PIDMode=Catch;				//继续循迹
	motor_all.Cspeed=20;
	nodesr.nowNode.function=0;
}
#endif

/*void QQB_1(void)
{
	float num;
	struct PID_param origin_param = line_pid_param;
	
	num = motor_all.Distance;
	
	motor_pid_clear();
	pid_mode_switch(is_Line);
	motor_all.Cspeed = 30;  //给低速进入寻线 60
	
	scaner_set.CatchsensorNum = line_weight[7];   //给予左边权值
	line_pid_param.kp = 45;    //拉大KP 45
	
	
	if(nodesr.nowNode.nodenum == B9)
	{	
		while(fabs(motor_all.Distance - num) < 28);//适当调整
		pid_mode_switch(is_No);
		angle.AngleT = -92;//-89
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
		
		angle.AngleG = -92;//-89
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 20;
		while(imu.pitch > Up_pitch);//还在平地
		motor_all.Gspeed = 20;
		while(imu.pitch <= Up_pitch );
		
//		//start开环
//		while(imu.pitch > -1);//等待上坡
//		buzzer_on();
//		HAL_Delay(200);
//		buzzer_off();
//		motor_all.Lspeed = 20;
//		motor_all.Rspeed = 25; 
//		//end闭环
		
		while(imu.pitch <= -3);
		CarBrake();
		while(imu.pitch <= Down_pitch);
		angle.AngleT = -25;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
		
	}
	else if(nodesr.nowNode.nodenum == B8)
	{	
		while(fabs(motor_all.Distance - num) <32);
		pid_mode_switch(is_No);
		angle.AngleT = 90;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
		
		angle.AngleG = 90;
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 30;
		while(imu.pitch > Up_pitch);//还在平地
		motor_all.Gspeed = 30;
		while(imu.pitch <= Up_pitch );
		
//		//start开环
//		while(imu.pitch > -1);//等待上坡
//		motor_all.Lspeed = 18;
//		motor_all.Rspeed = 25; 
//		while(imu.pitch < -16);
//		//end闭环
		
		CarBrake();
		angle.AngleT = -25;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
	}
	
	
//	pid_mode_switch(is_Line);
//	motor_all.Cspeed = 20;
	scaner_set.CatchsensorNum = 0;
	line_pid_param = origin_param;  //恢复原来的PID参数
	nodesr.nowNode.function=0;
	nodesr.flag|=0X04;	//到达路口
	
}*/

void QQB_1(void)
{
	float num;
	struct PID_param origin_param = line_pid_param;
	
	num = motor_all.Distance;
	
	motor_pid_clear();
	pid_mode_switch(is_Line);
	motor_all.Cspeed = 60;  //给低速进入寻线
	
	scaner_set.CatchsensorNum = line_weight[6];    //给予左边权值
	line_pid_param.kp = 45;    //拉大KP 
	
	if(nodesr.nowNode.nodenum == B9)
	{	
		while(infrared.outside_right == 0);
		CarBrake();
		
		buzzer_on();
		
		//转去跷跷板的正向
		angle.AngleT = -90;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
		/*angle.AngleT = 89;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);*/
		
		angle.AngleG = -90;
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 20;
		while(imu.pitch > Up_pitch);//还在平地
		while(imu.pitch <= Up_pitch )
		{
			buzzer_off();
			if (infrared.outside_left == 1)
				angle.AngleG -= 4; 
			else if (infrared.outside_right == 1)
				angle.AngleG += 4; 
			else
				angle.AngleG = nodesr.nowNode.angle;
		}
		while(imu.pitch <= -2);
		HAL_Delay(800);
		buzzer_off();
		CarBrake();
		while(imu.pitch <= Down_pitch);
		motor_all.Gspeed = 20;
		HAL_Delay(800);
		angle.AngleT = -55;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
		//while(imu.pitch <= Down_pitch);
		
		/*angle.AngleT = 25;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);*/
		
	}
	else if(nodesr.nowNode.nodenum == B8)
	{	
		while(fabs(motor_all.Distance - num) <68);
		angle.AngleT = 90;
		pid_mode_switch(is_Turn);
		while(fabs(angle.AngleT - getAngleZ())>5);
		angle.AngleG = 90;
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 30;
		while(imu.pitch > Up_pitch);//还在平地
		motor_all.Gspeed = 30;
		while(imu.pitch <= Up_pitch );
		CarBrake();
//		angle.AngleT = -25;
//		pid_mode_switch(is_Turn);
//		while(fabs(angle.AngleT - getAngleZ())>5);
	}
	
	
//	pid_mode_switch(is_Line);
//	pid_mode_switch(is_Gyro);
//	num = motor_all.Distance;
//	motor_all.Gspeed = 10;
//	while(fabs(motor_all.Distance - num) < 5);
	scaner_set.CatchsensorNum = line_weight[8];    //给予左边权值
	line_pid_param.kp = 45;    //拉大KP 
	motor_all.Cspeed = 60;  //给低速进入寻线
	pid_mode_switch(is_Line);
	num = motor_all.Distance;
	while(fabs(motor_all.Distance - num) < 40);
	line_pid_param = origin_param;  //恢复原来的PID参数
	scaner_set.CatchsensorNum = 0;
	nodesr.nowNode.function=0;
	nodesr.flag|=0X04;	//到达路口
	
}

void door()
{
	float num = 0;
	//	buzzer_on();
	static u8 flag=0;
	motor_all.Cspeed = 60;
	pid_mode_switch(is_Line);
	if(flag==2)	
	{
		route_reset(flag);
		flag = 3;
		return;
	}
	else if(flag==4)
	{
		route_reset(flag);
		return;
	}
	while(1)
	{		//走一米来检测全白停止线
		if(Scaner.ledNum>=8)
		{
			buzzer_on();
			CarBrake();
			// 识别红绿灯的时间
			HAL_Delay(400);    
			if(value == 1)
			{
				buzzer_off();
				if(flag==0)	//第一个路口的交通指示牌是红牌，还不能把路线连接起来
				{
					map.point -= 2;
					route[map.point] = N8;
					pid_mode_switch(is_No);			//后退
	
					//motor_L0.target = motor_L1.target = BACK_SPEED;
					//motor_R0.target = motor_R1.target = BACK_SPEED;
					motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;
	
					num = motor_all.Distance;
					while(num-motor_all.Distance<20);
					Turn_Angle_Relative(181);	//	转到当前结点方向
					while(fabs(angle.AngleT-getAngleZ())>3);
					nodesr.nowNode	= Node[getNextConnectNode(N10,N3)];		//重新设置nowNode
					nodesr.nowNode.step = 45;
					nodesr.nowNode.flag|=STOPTURN;
					nodesr.nowNode.speed=120;
					pid_mode_switch(is_Line);
					nodesr.flag|=0x20;
					flag=1;
					return ;
				}
				else if(flag==1)//第二个也是红色，还不能把路线连接起来
				{		
					map.point-=2;		
					route[map.point]=N4;
					pid_mode_switch(is_No);			//后退

					motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;

					num=motor_all.Distance;
					while(num-motor_all.Distance<20);
					Turn_Angle_Relative(181);	//	转到当前结点方向
					while(fabs(angle.AngleT-getAngleZ())>3);
					nodesr.nowNode	= Node[getNextConnectNode(N8,N3)];		//重新设置nowNode
					nodesr.nowNode.step = 30;
					nodesr.nowNode.speed = 120;
					nodesr.nowNode.flag|=STOPTURN;
					pid_mode_switch(is_Line);
					nodesr.flag|=0x20;				
					flag=2;
					return ;
				}		
				else if(flag==3)
				{
					map.point-=2;		
					route[map.point]=N8;
					pid_mode_switch(is_No);	 //后退
	
					//motor_L0.target = motor_L1.target = BACK_SPEED;
					//motor_R0.target = motor_R1.target = BACK_SPEED;
					motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;
	
					num=motor_all.Distance;
					while(num-motor_all.Distance<20);
					Turn_Angle_Relative(181);	//	转到当前结点方向
					while(fabs(angle.AngleT-getAngleZ())>3);
					nodesr.nowNode	= Node[getNextConnectNode(N12,N5)];		//重新设置nowNode
					nodesr.nowNode.step=48;
					nodesr.nowNode.speed=120;
					nodesr.nowNode.flag|=STOPTURN;
					pid_mode_switch(is_Line);
					nodesr.flag|=0x20;				
					flag=4;
					return ;
				}
			}	
			else
			{
				// buzzer_off();
				motor_all.Cspeed=60;
				pid_mode_switch(is_Line);
				nodesr.nowNode.function=1;
				HAL_Delay(1000);
				break;
			}
		}
		else 
		{
			motor_all.Cspeed=60;
			pid_mode_switch(is_Line);
		}				
	}

	if(flag != 3)   route_reset(flag);//路线重新设置

}


void route_reset(u8 flag)
{
	u8 temp=0,i=0;
	temp = map.point - 1;
	if(flag==0)//第一个门开着
	{			
		while(1)
		{	
			route[temp++]=door1route[i++];		//路线连接
			if(door1route[i]==255)
			{
				route[temp]=door1route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N3,N10)];		//重新设置nowNode
				nodesr.nextNode = Node[getNextConnectNode(N10,route[map.point-1])];	//重新设置nextNode	
				nodesr.nowNode.step=20;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}
	else if(flag==1)//第二个门开着
	{		
		while(1)
		{	
			route[temp++]=door2route[i++];	//路线连接
			if(door2route[i]==255)
			{
				route[temp]=door2route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N3,N8)];		//重新设置nowNode
				nodesr.nextNode = Node[getNextConnectNode(N8,route[map.point-1])];	//重新设置nextNode	
				nodesr.nowNode.step=20;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}	
	else if(flag==2)//去到第四个通道口
	{	
		while(1)
		{	
			route[temp++]=door3route[i++];	//路线连接
			if(door3route[i]==255)
			{
				route[temp]=door3route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N3,N4)];		//重新设置nowNode
				nodesr.nextNode = Node[getNextConnectNode(N4,route[map.point-1])];	//重新设置nextNode
				nodesr.nowNode.step=20;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}
	else if(flag==4)//第三个门开着
	{
		while(1)
		{	
			route[temp++]=door4route[i++];	//路线连接
			if(door4route[i]==255)
			{
				route[temp]=door4route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N5,N8)];		//重新设置nowNode
				nodesr.nextNode = Node[getNextConnectNode(N8,route[map.point-1])];	//重新设置nextNode
				nodesr.nowNode.step=10;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}
}

void undermou(void)
{
	float num = 0;
	while(imu.pitch > -3);
	motor_all.Cspeed = 60;
	num = motor_all.Distance;
	while(motor_all.Distance - num < 50);
	buzzer_on();
	if(nodesr.nowNode.nodenum == N14)
	{
		motor_all.Cspeed = 110;
		num = motor_all.Distance;
		while(motor_all.Distance - num < 100);
		motor_all.Cspeed = 60;
	}
	while(!deal_arrive());
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}
void S_curve(void)
{
	float num;
	struct PID_param origin_param = line_pid_param;
	
	num = motor_all.Distance;
	motor_all.Cspeed = nodesr.nowNode.speed;
	
	if (nodesr.nextNode.nodenum == N13)
	{
		motor_all.Cspeed = 110;
		while (fabsf(motor_all.Distance-num) < 110);
		motor_all.Cspeed = 60;
		scaner_set.CatchsensorNum = line_weight[11];  //C1->C2
		line_pid_param.kp = 40;
		while (fabsf(getAngleZ() - nodesr.nextNode.angle) > 10);
	}
	else if (nodesr.nextNode.nodenum == C1)
	{
		motor_all.Cspeed = 110;
		while (fabsf(motor_all.Distance-num) < 60);
		scaner_set.CatchsensorNum = line_weight[5];
		line_pid_param.kp = 70;
		while (fabsf(getAngleZ() - nodesr.nextNode.angle) > 10);
	}
	
	line_pid_param = origin_param;
	scaner_set.CatchsensorNum = 0;
	
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}


void ignore_node(void)
{
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//到达路口
}





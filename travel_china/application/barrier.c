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
#define BACK_SPEED1 -200
#define GOSPEED 10

extern int BW_num[];
uint8_t  value;//openmv�ӿ�
uint8_t special_arrive=0;
//������ϵ�к���
//1._shake��ƽ̨ʶ������У���ֹ��һЩ�������ص��³���ǰֹͣ,ͬʱ���ײ���Ŀ��
//2._Rshake�����ж����Ƿ񾭹����˰�
//3._pshakeʶ��������״̬
void Anti_shake(int Uneed_time)
{
	uint32_t i = 0;  //ѭ���������ó�unit32_t����forѭ���������ָ��
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
//	uint32_t i = 0, cnt = 0;  //ѭ���������ó�unit32_t����forѭ���������ָ��
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
//	uint32_t i = 0;  //ѭ���������ó�unit32_t����forѭ���������ָ��
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

void Stage()		//flag==1ʱȡ���ȽǶȣ�flag==0ʱȡ��ԽǶ�
{
	float num = 0; //distan = 0;
	

	
	if (nodesr.nowNode.nodenum == P2)
	{
		Stage_P2();
		return;
	}
	if ((Scaner.detail & 0X0180) == 0X0180) //��������м�λ��
	{
		angle.AngleG = getAngleZ();//��ƽ���ߵĽǶ�
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 400;
	}
	else{
		angle.AngleG = nodesr.nowNode.angle;//��ƽ���ߵĽǶ�
		pid_mode_switch(is_Gyro);
		motor_all.Gspeed = 400;
	}
    while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//��ʼ��ƽ̨

	while(imu.pitch>After_up)
	{
		vTaskDelay(5);
	}//������

	while(Infrared_ahead == 0)
	{
		vTaskDelay(5);
	}  //ײ����

	num = motor_all.Distance;				
	while(motor_all.Distance - num < 30)//ǰ��һ�ξ���
	{
		vTaskDelay(5);
	}
	
	//delay_ms(300);
	CarBrake();
	
	//HAL_Delay(1000);
	mpuZreset(imu.yaw, nodesr.nowNode.angle);  //������У��
	num = motor_all.Distance;				//����һ�ξ���
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
	nodesr.nowNode.function=0;	//����ϰ���־
	nodesr.flag|=0x04;	//����·��
}
//������
void Special_Node()
{
	
	if(((nodesr.nowNode.flag&DRIGHT)==DRIGHT)&((nodesr.nowNode.flag&CRIGHT)==CRIGHT)&(nodesr.nowNode.nodenum==N5))//N4-N5��ѭ�� ��ѭ�� ��Ե����
	{
		nodesr.nowNode.flag&=(~RIGHT_LINE);//ȡ����ѭ����־λ
		nodesr.nowNode.flag|=LEFT_LINE;//��ѭ��
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}//�ҷֲ�
		nodesr.nowNode.flag&=(~CRIGHT);//ȡ���ҷֲ��־λ
	    while(deal_arrive()!=1)//�Ұ����
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
		nodesr.nowNode.flag|=LEFT_LINE;//��ѭ��
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
		num=motor_all.Distance;
		while(motor_all.Distance-num<10)//��һ����ֲ�·������10����
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
		nodesr.nowNode.flag|=RIGHT_LINE;//��ѭ��
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
		num=motor_all.Distance;
		while(motor_all.Distance-num<10)//��һ����ֲ�·������10����
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
    else if((nodesr.nowNode.nodenum==N9)&((nodesr.nowNode.flag&CLEFT)==CLEFT))//C3-N9
	{
		float num=0;
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
		num=motor_all.Distance;
		while(motor_all.Distance-num<30)
		{
			vTaskDelay(2);
		}
		while(deal_arrive()!=1)
		{
			vTaskDelay(2);
		}
		special_arrive=1;
	}
	else
	{
		angle.AngleG=getAngleZ();
		motor_all.Gspeed=600;
		pid_mode_switch(is_Gyro);
	}
//	if(((nodesr.nowNode.flag&CRIGHT)==CRIGHT)&((nodesr.nowNode.flag&CLEFT)==CLEFT))//N5-N6  P4-N6����ѭ������ѭ�� 
//	{
//		angle.AngleT=getAngleZ();
//		pid_mode_switch(is_Gyro);
////		nodesr.nowNode.flag|=LEFT_LINE;//��ѭ��
////		while(deal_arrive()!=1)
////		{
////			vTaskDelay(2);
////		}//��⵽�ҷֲ�
////		nodesr.nowNode.flag&=(~CRIGHT);//ȡ���ҷֲ��־λ
////		while(deal_arrive()!=1)
////		{
////			vTaskDelay(2);
////		}//��⵽��ֲ�
////		nodesr.nowNode.flag&=(~LEFT_LINE);//ȡ����ѭ��
////		nodesr.nowNode.flag|=RIGHT_LINE;//��ѭ��
////		special_arrive=1;
//	}
}

//ƽ̨���Ķ���
void Stage_P2()		//flag==1ʱȡ���ȽǶȣ�flag==0ʱȡ��ԽǶ�
{
	//float num = 0;
	while(imu.pitch > Up_pitch); //��б��ʱ��Ϊ����ƽ̨
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=40;
	pid_mode_switch(is_Gyro);//��ƽ̨����������
	while (Infrared_ahead == 0);//������

	CarBrake();
	
	Turn_Angle_Relative(182);		//ת>=180�ȣ�
	while (fabs(angle.AngleT - getAngleZ())>5);//�����Ƕ�
	motor_pid_clear();
	nodesr.nowNode.function=0;	//����ϰ���־
	nodesr.flag|=0x04;	//����·��
}

/***************************************************
������
************************************************/

void Barrier_Bridge(float step,float speed)	//������
{
	float num=0;
	num=motor_all.Distance;
	motor_all.Gspeed = 300;    //��ƽ���ٶ�
	if ((Scaner.detail & 0X0180) == 0X0180) //��������м�λ��
	{
		angle.AngleG = getAngleZ();//��ƽ���ߵĽǶ�
		pid_mode_switch(is_Gyro);
	}
	else{
		angle.AngleG = nodesr.nowNode.angle;//��ƽ���ߵĽǶ�
		pid_mode_switch(is_Gyro);
	}
	struct PID_param origin_param = gyroG_pid_param;
	gyroG_pid_param.kp = 4.8;//ԭ��3.7
	
	while(imu.pitch <= Up_pitch)	//����ƽ��,��ѭ������������
	{
		vTaskDelay(2);
	}
	//is_Up = false;
    while(imu.pitch>Up_pitch)
	{
		vTaskDelay(2);
	}//�������
	motor_all.Gspeed = 800;   //ԭ��65   90
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
			angle.AngleG = getAngleZ();//��ȷ�ĽǶ�
			buzzer_off();
		}
		if(infrared.outside_right==0)
		{
			angle.AngleG = getAngleZ() + 2;
            buzzer_on();
		}
		else if(infrared.outside_left==0)
		{
				angle.AngleG = getAngleZ() - 2; 
				buzzer_on();
		}
		else{
			angle.AngleG = getAngleZ();//��ȷ�ĽǶ�
			buzzer_off();
		}
		if (motor_all.Distance-num < 200)
			motor_all.Gspeed = 800;     //ԭ����80  90
		else
			motor_all.Gspeed = 500;      //ԭ����65   75
		vTaskDelay(2);
	}
//	while(imu.pitch<Down_pitch)
//	{
//		angle.AngleG = nodesr.nowNode.angle;
//		motor_all.Cspeed = 300;
//		vTaskDelay(2);
//	}//������
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);	
	}//��ѭ��������ƽ��
	pid_mode_switch(is_Line);
		//·�̼�¼����
	motor_all.Distance = 0;
	motor_all.encoder_avg = 0;
	nodesr.nowNode.function = 0;
	nodesr.flag |= 0X04;  //����·��
	gyroG_pid_param=origin_param;
}

//��¥��
void Barrier_Hill(uint8_t order)  //¥������
{
	angle.AngleG = getAngleZ();
	struct PID_param origin_param = line_pid_param;
	motor_all.Cspeed = 300;   //ԭ����400
	pid_mode_switch(is_Line);
	while ( imu.pitch >10+basic_p )	
	{
		vTaskDelay(2);
	}//��ʼ��¥��
	while(imu.pitch>-10+basic_p)
	{
		vTaskDelay(2);
	}//��ʼ��¥��
    while(imu.pitch<basic_p-3)
	{
		vTaskDelay(2);
	}//����¥��

	line_pid_param = origin_param;
	nodesr.nowNode.function=0;//����ϰ���־
	nodesr.flag|=0x04;	//����·��
}


//��ɽ
void Sword_Mountain()
{
	float num;
	struct PID_param origin_param = line_pid_param;
	struct PID_param origin_param1 = gyroG_pid_param;
	num = motor_all.Distance;
	motor_all.Cspeed = 300; 
	line_pid_param.kp = 50;
	
	
	//mpuZreset(imu.yaw, nodesr.nowNode.angle);  //������У��
	while(motor_all.Distance - num < 25)//ǿ����ѭ��λ��
	{
		vTaskDelay(2);
	}
	angle.AngleG = getAngleZ();
	
	gyroG_pid_param.kp = 2.4;   //����ɽ����ƽ��kp
	motor_all.Gspeed = 200;
	pid_mode_switch(is_Gyro);

	while(imu.pitch < After_up)//��ѭ���ϵ�ɽ
	{
		vTaskDelay(2);
	}
	buzzer_on();
	

	while(imu.pitch > After_down)//��ѭ���µ�ɽ
	{
		vTaskDelay(2);
	}
	gyroG_pid_param = origin_param1;
	line_pid_param = origin_param;
	buzzer_off();
	
	 
	nodesr.nowNode.function=0;//����ϰ���־
	nodesr.flag|=0x04;	//����·��
}

/*****************************************************************************
����壬���������
*****************************************************************************/
void Barrier_HighMountain(float speed)
{
	float origin_turnM=motor_all.GyroT_speedMax;
	float num = 0;
	struct PID_param origin_param = line_pid_param;
	struct PID_param origin_param1=gyroG_pid_param;
	line_pid_param.kp = 25;
	line_pid_param.kd = 15;
	gyroG_pid_param.kp=10;
	motor_all.Cspeed = speed; //�ʼ��û���µ��ٶ� 90
	motor_all.Cincrement = 20;
	pid_mode_switch(is_Line);
	while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//������
    num=motor_all.Distance;
	motor_all.Cincrement = 20; //���µ��ٶ�  2
	motor_all.Cspeed = speed;//80
	while(motor_all.Distance-num<90)//������ѭ��
	{
		vTaskDelay(2);
	}
	motor_all.Gspeed = speed; //��һ��ƽ����ƽ��	
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
	}//�ڶ�����
	buzzer_off();
	
	
	motor_all.Cspeed = speed; 
	motor_all.Cincrement = 20;
	pid_mode_switch(is_Line);
	num=motor_all.Distance;
	while(motor_all.Distance-num<90)//������ѭ��
	{
		vTaskDelay(2);
	}
	motor_all.Gspeed = speed; //�ڶ�����ƽ��	
	angle.AngleG=getAngleZ();
    pid_mode_switch(is_Gyro);
	buzzer_on();
    while(imu.pitch>After_up)
	{
		vTaskDelay(2);
	}//�ڶ�����ƽ̨
	buzzer_off();
	while(Infrared_ahead == 0)
	{
		vTaskDelay(5);
	}  //ײ����
	num = motor_all.Distance;				
	while(motor_all.Distance - num < 30)//ǰ��һ�ξ���
	{
		vTaskDelay(5);
	}
	CarBrake();
	mpuZreset(imu.yaw, nodesr.nowNode.angle);  //������У��
   	
	
	num = motor_all.Distance;
	pid_mode_switch(is_No);
	motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED1;
	while(num-motor_all.Distance < 15)
	{
		vTaskDelay(2);
	}
	CarBrake();
	motor_all.GyroT_speedMax=400;
	Turn_Angle_Relative(179);
	while(fabs(angle.AngleT - getAngleZ())>2) //�ж����
	{
		vTaskDelay(2);
	}
	vTaskDelay(300);
	Turn_Angle360();
	 
	motor_pid_clear();
	pid_mode_switch(is_No);
	

    
    Barrier_Down_HighMountain(450);

	line_pid_param = origin_param;
	gyroG_pid_param=origin_param1;
    motor_all.GyroT_speedMax=origin_turnM;
	nodesr.nowNode.function=0;//����ϰ���־
	nodesr.flag|=0x04;	//����·��
}

/*****************************************************************************
�������ܣ�����壬��ת��180�����
*****************************************************************************/
void Barrier_Down_HighMountain(float speed)
{
	float num=0;
	pid_mode_switch(is_Gyro);
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=450;
	while(imu.pitch>Down_pitch)
	{
		vTaskDelay(2);
	}//����
	pid_mode_switch(is_Line);
	motor_all.Cspeed=450;
	num=motor_all.Distance;
	while(motor_all.Distance-num<80)
	{
		vTaskDelay(2);
	}
	buzzer_on();
	pid_mode_switch(is_Gyro);
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=450;
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);
	}//��һ��ƽ��
	num=motor_all.Distance;
	while(motor_all.Distance-num<20)
	{
		vTaskDelay(2);
	}
	buzzer_off();
	pid_mode_switch(is_Gyro);
	angle.AngleG=getAngleZ();
	motor_all.Gspeed=450;
	while(Scaner.ledNum<=12)
	{
		getline_error();
		vTaskDelay(2);
	}//�յ�����
	while(Scaner.ledNum>4)
	{
		getline_error();
		vTaskDelay(2);
	}//�յ�������
	if((Scaner.detail&0xF000)>0)//���������4������
	{
		pid_mode_switch(is_Turn);
		angle.AngleT=getAngleZ()+10;
		buzzer_on();
	}
	if((Scaner.detail&0xF)>0)//���������4������
	{
		pid_mode_switch(is_Turn);
		angle.AngleT=getAngleZ()-10;
		buzzer_on();
	}
	while(imu.pitch>Down_pitch)
	{
		vTaskDelay(2);
	}//����
	pid_mode_switch(is_Line);
	motor_all.Cspeed=speed;
	num=motor_all.Distance;
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);
	}//�ڵ���
	buzzer_off();
}


void view()//�򾰵�	
{	
	float num = 0;
	while(Infrared_ahead == 0);		//ײ����
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
	nodesr.flag|=0x04;	//����·��
}

void view1()//�򾰵�	
{	
	pid_mode_switch(is_Line);
	while(Infrared_ahead == 0);		//ײ����
	delay_ms(100);
	//mpuZreset(gyro.yaw,nodesr.nowNode.angle-10);
	CarBrake();
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//����·��
}

void back()
{
	pid_mode_switch(is_No);
	
	motor_L0.target = motor_L1.target = BACK_SPEED;
	motor_R0.target = motor_R1.target = BACK_SPEED;
	
	while(infrared.outside_left == 0 && infrared.outside_right == 0);
	CarBrake();
	//ת���ԽǶ�
	angle.AngleT = nodesr.nextNode.angle;
	pid_mode_switch(is_Turn);
	while(fabs(angle.AngleT - getAngleZ())>5);
	
	motor_pid_clear();
	pid_mode_switch(is_Line);
	//motor_all.Cspeed=15;
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//����·��
}
/*****************************************************************************
�����˰�
�ٶ�   ��  ���˰峤�ȣ�cm��
������Ҫ�Ƿ�ֹ������ǰ���벨�˰壬Ȼ����ǰ�����������ʧ��
������һ�γ��ȣ���֤�ܽ��벨�˰壬�ұ�����ǰ���г��Ѿ���ȥ���˰塣
*****************************************************************************/
void Barrier_WavedPlate(float lenght)//���˰峤��
{
	
	float num = 0;
    struct PID_param origin_param = line_pid_param;
	line_pid_param.kp=50;
	motor_all.Cspeed = 300;   
	pid_mode_switch(is_Line);
//	scaner_set.EdgeIgnore=2;
	num = motor_all.Distance;
	while( motor_all.Distance-num <120)
	{
		scaner_set.EdgeIgnore=6;
		getline_error();
		vTaskDelay(2);
	}
	line_pid_param=origin_param;
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//����·��
}


void South_Pole(float length)
{
	float num = 0;
	struct PID_param origin_param = line_pid_param;
	struct PID_param origin_param1=gyroT_pid_param;
	motor_all.Cspeed = 450;
	pid_mode_switch(is_Line);
	line_pid_param.kp = 25;
	line_pid_param.kd = 15;
	gyroT_pid_param.kp=10;
	while(imu.pitch<Up_pitch)
	{
		vTaskDelay(2);
	}//����
	
	motor_all.Cspeed = 450;
	num = motor_all.Distance;
	while(motor_all.Distance - num < 90)
	{
		vTaskDelay(2);
	}		
	
	angle.AngleG = getAngleZ();
	pid_mode_switch(is_Gyro);
	motor_all.Gspeed = 450;

	while(Infrared_ahead == 0)
	{
		vTaskDelay(5);
	}  //ײ����

	num = motor_all.Distance;				
	while(motor_all.Distance - num < 30)//ǰ��һ�ξ���
	{
		vTaskDelay(5);
	}
	

	CarBrake();
	
	//myAction(); //�����˶���+Ѱ�����
	
	mpuZreset(imu.yaw, nodesr.nowNode.angle);  //������У��
	
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
	
	motor_all.Cspeed = 450;
	pid_mode_switch(is_Line);
	while(imu.pitch>Down_pitch)
	{
		vTaskDelay(2);
	}
	while(imu.pitch<After_down)
	{
		vTaskDelay(2);
	}	
	line_pid_param = origin_param;  //�ָ�ԭ����PID����
	gyroT_pid_param=origin_param1;
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//����·��
}




void QQB_1(void)
{
	float num;
	struct PID_param origin_param = line_pid_param;
    struct PID_param origin_param1=gyroG_pid_param;
	gyroG_pid_param.kp=15;
//	gyroT_pid_param.outputMax = 200;
//	gyroT_pid_param.outputMin = 200;
	motor_pid_clear();
	pid_mode_switch(is_Line);
	motor_all.Cspeed = 150;  //�����ٽ���Ѱ��
	line_pid_param.kp = 105;    //����KP 
	nodesr.nowNode.flag|=LEFT_LINE;
	num=motor_all.Distance;
	while(motor_all.Distance-num<20)
	{
		vTaskDelay(2);
	}
	if(nodesr.nowNode.nodenum==B9)
	{
		scaner_set.CatchsensorNum = line_weight[4];    //�������Ȩֵ
		num=motor_all.Distance;
		while(motor_all.Distance-num<52)
		{
			vTaskDelay(2);
		}

		pid_mode_switch(is_Turn);
		angle.AngleT=-90;
		while(fabs(getAngleZ()-angle.AngleT)>2)
		{
			vTaskDelay(2);
		}
		angle.AngleG=getAngleZ();
		motor_all.Gspeed=250;
		pid_mode_switch(is_Gyro);

		num=motor_all.Distance;
		while(motor_all.Distance-num<68)
		{
			infrared_open();
			buzzer_on();
			if(infrared.outside_left==0&infrared.outside_right==1)
			{
				angle.AngleG=getAngleZ()-2;
			}
			else if(infrared.outside_right==0&infrared.outside_left==1)
			{
					angle.AngleG=getAngleZ()+2;
			}
			else
			{
				angle.AngleG=getAngleZ();
			}
				vTaskDelay(2);
		}
			CarBrake();
			vTaskDelay(1000);
			angle.AngleG=getAngleZ();
			motor_all.Gspeed=200;
			pid_mode_switch(is_Gyro);
			while(imu.pitch>Down_pitch)
			{
				vTaskDelay(2);
			}
	//		CarBrake();
			buzzer_off();
	}
	if(nodesr.nowNode.nodenum==B8)
	{
		scaner_set.CatchsensorNum = line_weight[4];    //�������Ȩֵ
		num=motor_all.Distance;
		while(motor_all.Distance-num<45)
		{
			vTaskDelay(2);
		}
		pid_mode_switch(is_Turn);
		angle.AngleT=90;
		while(fabs(getAngleZ()-angle.AngleT)>2)
		{
			vTaskDelay(2);
		}
		angle.AngleG=getAngleZ();
		motor_all.Gspeed=250;
		pid_mode_switch(is_Gyro);

		num=motor_all.Distance;
		while(motor_all.Distance-num<68)
		{
			infrared_open();
			buzzer_on();
			if(infrared.outside_left==0&infrared.outside_right==1)
			{
				angle.AngleG=getAngleZ()-2;
			}
			else if(infrared.outside_right==0&infrared.outside_left==1)
			{
					angle.AngleG=getAngleZ()+2;
			}
			else
			{
				angle.AngleG=getAngleZ();
			}
				vTaskDelay(2);
		}
			CarBrake();
			vTaskDelay(1000);
			angle.AngleG=getAngleZ();
			motor_all.Gspeed=200;
			pid_mode_switch(is_Gyro);
			while(imu.pitch>Down_pitch)
			{
				vTaskDelay(2);
			}
	//		CarBrake();
			buzzer_off();
	}
//	CarBrake();
//	while(1)
//	{
//		vTaskDelay(2);
//	}
	line_pid_param = origin_param;  //�ָ�ԭ����PID����
	gyroG_pid_param=origin_param1;
	nodesr.nowNode.function=0;
	scaner_set.CatchsensorNum =0;
	nodesr.flag|=0X04;	//����·��
	
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
	{		//��һ�������ȫ��ֹͣ��
		if(Scaner.ledNum>=8)
		{
			buzzer_on();
			CarBrake();
			// ʶ����̵Ƶ�ʱ��
			HAL_Delay(400);    
			if(value == 1)
			{
				buzzer_off();
				if(flag==0)	//��һ��·�ڵĽ�ָͨʾ���Ǻ��ƣ������ܰ�·����������
				{
					map.point -= 2;
					route[map.point] = N8;
					pid_mode_switch(is_No);			//����
	
					//motor_L0.target = motor_L1.target = BACK_SPEED;
					//motor_R0.target = motor_R1.target = BACK_SPEED;
					motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;
	
					num = motor_all.Distance;
					while(num-motor_all.Distance<20);
					Turn_Angle_Relative(181);	//	ת����ǰ��㷽��
					while(fabs(angle.AngleT-getAngleZ())>3);
					nodesr.nowNode	= Node[getNextConnectNode(N10,N3)];		//��������nowNode
					nodesr.nowNode.step = 45;
					nodesr.nowNode.flag|=STOPTURN;
					nodesr.nowNode.speed=120;
					pid_mode_switch(is_Line);
					nodesr.flag|=0x20;
					flag=1;
					return ;
				}
				else if(flag==1)//�ڶ���Ҳ�Ǻ�ɫ�������ܰ�·����������
				{		
					map.point-=2;		
					route[map.point]=N4;
					pid_mode_switch(is_No);			//����

					motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;

					num=motor_all.Distance;
					while(num-motor_all.Distance<20);
					Turn_Angle_Relative(181);	//	ת����ǰ��㷽��
					while(fabs(angle.AngleT-getAngleZ())>3);
					nodesr.nowNode	= Node[getNextConnectNode(N8,N3)];		//��������nowNode
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
					pid_mode_switch(is_No);	 //����
	
					//motor_L0.target = motor_L1.target = BACK_SPEED;
					//motor_R0.target = motor_R1.target = BACK_SPEED;
					motor_all.Lspeed = motor_all.Rspeed = BACK_SPEED;
	
					num=motor_all.Distance;
					while(num-motor_all.Distance<20);
					Turn_Angle_Relative(181);	//	ת����ǰ��㷽��
					while(fabs(angle.AngleT-getAngleZ())>3);
					nodesr.nowNode	= Node[getNextConnectNode(N12,N5)];		//��������nowNode
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

	if(flag != 3)   route_reset(flag);//·����������

}


void route_reset(u8 flag)
{
	u8 temp=0,i=0;
	temp = map.point - 1;
	if(flag==0)//��һ���ſ���
	{			
		while(1)
		{	
			route[temp++]=door1route[i++];		//·������
			if(door1route[i]==255)
			{
				route[temp]=door1route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N3,N10)];		//��������nowNode
				nodesr.nextNode = Node[getNextConnectNode(N10,route[map.point-1])];	//��������nextNode	
				nodesr.nowNode.step=20;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}
	else if(flag==1)//�ڶ����ſ���
	{		
		while(1)
		{	
			route[temp++]=door2route[i++];	//·������
			if(door2route[i]==255)
			{
				route[temp]=door2route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N3,N8)];		//��������nowNode
				nodesr.nextNode = Node[getNextConnectNode(N8,route[map.point-1])];	//��������nextNode	
				nodesr.nowNode.step=20;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}	
	else if(flag==2)//ȥ�����ĸ�ͨ����
	{	
		while(1)
		{	
			route[temp++]=door3route[i++];	//·������
			if(door3route[i]==255)
			{
				route[temp]=door3route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N3,N4)];		//��������nowNode
				nodesr.nextNode = Node[getNextConnectNode(N4,route[map.point-1])];	//��������nextNode
				nodesr.nowNode.step=20;
				nodesr.nowNode.speed=120;
				nodesr.nowNode.function=1;
				break;
			}
		}
	}
	else if(flag==4)//�������ſ���
	{
		while(1)
		{	
			route[temp++]=door4route[i++];	//·������
			if(door4route[i]==255)
			{
				route[temp]=door4route[i];	
				nodesr.nowNode	= Node[getNextConnectNode(N5,N8)];		//��������nowNode
				nodesr.nextNode = Node[getNextConnectNode(N8,route[map.point-1])];	//��������nextNode
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
	nodesr.flag|=0x04;	//����·��
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
	nodesr.flag|=0x04;	//����·��
}


void ignore_node(void)
{
	nodesr.nowNode.function=0;
	nodesr.flag|=0x04;	//����·��
}





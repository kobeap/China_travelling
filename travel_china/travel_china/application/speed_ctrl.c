#include "speed_ctrl.h"
#include "motor_task.h"
#include "pid.h"

volatile struct Motors motor_all = {
	.Lspeed = 0,
	.Rspeed = 0,
	.encoder_avg = 0,
	.GyroG_speedMax = 50,  //��ƽ������ƫ�����ֵ
	.GyroT_speedMax = 1000,//�Դ�����ٶ�
	.Cincrement = 25,	//ѭ�����ٶ�
	.Gincrement = 25,		//��ѭ�����ٶ�
	
	.is_UP = false,
	.is_DOWM = false,
};

struct Gradual TC_speed = {0,0,0}, TG_speed = {0,0,0};


void CarBrake(void)
{
	pid_mode_switch(is_No);
	motor_all.Lspeed = motor_all.Rspeed = 0;
}

//��һ�κ����������ٻ��߻���ֹͣ
void gradual_cal(struct Gradual *gradual, float target, float increment)
{
	uint8_t direction = 0;
	
	if(target - gradual->Now < 0)//����
		direction = 0;
	else
		direction = 1;
	
	if(gradual->Now != target)
	{
		if (direction)
			gradual->Now += increment;
		else
			gradual->Now -= increment;
	}
	else 
	{	
		return;
	}
	
	if(direction == 1)//����
	{
		if(gradual->Now > target)
		{
			gradual->Now = target;
		}
	}
	else if(direction == 0)
	{
		if(gradual->Now < target)//�ٶ�С��Ŀ���ٶ�
		{
			gradual->Now = target;
		}
	}
}

#ifndef __BARRIER_H
#define __BARRIER_H

#include "sys.h"
#define basic_p   -5
#define Up_pitch   basic_p+12   //while(imu.pitch<Up_pitch)  出循环 刚上桥
#define Down_pitch basic_p-12   //while(imu.pitch>Down_pitch)出循环 刚下桥 
#define After_down basic_p-3    //while(imu.pitch<After_down)出循环下完 在平地
#define After_up   basic_p+3    //while(imu.pitch>After_up)出循环上完 在平地
extern uint8_t special_arrive;
void Stage(void);
void Barrier_Bridge(float step,float speed);
void Barrier_Hill(uint8_t order) ;
void back(void);
void view1(void);//打景点	
void Sword_Mountain(void);
void Barrier_HighMountain(float speed);
void Barrier_Down_HighMountain(float speed);
void view(void);
void Barrier_WavedPlate(float lenght);
void South_Pole(float L);
void QQB_1(void);
void door(void);
void route_reset(u8 flag);
void Stage_P2(void);
void S_curve(void);
void special_node(void);
void ignore_node(void);
void undermou(void);
void Special_Node(void);
#endif

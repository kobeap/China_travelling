#include "map_message.h"

//��ͼ��Ϣ{������֣���־λ���Ƕȣ����ȣ����ף����ٶȣ����ܺ���}
//���Ĳ�����1Ϊ��ͨ·�ߣ�������ͷ�ļ���ע��
//��/*S1	 1*/	{N3,FLEFT|M2L,160,280,100,1},	�����ӣ���Ѱ���ߣ�����Ŀ�ɶ��С��Ϊ����·�ڡ�  ��S1��ָ��N3��ĽǶ�Ϊ160�ȡ���������Ϊ280cm���Ƽ��ٶ�Ϊ100��������ͨ·�ڡ�
NODE Node[118]={
/*S1	 1*/	{N3,CLEFT,160,250,110,1},

/*P1 	 2*/	{N1,CRIGHT,180,30,70,1},	// step 25

/*N1 	 3*/	{P1,NO,0,30,80,UpStage},{B2,NO,135,40,50,BHill},{B1,NO,180,25,70,BBridge},

/*B1	 K*/	{N1,CLEFT,0,40,80,1},/* step 25 *//*{N1,NO,0,25,70,IGNORE},¥�ݱ��˵�*/{N2,CRIGHT|LEFT_LINE,180,70,90,1},

/*B2	 K*/	{N1},{N4,CLEFT|DLEFT|STOPTURN,132,35,70,1},

/*B3	 K*/	{N2,CLEFT,-160,20,70,1},{N4,CLEFT,30,64,70,1},

/*N2 	 4*/	{B3,NO,30,30,70,BLBS}, {P2,NO,-178,86,70,UpStage},{B1,NO,0,10,90,BBridge},//�ٶ�ԭ����50  70
	
/*P2 	 5*/	{N2,/*CLEFT|DLEFT*/CLEFT|RIGHT_LINE,0,20,90,1},	// step 10		

/*S2   6*/	{N6,CLEFT,30,60,100,1},			

/*P3	 7*/	{N3,DRIGHT|CRIGHT|STOPTURN,180,285,130,1},				//ԭ����130

/*N3 	 8*/	{S1,NO,-28,280,80,View}, {P3,NO,-1,275,130,UpStage},/*step 300*/ {N10,DRIGHT,85,10,70,DOOR}, {N8,DRIGHT,130,25,70,DOOR}, {N4,CLEFT,180,120,130,1},	
	
/*N4 	 9*/	{B2}, {N3,DLEFT|CLEFT,0,120,130,1},/*step100*/ {N5,DRIGHT|CRIGHT,-178,110,130,1}, /* 175 116 */{B3,NO,-150,64,70,BLBS},			
	
/*N5 	10*/	{N4,CRIGHT,0,120,130,1}, {N8,DLEFT|STOPTURN,45,160,70,1}, {N12,AWHITE|STOPTURN|CRIGHT,95,20,70,DOOR},{N6,CLEFT|CRIGHT|MUL2MUL,180,70,130,1},//ԭ����130	
	
/*N6 	11*/	{N5,DLEFT|CLEFT,0,80,130,1}, {C1,CRIGHT|DRIGHT|STOPTURN,150,200,90,1},{P4,NO,180,93,130,UpStage}, /*step 100*/{S2,NO,-140,80,85,View},
	
/*P4 	12*/	{N6,MUL2SING|CRIGHT|CLEFT,0,145,130,1},

/*N7 	13*/	{P5,NO,90,50,60,UpStage}, {B9},{B8,LEFT_LINE,10,40,30,QQB},	

/*P5 	14*/	{N7,CLEFT,-90,20,50,1},	

/*B8    K*/ {N7},{N9,CRIGHT,150,30,30,1},

/*B9    K*/ {N7,DLEFT|CLEFT,0,1,60,1},{N9},

/*N8 	15*/	{N3,CRIGHT|CLEFT|STOPTURN,-45,10,70,1}, {N10,CRIGHT|CLEFT,38,186,70,1}, {N12,CRIGHT|CLEFT|STOPTURN,150,150,130,1}, {N5,CRIGHT|CLEFT,-140,186,70,1},	
	
/*C1 	16*/	{N6,CRIGHT,-50,151,70,1}, {C2,NO,10,10,110,Scurve},	

/*C2 	17*/	{C1,CLEFT,170,154,70,1}, {N13,CRIGHT|CLEFT,120,40,90,1},//ԭ������120��־λ��MCLEFT

/*C3 	18*/	{N14,DLEFT,90,53,70,1}, {N9,MUL2SING|CLEFT,180,60,80,1},			

/*N9 	19*/	{C3,CLEFT,0,94,70,1}, {N10,RESTMPUZ|CLEFT|CRIGHT,180,194,70,1}, {B8},{B9,LEFT_LINE,-170,130,50,QQB},	
	
/*N10	20*/	{N9,CRIGHT,0,194,70,1}, {N15,DRIGHT|CRIGHT,90,15,115,1}, {N12,DLEFT|DRIGHT|RESTMPUZ,-180,264,70,1}, {N8,CLEFT|CRIGHT|DLEFT|DRIGHT,-140,136,70,1}, {N3,RESTMPUZ|DRIGHT|DRIGHT,-90,240,50,1}, 
	
/*N12	22*/	{N10,DLEFT|DRIGHT|CRIGHT|CLEFT,0,294,70,1}, {N16,DRIGHT,90,30,60,1}, {N13,CLEFT|CRIGHT|MUL2SING,180,90,130,1}, {N5,CRIGHT|DRIGHT,-90,232,70,1}, {N8,CRIGHT|DLEFT,-43,186,70,1},		
	
/*N13   23*/	{N12,AWHITE,0,184,80,1}, {N18,CRIGHT|CLEFT|MUL2SING,65,100,110,1}, {P6,NO,180,65,130,UpStage}, {C2,CRIGHT,-45,15,70,Scurve},	
	
/*P6 	24*/	{N13,CLEFT|DRIGHT|DLEFT|CRIGHT|STOPTURN,0,80,130,1},		//70		

/*N14	25*/	{C3,DRIGHT|CRIGHT,-90,3,80,1}, {C7,DLEFT}, {S3,NO,-178,29,40,View1},				

/*S3 	26*/	{N14,NO,180,0,50,BACK},//{N14,DRIGHT|CRIGHT,0,20,50,1},	//{N14,NO,180,0,50,BACK},		

/*S4	27*/	{N15,NO,0,0,50,BACK},//{N15,DRIGHT|CRIGHT,180,20,50,1},	//{N15,NO,0,0,50,BACK},		

/*N15 28*/	{S4,NO,0,64,30,View1}, {C5,DLEFT|STOPTURN,90,4,100,1}, {N10,DLEFT|DRIGHT,-90,5,60,1},		
	
/*S5 	29*/	{N16,NO,0,0,50,BACK},//{N16,DRIGHT|CRIGHT,180,20,50,1}, //{N16,NO,0,0,50,BACK},		

/*C4 	30*/	{C8,RESTMPUZ|DRIGHT|CRIGHT|STOPTURN,90,90,110,UNDER}, /*����ԭ��160*/{N20,CRIGHT,150,204,70,1},			

/*C5 	31*/	{N15,DLEFT,-90,46,70,1},{B4,NO,180,50,130,IGNORE},

/*B4   K*/  {C5,DRIGHT,0,86,70,1},{N18,DLEFT|CLEFT|RESTMPUZ|STOPTURN,180,210,130,1},

/*B5   K*/  {N18,DRIGHT,0,54,70,1},{N19,DRIGHT|STOPTURN|CRIGHT,180,120,110,1},

/*B6   K*/  {N20,CRIGHT|DRIGHT|RESTMPUZ,0,55,90,1},/*ԭ��־λ��CRIGHT*/{N22,DRIGHT,180,104,70,1}, 

/*B7   K*/  {N22,DLEFT|STOPTURN|CLEFT,0,65,110,1},/*ԭ��������40�ٶ�60*/{C6},

/*N16 32*/  {S5,NO,0,52,40,View1}, {N12,DLEFT|DRIGHT|STOPTURN,-90,2,110,1}, {N18,AWHITE,90,57,70,1},
	
/*N18	34*/	{B4,NO,0,86,70,IGNORE}, {B5,NO,180,25,110,BHill}, {N16,DLEFT|CLEFT,-90,4,130,1},	
	
/*N19	35*/	{B5,NO,0,144,70,BHill}, {C6,RESTMPUZ|DRIGHT|STOPTURN,90,75,110,1},/*����79�ٶ�80*/ {N13,CRIGHT},	

/*P7 	36*/	{N20,RESTMPUZ|CLEFT,180,10,50,1},		//����60							

/*N20   37*/	{C4,CLEFT,-45,180,110,1},/*����ԭ��204*/ {P7,NO,0,30,90,BHM}, {B6,NO,180,90,70,LBHill},
	
/*N22	39*/	{C9,DLEFT|CLEFT|STOPTURN,89,37,80,1},/*����37*/ {B7,NO},{B6,RESTMPUZ,0,15,20,SM},	

/*C6 	40*/	{B7,NO,0,55,110,BHill}/*step 120 �ٶ�90 */, {N19,DLEFT},						

/*C7	41*/	{N14,DRIGHT|CRIGHT,-90,45,60,UNDER}, {C8,DLEFT,180,132,70,1},				

/*C8 	42*/	{C7,DRIGHT|CRIGHT|STOPTURN,0,100,110,1}, {C4,CRIGHT,-90,189,80,1},				

/*C9	43*/	{N22,DLEFT|DRIGHT|AWHITE|STOPTURN,-90,15,70,1}, {P8,NO,180,20,45,BSoutPole},	//60�ٶ�	

/*P8 	44*/	{C9,DRIGHT|STOPTURN,0,5,70,1}													
};

//ÿ���ڵ����ڽ�����Ŀ
unsigned char ConnectionNum[50] = {	//each node's connected node
	1, 1, 3, 2, 2, 2, 3, 1, 1, 1, 5, 4, 4, 4, 1, 3, 1, 2, 2, 4, 2, 2, 2, 4, 5, 5, 4, 1, 3, 1, 1, 3, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 1, 3, 3, 2, 2, 2, 2, 1
};

//ÿ���ڵ�����ڽ����׵�ַ
unsigned char Address[50] = {	//each type connected node's address
	0, 1, 2, 5, 7, 9, 11, 14, 15, 16, 17, 22, 26, 30, 34, 35, 38, 39, 41, 43, 47, 49, 51, 53, 57, 62, 67, 71, 72, 75, 76, 77, 80, 81, 83, 85, 87, 89, 91, 93, 96, 99, 102, 103, 106, 109, 111, 113, 115, 117
};


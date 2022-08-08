#include "QR.h"
#include "map.h"
int BW_num[3]={0,0,0};
int check_BW(u16 node)
{
	switch(node)
	{
		case P3:
			if(BW_num[0] == 3) return 1;else return 0;//3   
		case P4:
			if(BW_num[0] == 4) return 1;else return 0;//4
		case P5:
			if(BW_num[1] == 6) return 1;else return 0;//6
		case P6:
			if(BW_num[1] == 5) return 1;else return 0;//5
		case P7:
			if(BW_num[2] == 8) return 1;else return 0;//8
		case P8:
			if(BW_num[2] == 7) return 1;else return 0;//7
		default: return 0;
	}
		
}
//二维码1-31 2-32  以此类推
 
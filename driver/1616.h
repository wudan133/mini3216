#ifndef __1616_H__
#define __1616_H__

#include "typedef.h"
#include "font.h"


sbit R1=P3^0;    //������������
sbit SK=P3^1;    //��������ʱ������
sbit LT=P3^2;    //������������

extern UINT8 g_buff[64];

void DisplayScreen(void);
void DisplayMute(void);
void display_time_stop(void);
//void DisplayRun(UINT8 *pu8_buff1, UINT8 *pu8_buff2);

#endif
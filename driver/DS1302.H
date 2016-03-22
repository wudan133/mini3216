/*-----------------------------------------------
  程序名称:Cube时钟程序头文件（DS1302.h）
  开发人员：吴泽沛、吴丹
  编写：吴丹
  日期：2012.11. 24
------------------------------------------------*/
#ifndef __DS1302_H__
#define __DS1302_H__

#include "typedef.h"

sbit  DS1302_CLK = P2^5;              //实时时钟时钟线引脚 
sbit  DS1302_IO  = P2^6;              //实时时钟数据线引脚 
sbit  DS1302_RST = P2^7;              //实时时钟复位线引脚

sbit  ACC0 = ACC^0;
sbit  ACC7 = ACC^7;

#define ds1302_sec_add			0x80
#define ds1302_min_add			0x82
#define ds1302_hr_add			0x84
#define ds1302_date_add			0x86
#define ds1302_month_add		0x88
#define ds1302_day_add			0x8a
#define ds1302_year_add			0x8c
#define ds1302_control_add		0x8e
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

#define year    1
#define month   2
#define date    3
#define hour    4
#define minute  5
#define second  6
#define week    7

extern UINT8 g_Timerbuff[8];//年 月 日 时 分 秒 周 （十进制）


void Ds1302_Read_Time(UINT8 *Timerbuff);
void Ds1302_Write_Time(UINT8 *Timerbuff);

void Ds1302_Init(UINT8 *Timerbuff);

#endif
#include "DS1302.h"

                         //年 月 日 时 分 秒 周 （十进制）
UINT8 g_Timerbuff[8] = {20,16,03,22,12,00,00,02};	

/*****************************************************
函数功能：向1302写一个字节数据
入口参数：x
***************************************************/ 
void Write1302(UINT8 dat)
{
	unsigned char i;
	ACC = dat;
	for(i=8; i>0; i--)
	{
		DS1302_IO  = ACC0;           	//相当于汇编中的 RRC
		DS1302_CLK = 1;
		DS1302_CLK = 0;                 //发一个高跳变到低的脉冲
		ACC = ACC >> 1; 
	}
}
/*****************************************************
函数功能：从1302读一个字节数据
入口参数：x
***************************************************/ 
UINT8 Read1302(void)
{
	unsigned char i;
	for(i=8; i>0; i--)
	{
		ACC = ACC >>1;         			//相当于汇编中的 RRC 
		ACC7 = DS1302_IO;
		DS1302_CLK = 1;
		DS1302_CLK = 0;                 //发一个高跳变到低的脉冲
	} 
	return(ACC);
} 
/*****************************************************
函数功能：根据命令字，向1302写一个字节数据
入口参数：Cmd，储存命令字；dat，储存待写的数据
***************************************************/ 
void Ds1302_Write_Byte(UINT8 Cmd,UINT8 dat)
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1; 
    Write1302(Cmd);       	// 地址，命令 
    Write1302(dat);       	// 写1Byte数据
    DS1302_CLK = 1;
    DS1302_RST = 0;                  //RST 0->1->0,CLK 0->1
}
 
/*****************************************************
函数功能：根据命令字，从1302读取一个字节数据
入口参数：Cmd
***************************************************/ 
UINT8  Ds1302_Read_Byte(UINT8 Cmd)
{
    UINT8 ucData;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;                //enable
    Write1302(Cmd|0x01);     // 地址，命令 
    ucData = Read1302();   // 读1Byte数据
    DS1302_CLK = 1;                //RST 0->1->0,CLK 0->1
    DS1302_RST = 0;                    
    return(ucData);
}

/*------------------------------------------------
           向DS1302写入时钟数据
------------------------------------------------*/
void Ds1302_Write_Time(UINT8 *Timerbuff) 
{
     
    UINT8 i,tmp;
	UINT8 time_buf[8] ;

	for(i=0;i<8;i++)			  //BCD处理
    {                  
		tmp=Timerbuff[i]/10;
		time_buf[i]=Timerbuff[i]%10;
		time_buf[i]=time_buf[i]+tmp*16;					//*16 效果等于数据左移4位
    }
	Ds1302_Write_Byte(ds1302_control_add,0x00);			//关闭写保护 
	Ds1302_Write_Byte(ds1302_sec_add,0x80);				//暂停 
	Ds1302_Write_Byte(ds1302_charger_add,0xa9);			//涓流充电 
	Ds1302_Write_Byte(ds1302_year_add,time_buf[1]);		//年 
	Ds1302_Write_Byte(ds1302_month_add,time_buf[2]);	//月 
	Ds1302_Write_Byte(ds1302_date_add,time_buf[3]);		//日 
	Ds1302_Write_Byte(ds1302_day_add,time_buf[7]);		//周 
	Ds1302_Write_Byte(ds1302_hr_add,time_buf[4]);		//时 
	Ds1302_Write_Byte(ds1302_min_add,time_buf[5]);		//分
	Ds1302_Write_Byte(ds1302_sec_add,time_buf[6]);		//秒
	Ds1302_Write_Byte(ds1302_control_add,0x80);			//打开写保护 
}

/*------------------------------------------------
           从DS1302读出时钟数据
------------------------------------------------*/
void Ds1302_Read_Time(UINT8 *Timerbuff)  
{ 
   	UINT8 i,tmp;
	UINT8 time_buf[8];

	time_buf[1]=Ds1302_Read_Byte(ds1302_year_add);		//年 
	time_buf[2]=Ds1302_Read_Byte(ds1302_month_add);		//月 
	time_buf[3]=Ds1302_Read_Byte(ds1302_date_add);		//日 
	time_buf[4]=Ds1302_Read_Byte(ds1302_hr_add);		//时 
	time_buf[5]=Ds1302_Read_Byte(ds1302_min_add);		//分 
	time_buf[6]=(Ds1302_Read_Byte(ds1302_sec_add))&0x7F;//秒	最高位去零 
	time_buf[7]=Ds1302_Read_Byte(ds1302_day_add);		//周 


	for(i=0;i<8;i++)
	   {           //BCD处理
		tmp=time_buf[i]/16;
		Timerbuff[i]=time_buf[i]%16;
		Timerbuff[i]=Timerbuff[i]+tmp*10;
	   }
}

/*------------------------------------------------
                DS1302初始化
------------------------------------------------*/
void Ds1302_Init(UINT8 *Timerbuff)
{
    UINT8 flag; 
    flag= Ds1302_Read_Byte(0x81);
	if(flag&0x80)								//判断时钟芯片是否关闭
	{      
		Ds1302_Write_Time(Timerbuff);
  	}				 
}



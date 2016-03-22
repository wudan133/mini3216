#include "DS1302.h"

                         //�� �� �� ʱ �� �� �� ��ʮ���ƣ�
UINT8 g_Timerbuff[8] = {20,16,03,22,12,00,00,02};	

/*****************************************************
�������ܣ���1302дһ���ֽ�����
��ڲ�����x
***************************************************/ 
void Write1302(UINT8 dat)
{
	unsigned char i;
	ACC = dat;
	for(i=8; i>0; i--)
	{
		DS1302_IO  = ACC0;           	//�൱�ڻ���е� RRC
		DS1302_CLK = 1;
		DS1302_CLK = 0;                 //��һ�������䵽�͵�����
		ACC = ACC >> 1; 
	}
}
/*****************************************************
�������ܣ���1302��һ���ֽ�����
��ڲ�����x
***************************************************/ 
UINT8 Read1302(void)
{
	unsigned char i;
	for(i=8; i>0; i--)
	{
		ACC = ACC >>1;         			//�൱�ڻ���е� RRC 
		ACC7 = DS1302_IO;
		DS1302_CLK = 1;
		DS1302_CLK = 0;                 //��һ�������䵽�͵�����
	} 
	return(ACC);
} 
/*****************************************************
�������ܣ����������֣���1302дһ���ֽ�����
��ڲ�����Cmd�����������֣�dat�������д������
***************************************************/ 
void Ds1302_Write_Byte(UINT8 Cmd,UINT8 dat)
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1; 
    Write1302(Cmd);       	// ��ַ������ 
    Write1302(dat);       	// д1Byte����
    DS1302_CLK = 1;
    DS1302_RST = 0;                  //RST 0->1->0,CLK 0->1
}
 
/*****************************************************
�������ܣ����������֣���1302��ȡһ���ֽ�����
��ڲ�����Cmd
***************************************************/ 
UINT8  Ds1302_Read_Byte(UINT8 Cmd)
{
    UINT8 ucData;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;                //enable
    Write1302(Cmd|0x01);     // ��ַ������ 
    ucData = Read1302();   // ��1Byte����
    DS1302_CLK = 1;                //RST 0->1->0,CLK 0->1
    DS1302_RST = 0;                    
    return(ucData);
}

/*------------------------------------------------
           ��DS1302д��ʱ������
------------------------------------------------*/
void Ds1302_Write_Time(UINT8 *Timerbuff) 
{
     
    UINT8 i,tmp;
	UINT8 time_buf[8] ;

	for(i=0;i<8;i++)			  //BCD����
    {                  
		tmp=Timerbuff[i]/10;
		time_buf[i]=Timerbuff[i]%10;
		time_buf[i]=time_buf[i]+tmp*16;					//*16 Ч��������������4λ
    }
	Ds1302_Write_Byte(ds1302_control_add,0x00);			//�ر�д���� 
	Ds1302_Write_Byte(ds1302_sec_add,0x80);				//��ͣ 
	Ds1302_Write_Byte(ds1302_charger_add,0xa9);			//������ 
	Ds1302_Write_Byte(ds1302_year_add,time_buf[1]);		//�� 
	Ds1302_Write_Byte(ds1302_month_add,time_buf[2]);	//�� 
	Ds1302_Write_Byte(ds1302_date_add,time_buf[3]);		//�� 
	Ds1302_Write_Byte(ds1302_day_add,time_buf[7]);		//�� 
	Ds1302_Write_Byte(ds1302_hr_add,time_buf[4]);		//ʱ 
	Ds1302_Write_Byte(ds1302_min_add,time_buf[5]);		//��
	Ds1302_Write_Byte(ds1302_sec_add,time_buf[6]);		//��
	Ds1302_Write_Byte(ds1302_control_add,0x80);			//��д���� 
}

/*------------------------------------------------
           ��DS1302����ʱ������
------------------------------------------------*/
void Ds1302_Read_Time(UINT8 *Timerbuff)  
{ 
   	UINT8 i,tmp;
	UINT8 time_buf[8];

	time_buf[1]=Ds1302_Read_Byte(ds1302_year_add);		//�� 
	time_buf[2]=Ds1302_Read_Byte(ds1302_month_add);		//�� 
	time_buf[3]=Ds1302_Read_Byte(ds1302_date_add);		//�� 
	time_buf[4]=Ds1302_Read_Byte(ds1302_hr_add);		//ʱ 
	time_buf[5]=Ds1302_Read_Byte(ds1302_min_add);		//�� 
	time_buf[6]=(Ds1302_Read_Byte(ds1302_sec_add))&0x7F;//��	���λȥ�� 
	time_buf[7]=Ds1302_Read_Byte(ds1302_day_add);		//�� 


	for(i=0;i<8;i++)
	   {           //BCD����
		tmp=time_buf[i]/16;
		Timerbuff[i]=time_buf[i]%16;
		Timerbuff[i]=Timerbuff[i]+tmp*10;
	   }
}

/*------------------------------------------------
                DS1302��ʼ��
------------------------------------------------*/
void Ds1302_Init(UINT8 *Timerbuff)
{
    UINT8 flag; 
    flag= Ds1302_Read_Byte(0x81);
	if(flag&0x80)								//�ж�ʱ��оƬ�Ƿ�ر�
	{      
		Ds1302_Write_Time(Timerbuff);
  	}				 
}



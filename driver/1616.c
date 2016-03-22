#include "1616.h"
#include "UART.h"
#include "DS1302.H"

//定义扫描的数据码
UINT32 code ROW_code[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
                        0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

UINT8 g_buff[64] = {0};

void DisplayLine(UINT8 u8_line, UINT8 *u8_buff)
{
    UINT8  i,j,k;
    UINT16 num;
    UINT8  buff;
    
    for(i=0;i<2;++i)
    {
        num=ROW_code[u8_line];
        for(k=0;k<16;++k)      //行扫描 ，锁存一行
        {
            SK=0;              //时钟
            R1=num&0x8000;
            SK=1;
            num<<=1;
        }
        for(j=0;j<2;++j)        //16点
        {
            buff = u8_buff[j+i*2];
            for(k=0;k<8;k++)       
            {
                SK=0;
                R1=~buff&0x80;
                SK=1;  
                buff<<=1;
            }   
        }                  
    }
    LT=0;                  //锁存脚
    LT=1;    
}



void DisplayScreen(void)
{
    static UINT8 u8_line = 0;
    u8_line = ++u8_line%16;
    
    DisplayLine((u8_line), g_buff+(u8_line*4));
}

void DisplayMute(void)
{
    UINT8 i;
    for(i=0;i<64;++i)      //行扫描 ，锁存一行
    {
        SK=0;              //时钟
        R1=0x00;
        SK=1;
    }
    LT=0;                  //锁存脚
    LT=1; 
}

void display_time_stop(void)
{
    unsigned char i;
    unsigned char temp,temp1;
    unsigned char D_buff2[4] = {0};
    UINT8 b_Dot=0;
    
    Ds1302_Read_Time(g_Timerbuff);              //不能注释掉
    
    b_Dot = g_Timerbuff[second]%2;

    D_buff2[0] = g_Timerbuff[hour]/10;        //时十位
    D_buff2[1] = g_Timerbuff[hour]%10;        //时个位

    D_buff2[2] = g_Timerbuff[minute]/10;        //分十位
    D_buff2[3] = g_Timerbuff[minute]%10;        //分个位
    
    for (i=0;i<16;i++) //1
    {
        temp  = Timer_6x16[D_buff2[0]*16+i];
        temp1 = Timer_6x16[D_buff2[1]*16+i];
        g_buff[i*4] = (temp | ((temp1 >>=7)&0x01));
    }
    
    for (i=0;i<16;i++)  //2
    {
        temp  = Timer_6x16[D_buff2[1]*16+i];
        temp1 = Timer_6x16[(10+b_Dot)*16+i];
        g_buff[i*4+1] = ((temp<<1) | (temp1&0x01));
    }
    
    for (i=0;i<16;i++)  //3
    {
        temp  = Timer_6x16[D_buff2[2]*16+i];
        temp1 = Timer_6x16[(10+b_Dot)*16+i];
        g_buff[i*4+2] = ((temp>>3) | (temp1&0x80));
    }
    
    for (i=0;i<16;i++)  //4
    {
        temp  = Timer_6x16[D_buff2[3]*16+i];
        temp1 = Timer_6x16[D_buff2[2]*16+i];
        g_buff[i*4+3] = ((temp>>2) | ((temp1<<5)&0xc0));
    }
    
}

#if 0
void DisplayRun(UINT8 *pu8_buff1, UINT8 *pu8_buff2)
{
    static UINT8 u8_count=0;
    UINT8 i, u8_bit, u8_byte;
       
    u8_count = ++u8_count%32;

    u8_count = ReadBuff[buf_count];

    u8_byte  = u8_count/8;
    u8_bit   = u8_count%8;

    for(i=0;i<64;i++)
    {
        switch (u8_byte)
        {
        case 0:
            switch (i%4)
            {
            case 0:
            case 1:
            case 2:
                g_buffRun[i] = ((pu8_buff1[i]<<u8_bit) | (pu8_buff1[i+1] >> (8-u8_bit)));
                break;
            case 3:
                g_buffRun[i] = ((pu8_buff1[i]<<u8_bit) | (pu8_buff2[i-3] >> (8-u8_bit)));
                break;
            }
            break;
        case 1:
            switch (i%4)
            {
            case 0:
            case 1:
                g_buffRun[i] = ((pu8_buff1[i+1]<<u8_bit) | (pu8_buff1[i+2] >> (8-u8_bit)));
                break;
            case 2:
                g_buffRun[i] = ((pu8_buff1[i+1]<<u8_bit) | (pu8_buff2[i-2] >> (8-u8_bit)));
                break;
            case 3:
                g_buffRun[i] = ((pu8_buff2[i-3]<<u8_bit) | (pu8_buff2[i-2] >> (8-u8_bit)));
                break;
            }
            break;
        case 2:
            switch (i%4)
            {
            case 0:
                g_buffRun[i] = ((pu8_buff1[i+2]<<u8_bit) | (pu8_buff1[i+3] >> (8-u8_bit)));
                break;
            case 1:
                g_buffRun[i] = ((pu8_buff1[i+2]<<u8_bit) | (pu8_buff2[i-1] >> (8-u8_bit)));
                break;
            case 2:
            case 3:
                g_buffRun[i] = ((pu8_buff2[i-2]<<u8_bit) | (pu8_buff2[i-1] >> (8-u8_bit)));
                break;
            }
            break;
        case 3:
            switch (i%4)
            {
            case 0:
                g_buffRun[i] = ((pu8_buff1[i+3]<<u8_bit) | (pu8_buff2[i] >> (8-u8_bit)));
                break;
            case 1:
            case 2:
            case 3:
                g_buffRun[i] = ((pu8_buff2[i-1]<<u8_bit) | (pu8_buff2[i] >> (8-u8_bit)));
                break;
            }
            break;
        }
    }
}
#endif

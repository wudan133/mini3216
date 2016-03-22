#include "TimerPolling.h"
#include "UART.h"
#include "DS1302.H"
#include "1616.h"
#include "timer.h"


#define INTERRUPT_TIME          500UL //us
#define INTERRUPT_CECOUNT       5     //
#define DISPLAY_LINE_TIME       1000  //us
#define DIAPLAY_TIMER_COUNT     20   //100ms
#define RUNSPEED                2


UINT8 g_LineCount  = 0;
UINT8 g_InterCount = 0;

UINT8 g_RunPlase   = 0;

void InitSys(void);
void Time0Init(void);


/**************************************************************************************
* FunctionName   : InitSys()
* Description    : 系统初始化
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void InitSys(void)
{    
    UartInit(9600);
    Time0Init();
    Ds1302_Init(g_Timerbuff);
    SendString("SysInit OK!\r\n");
}

/**************************************************************************************
* FunctionName   : main()
* Description    : 主函数
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
int main(void) 
{
    DisplayMute();
    P3 = 0xFF;                  //释放UARE脚。
    InitSys();                  // 初始化

    
    while (1)
    {
        P3 = 0xFF;                  //释放UARE脚。
        mculib_uart_service();      
        TaskProcess();              // 任务处理      
    }
}

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1  //1ms
{
	TL0 = 0xCD;		        //设置定时初值
	TH0 = 0xD4;		        //设置定时初值
    TaskRemarks();
}


/**************************************************************************************
* FunctionName   : InitSys()
* Description    : 定时器0初始化
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Time0Init(void)
{
    TMOD &= 0xf1; 
    TMOD |= 0x01;   //T0 gate = 0 means counter is only controled by TR, C/T = 0 means timer, mode =1 means 16bit;
    AUXR |= 0x80;   //T0 is 1T mode
	TL0 = 0xCD;	    //设置定时初值
	TH0 = 0xD4;     //设置定时初值
    TR0 = 1;        //timer0 start running
    ET0 = 1;        //enable timer0 interrupt
    
    EA = 1;
}


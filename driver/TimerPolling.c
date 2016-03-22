#include "TimerPolling.h"
#include "UART.h"
#include "1616.h"

sbit LED = P1^4;


void Task1(void);

#define TASK_NUM   (3)                  //  这里定义的任务数为4，表示有三个任务会使用此定时器定时。
/**************************************************************************************
* Variable definition
* 1. 程序运行标记：0-不运行，1运行
* 2. 计时器
* 3. 任务运行间隔时间 *1ms
* 4. 要运行的任务函数
**************************************************************************************/
static TASK_COMPONENTS TaskComps[] = 
{
    {0, 0, 2,    DisplayScreen},
    {0, 0, 2,    Task1},
    {0, 0, 500,  display_time_stop},
    
     // 这里添加你的任务。。。。
};


/**************************************************************************************
* FunctionName   : Task1()
* Description    : 任务 1
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Task1(void)
{
    
}

/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : 任务标志处理 //1ms
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskRemarks(void)      //1ms
{
    UINT8 i;
    for (i=0; i<TASK_NUM; i++)          // 逐个任务时间处理
    {
         
        if (TaskComps[i].Timer != 0)    // 时间不为0
        {
            TaskComps[i].Timer--;           // 减去一个节拍
        }
        else                            // 时间减完了
        {
            TaskComps[i].Timer = TaskComps[i].ItvTime;       // 恢复计时器值，从新下一次
            TaskComps[i].Run = 1;      // 任务可以运行
        }
   }
}

/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : 任务处理
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskProcess(void)
{
    if (TaskComps[0].Run)
    {
        TaskComps[0].TaskHook();
        TaskComps[0].Run = 0;
    }
    
    else if (TaskComps[1].Run)
    {

        TaskComps[1].TaskHook();
        TaskComps[1].Run = 0;
    } 
    
    else if (TaskComps[2].Run)
    {

        TaskComps[2].TaskHook();
        TaskComps[2].Run = 0;
    }
}

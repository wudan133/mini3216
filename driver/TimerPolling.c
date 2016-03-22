#include "TimerPolling.h"
#include "UART.h"
#include "1616.h"

sbit LED = P1^4;


void Task1(void);

#define TASK_NUM   (3)                  //  ���ﶨ���������Ϊ4����ʾ�����������ʹ�ô˶�ʱ����ʱ��
/**************************************************************************************
* Variable definition
* 1. �������б�ǣ�0-�����У�1����
* 2. ��ʱ��
* 3. �������м��ʱ�� *1ms
* 4. Ҫ���е�������
**************************************************************************************/
static TASK_COMPONENTS TaskComps[] = 
{
    {0, 0, 2,    DisplayScreen},
    {0, 0, 2,    Task1},
    {0, 0, 500,  display_time_stop},
    
     // �������������񡣡�����
};


/**************************************************************************************
* FunctionName   : Task1()
* Description    : ���� 1
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Task1(void)
{
    
}

/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : �����־���� //1ms
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskRemarks(void)      //1ms
{
    UINT8 i;
    for (i=0; i<TASK_NUM; i++)          // �������ʱ�䴦��
    {
         
        if (TaskComps[i].Timer != 0)    // ʱ�䲻Ϊ0
        {
            TaskComps[i].Timer--;           // ��ȥһ������
        }
        else                            // ʱ�������
        {
            TaskComps[i].Timer = TaskComps[i].ItvTime;       // �ָ���ʱ��ֵ��������һ��
            TaskComps[i].Run = 1;      // �����������
        }
   }
}

/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : ������
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

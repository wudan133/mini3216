
#include "typedef.h"


// ����ṹ
typedef struct _TASK_COMPONENTS
{
    uint8  Run;                // �������б�ǣ�0-�����У�1����
    uint16 Timer;              // ��ʱ��
    uint16 ItvTime;            // �������м��ʱ��
    void (*TaskHook)(void);    // Ҫ���е�������
} TASK_COMPONENTS;             // ������


void TaskRemarks(void);
void TaskProcess(void);
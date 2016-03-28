#ifndef    __FFT_H__
#define    __FFT_H__

#include<math.h>


#define PI 3.1415926535897932384626433832795028841971               //定义圆周率值

struct compx {float real,imag;};                                    //定义一个复数结构

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************
函数原型：void FFT(struct compx *xin,int N)
函数功能：对输入的复数组进行快速傅里叶变换（FFT）
输入参数：*xin复数结构体组的首地址指针，struct型
*****************************************************************/
void FFT(struct compx *xin, int data_num);

#ifdef __cplusplus
}
#endif

#endif
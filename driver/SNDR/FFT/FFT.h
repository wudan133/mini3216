#ifndef    __FFT_H__
#define    __FFT_H__

#include<math.h>


#define PI 3.1415926535897932384626433832795028841971               //����Բ����ֵ

struct compx {float real,imag;};                                    //����һ�������ṹ

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************
����ԭ�ͣ�void FFT(struct compx *xin,int N)
�������ܣ�������ĸ�������п��ٸ���Ҷ�任��FFT��
���������*xin�����ṹ������׵�ַָ�룬struct��
*****************************************************************/
void FFT(struct compx *xin, int data_num);

#ifdef __cplusplus
}
#endif

#endif
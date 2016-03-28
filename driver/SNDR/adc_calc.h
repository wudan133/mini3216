#ifndef _ADC_CALCULATOR1_H
#define _ADC_CALCULATOR1_H

#define LOG printf
//#define LOG

//how many DC point should be neglect
#define DC_neg_num  2

//how many point should be calculate as signal (sigle side)
#define sig_cal_num 1

typedef struct _T_ADC_SNDR_RESUL_T_
{
    double  d_sndr;
    double  d_enob;
}ADCSNDRRESULT_T;

#ifdef __cplusplus
extern "C" {
#endif

ADCSNDRRESULT_T adc_sndr(float *adc_data, int data_num);

#ifdef __cplusplus
}
#endif

#endif


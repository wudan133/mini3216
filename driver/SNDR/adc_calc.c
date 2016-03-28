#include "adc_calc.h"
#include <stdlib.h> // malloc, free
#include <memory.h>
#include "FFT.h"

ADCSNDRRESULT_T adc_sndr(float *adc_data, int data_num)
{
	ADCSNDRRESULT_T adc_sndr_resul;
	float *fft_data_o;
    float *fft_data;
	struct compx *s;
	struct compx buff;
	
	double sig_p;
	int sig_num;
	double sig_p_tot=0;
	double nd1=0,nd2=0;
	int i =0;
	int a,b,c,d;
	

	fft_data_o = ( float* )malloc( sizeof(float) * data_num/2 );
	fft_data   = ( float* )malloc( sizeof(float) * data_num/2 );
	s = ( struct compx* )malloc( sizeof(struct compx) * data_num);

	memset(fft_data_o, 0, data_num/2 );
	memset(fft_data, 0, data_num/2 );

	for (i=0;i < data_num;i++)
	{
		s[i].real = (float)adc_data[i];
		s[i].imag = 0;
		//LOG("s[%d].real = %f\r\n", i, s[i].real);
	}
	
	//FFT calculate
    FFT(s, data_num);
	
	for (i=0;i < data_num/2; i++)
	{
		buff.real = (s[i].real <0)? -s[i].real : s[i].real;
		buff.imag = (s[i].imag <0)? -s[i].imag : s[i].imag;
		fft_data_o[i] = (buff.real + buff.imag);
		//LOG("fft_data_o[%d] = %f\r\n", i, fft_data_o[i]);
	}

	//LOG(" ---------------- start -------------------- \r\n");
	
	//i = sizeof(fft_data_o)/sizeof(double);
	//LOG("i = %d\r\n",i);
	
	for(i=0;i<data_num/2;i++)                           
    {
		fft_data[i] = (float)(20* log10(fft_data_o[i]));
		//LOG("fft_data[%d] = %f\r\n", i, fft_data[i]);
    }
	
    //find the signal pot
    sig_p = fft_data[DC_neg_num-1];
	sig_num = DC_neg_num;
    for(i=DC_neg_num;i<data_num/2-1;i++)
    {
        if (fft_data[i] > sig_p)
        {
            sig_p = fft_data[i];
            sig_num = i;
			//LOG("sig_num = %d\r\n",sig_num);
        }
    }
	
	
	
    //signal power
    a = sig_num - sig_cal_num;
	b = sig_num + sig_cal_num;
    for(i=a;i<=b;i++)
    {
        sig_p_tot += pow(fft_data_o[i],2);
    }
	//LOG("\r\n");
	//LOG("sig_p_tot = %f\r\n",sig_p_tot);
	
	
    //noise and harmonic power
    a = DC_neg_num;
    b = sig_num-sig_cal_num-1;
	c = sig_num+sig_cal_num+1;
    d = data_num/2 - 1;
	
    for(i=a;i<=b;i++)
    {
        nd1 += pow(fft_data_o[i],2);
    }
	//LOG("nd1 = %f\r\n",nd1);
    for(i=c;i<=d;i++)
    {
        nd2 += pow(fft_data_o[i],2);
    }
	//LOG("nd2 = %f\r\n",nd2);
	
	
    //calculate SNDR and ENOB
    adc_sndr_resul.d_sndr = 10*log10(sig_p_tot/(nd1+nd2));
    adc_sndr_resul.d_enob = (adc_sndr_resul.d_sndr - 1.76)/6.02;

	//free( p_fft_data_o ); 
	//free( fft_data ); 

	return adc_sndr_resul;
}


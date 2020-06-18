#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
								  
////////////////////////////////////////////////////////////////////////////////// 

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times);
u16 Get_Adc_fft(u8 ch);
//void Fft_Value_Updata(void);
//#define NPT 64;
u16 dataprocessing(u16 *combuffer,u8 comlen);
#endif 

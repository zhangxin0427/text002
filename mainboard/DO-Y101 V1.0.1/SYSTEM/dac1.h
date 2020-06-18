#ifndef _DAC1_H
#define _DAC1_H	 
#include "sys.h"	    
//#include "stm32f10x_dma.h"
#define PI        3.1415926
#define DAC1_SIZE        50 	       //�������ݳ���,һ�����ڵĵ�
extern u16 DAC1_SIN_BUF[DAC1_SIZE];  //�������ݻ�����

#define DAC1_DMA            1        //0�ر�DMA 1����DMA
 
#if DAC1_DMA==1
	#define DAC1_TIME         5    //3us����һ����
	//����=DAC1_SIZE*DAC1_TIME=100*10=1ms 1K(DAC1_TIMEΪ10)������Ϊ3.3KHZ,timeΪ3 
	void DAC1_TIM3_Init(u16 arr,u16 psc);
#endif

void Dac1_Test(void);
void Dac1_Creat_Buf(void);
void Dac1_Init(void);
void Dac1_Set_Vol(u16 vol);
void Dac1_turn_off(void);
#endif

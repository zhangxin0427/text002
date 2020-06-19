#include "bsp.h"

/********************************************
*��������IWDG_Init
*������������ʼ���������Ź�
*���������(unsigned char)prer����Ƶ��(0~7)	(unsigned int)rlr����װ��ֵ(0~4095)
*�����������
*����ֵ����
*����˵����ʱ����㣺Tout = ((4*2^prer)*rlr)/40(ms)
********************************************/
void IWDG_Init(unsigned char prer,unsigned int rlr)
{
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(prer);
	IWDG_SetReload(rlr);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

/********************************************
*��������IWDG_Feed
*����������ι�������Ź�
*�����������
*�����������
*����ֵ����
*����˵������
********************************************/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

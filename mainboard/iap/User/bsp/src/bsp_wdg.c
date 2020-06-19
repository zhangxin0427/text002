#include "bsp.h"

/********************************************
*函数名：IWDG_Init
*功能描述：初始化独立看门狗
*输入参数：(unsigned char)prer：分频数(0~7)	(unsigned int)rlr：重装载值(0~4095)
*输出参数：无
*返回值：无
*其他说明：时间计算：Tout = ((4*2^prer)*rlr)/40(ms)
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
*函数名：IWDG_Feed
*功能描述：喂独立看门狗
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：无
********************************************/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

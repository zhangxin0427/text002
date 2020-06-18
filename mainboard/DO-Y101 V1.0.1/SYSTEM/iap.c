#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "iap.h"
//////////////////////////////////////////////////////////////////////////////////	 
//半双工485接口IAP BootLoader程序
//wangwang@ZYWL
//修改日期:2018.10.12
//版本：V1.0									  
////////////////////////////////////////////////////////////////////////////////// 	

iapfun jump2app; 

void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		USART1_SendTxData("\n\rBootloader!",15);
		//delay_ms(1000);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, DISABLE);
		TIM_Cmd(TIM3,DISABLE);
		SPI_Cmd(SPI1, DISABLE);
		TIM_Cmd(TIM2,DISABLE);
		USART_Cmd(USART1, DISABLE); 
		//IWDG_Feed();
		jump2app=(iapfun)*(vu32*)(appxaddr+4);				//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					      		//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									              		//跳转到APP.
	}
}		 















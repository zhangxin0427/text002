#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "iap.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��˫��485�ӿ�IAP BootLoader����
//wangwang@ZYWL
//�޸�����:2018.10.12
//�汾��V1.0									  
////////////////////////////////////////////////////////////////////////////////// 	

iapfun jump2app; 

void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		USART1_SendTxData("\n\rBootloader!",15);
		//delay_ms(1000);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, DISABLE);
		TIM_Cmd(TIM3,DISABLE);
		SPI_Cmd(SPI1, DISABLE);
		TIM_Cmd(TIM2,DISABLE);
		USART_Cmd(USART1, DISABLE); 
		//IWDG_Feed();
		jump2app=(iapfun)*(vu32*)(appxaddr+4);				//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					      		//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									              		//��ת��APP.
	}
}		 















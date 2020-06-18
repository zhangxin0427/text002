#include "stm32f10x_gpio.h"
#include "delay.h"
void power_sup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	       //??GPIOA??
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //??GPIOB??
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	       //??GPIOC??
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	       //??GPIOD??
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	       //??GPIOA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//???GPIOA
	GPIO_SetBits(GPIOA,GPIO_Pin_3); 
//	GPIO_ResetBits(GPIOA,GPIO_Pin_3);

}

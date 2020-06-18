#include "stm32f10x_gpio.h"
#include "delay.h"

void LTC2450_Config(void)
  {
		GPIO_InitTypeDef GPIO_InitStructure;
    //SCK
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	
    GPIO_Init(GPIOC, &GPIO_InitStructure);
   
    //CS 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

     //SDO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }


float LTC2450_Read(void)
{
	u16 cnt;
	u16 SPIData;
	float RES1;
	for(cnt=0;cnt<16;cnt++)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);/*10,11°åÓÃ*/
		GPIO_ResetBits(GPIOC,GPIO_Pin_5);
		
		//GPIO_ResetBits(GPIOC,GPIO_Pin_5);
		//GPIO_ResetBits(GPIOC,GPIO_Pin_6);
		
		delay_us(1);
		SPIData <<= 1;
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))
		{
			SPIData|=0x01;
			
		}
		GPIO_SetBits(GPIOC,GPIO_Pin_5);
		//GPIO_SetBits(GPIOC,GPIO_Pin_6);
		delay_us(1);
	}
	  GPIO_SetBits(GPIOC,GPIO_Pin_6);
	//GPIO_SetBits(GPIOC,GPIO_Pin_5);
	
	RES1 = (float)(SPIData)*20000/(65535-(float)(SPIData));
	
	return (RES1);
}


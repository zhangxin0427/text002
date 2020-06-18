#include "gprs.h" 
//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 	 
u8 DATA_send_flag=0;    //1  代表传递为命令指令   0  代表为透传数据
u8 CGQ_val[32][4];      // 32传感器数据 
u8 power_flag=0;


void RS3485_Init(void)     //rs485_re  低电平接收，高电平发送
{   
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能GPIOC时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA
	
  GPIO_ResetBits(GPIOC,GPIO_Pin_12);  
}





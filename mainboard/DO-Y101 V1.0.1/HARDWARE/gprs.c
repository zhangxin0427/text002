#include "gprs.h" 
//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 	 
u8 DATA_send_flag=0;    //1  ������Ϊ����ָ��   0  ����Ϊ͸������
u8 CGQ_val[32][4];      // 32���������� 
u8 power_flag=0;


void RS3485_Init(void)     //rs485_re  �͵�ƽ���գ��ߵ�ƽ����
{   
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA
	
  GPIO_ResetBits(GPIOC,GPIO_Pin_12);  
}





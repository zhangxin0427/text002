#include "sys.h"
#include "usart.h"
#include "crc.h"
#include "timer.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 

//�ض���fputc���� 
/*int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}*/
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
extern struct uart_data_buff uart1_buff;
extern u8 gs_u8_Addr485; 
extern u8 USART1RxFlag;

/*********************����1����**********************************/
void uart1_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

  //USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10 

  //485ʹ�ܽ����� PA8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
	
  GPIO_ResetBits(GPIOA,GPIO_Pin_8); //485ʹ�ܽ�����Ϊ�ͣ�����ģʽ

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;  //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;       //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;          //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	 //�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure);     //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}


#if EN_USART1_RX   		//���ʹ���˽���
//���ջ����� 	
#define USART1_RX_BUFFER_SIZE 128
#define USART1_RX_BUFFER_MASK (USART1_RX_BUFFER_SIZE - 1) 	//���ջ���,���128���ֽ�.

static u8 USART1_RxBuf[USART1_RX_BUFFER_SIZE];
static u8 USART1_RxHead = 0;
static u8 USART1_RxTail = 0;
static u8 FRAMEHead_FLAG = 0;

void USART1_IRQHandler(void)//����1���������жϷ������
{
	u8  tmpRxData = 0;
	u8  tmpRxHead = 0;
	u16 temp = 0;
	u8  crc_datah, crc_datal;
	
	//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	//��ֹUSART�ж�

	#if SYSTEM_SUPPORT_OS 					//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
  #endif	
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	 //��ȡ�жϱ�־
	{	
		tmpRxData = USART_ReceiveData(USART1);             //��ȡ��������
				
		//03ָ��֡ͷ�ж�
		if ((FRAMEHead_FLAG == 0) 	 && 
				(tmpRxData 			== 0x03) && 
				(USART1_RxHead 	>  0) 	 && 
				(USART1_RxBuf[USART1_RxHead-1] == gs_u8_Addr485))    //֡ͷ:01 03
		{
				USART1_RxBuf[0] = gs_u8_Addr485;
				USART1_RxHead		=	1;
				FRAMEHead_FLAG 	= 1;
		}
		
		//06ָ��֡ͷ�ж�
		if ((FRAMEHead_FLAG == 0) 	 &&
				(tmpRxData 			== 0x06) && 
				(USART1_RxHead 	>  0) 	 && 
				(USART1_RxBuf[USART1_RxHead-1] == gs_u8_Addr485))    //֡ͷ:01 06
		{
				USART1_RxBuf[0] = gs_u8_Addr485;
				USART1_RxHead		=	1;
			  FRAMEHead_FLAG 	= 1;
		}

		tmpRxHead = USART1_RxHead & USART1_RX_BUFFER_MASK;
		USART1_RxBuf[tmpRxHead] = tmpRxData;
		
		USART1_RxHead = tmpRxHead + 1; 
		
		if ((USART1_RxHead > 2))
		{
			//CRCУ��֡����
			temp = CRC16((u8 *)USART1_RxBuf, (USART1_RxHead-2));
			crc_datal = (u8)(temp 		 & 0x00FF);
			crc_datah = (u8)((temp>>8) & 0x00FF);
			
			if ((USART1_RxBuf[USART1_RxHead-2] == crc_datah)&&(USART1_RxBuf[USART1_RxHead-1] == crc_datal))
			{	
				//CRCУ����ȷ���ж�������ϣ��ñ�־λ
						USART1RxFlag 	 = 1;
						FRAMEHead_FLAG = 0;
			}
			else 
			{
				
						USART1RxFlag 	 = 0;
						FRAMEHead_FLAG = 0;
			}	
		}	
 
		//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			  //ʹ��USART�ж�
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); 		  	//���RXNE��־
		
	}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}

//*
u8 USART1_ReadRxData(u8 *ReadData)
{
	u8 tmpRxTail = 0;
	u8 RxDataLen = 0;
  			
  while(USART1_RxHead != USART1_RxTail)
	{
		tmpRxTail = USART1_RxTail & USART1_RX_BUFFER_MASK;	//������ջ���index
		USART1_RxTail = tmpRxTail +1; 				              //store new index
		
		ReadData[RxDataLen++] = USART1_RxBuf[tmpRxTail]; 	  //Copy���յ�������
	}
	USART1_RxHead=0;
	USART1_RxTail=0;
	uart1_buff.rx_len = RxDataLen;		
  	
  return RxDataLen;
}
//*/
#endif  //#if EN_USART1_RX   		//���ʹ���˽���   	 

u8 USART1_SendTxData(u8 *SendData, u8 DataLen)
{	
	u8 i;
	
	USART1->SR;  //USART_GetFlagStatus(USART1, USART_FLAG_TC); //�����һ���ַ�����ʧ�ܵ�����
	RS485_Se1();

	for(i = 0; i < DataLen; i++)
	{
		USART_SendData(USART1, SendData[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);		
	}
	RS485_Re1();
	
	return i;
}


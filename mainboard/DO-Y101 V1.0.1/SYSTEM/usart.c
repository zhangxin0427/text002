#include "sys.h"
#include "usart.h"
#include "crc.h"
#include "timer.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 

//重定义fputc函数 
/*int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}*/
#endif 

/*使用microLib的方法*/
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

/*********************串口1设置**********************************/
void uart1_init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

  //USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10 

  //485使能脚设置 PA8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;              
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA
	
  GPIO_ResetBits(GPIOA,GPIO_Pin_8); //485使能脚配置为低，接收模式

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;  //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器
  
   //USART初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;       //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;          //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	 //收发模式

  USART_Init(USART1, &USART_InitStructure);     //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
}


#if EN_USART1_RX   		//如果使能了接收
//接收缓存区 	
#define USART1_RX_BUFFER_SIZE 128
#define USART1_RX_BUFFER_MASK (USART1_RX_BUFFER_SIZE - 1) 	//接收缓冲,最大128个字节.

static u8 USART1_RxBuf[USART1_RX_BUFFER_SIZE];
static u8 USART1_RxHead = 0;
static u8 USART1_RxTail = 0;
static u8 FRAMEHead_FLAG = 0;

void USART1_IRQHandler(void)//串口1接收数据中断服务程序
{
	u8  tmpRxData = 0;
	u8  tmpRxHead = 0;
	u16 temp = 0;
	u8  crc_datah, crc_datal;
	
	//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	//禁止USART中断

	#if SYSTEM_SUPPORT_OS 					//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
  #endif	
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	 //获取中断标志
	{	
		tmpRxData = USART_ReceiveData(USART1);             //获取串口数据
				
		//03指令帧头判断
		if ((FRAMEHead_FLAG == 0) 	 && 
				(tmpRxData 			== 0x03) && 
				(USART1_RxHead 	>  0) 	 && 
				(USART1_RxBuf[USART1_RxHead-1] == gs_u8_Addr485))    //帧头:01 03
		{
				USART1_RxBuf[0] = gs_u8_Addr485;
				USART1_RxHead		=	1;
				FRAMEHead_FLAG 	= 1;
		}
		
		//06指令帧头判断
		if ((FRAMEHead_FLAG == 0) 	 &&
				(tmpRxData 			== 0x06) && 
				(USART1_RxHead 	>  0) 	 && 
				(USART1_RxBuf[USART1_RxHead-1] == gs_u8_Addr485))    //帧头:01 06
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
			//CRC校验帧数据
			temp = CRC16((u8 *)USART1_RxBuf, (USART1_RxHead-2));
			crc_datal = (u8)(temp 		 & 0x00FF);
			crc_datah = (u8)((temp>>8) & 0x00FF);
			
			if ((USART1_RxBuf[USART1_RxHead-2] == crc_datah)&&(USART1_RxBuf[USART1_RxHead-1] == crc_datal))
			{	
				//CRC校验正确，判定接收完毕，置标志位
						USART1RxFlag 	 = 1;
						FRAMEHead_FLAG = 0;
			}
			else 
			{
				
						USART1RxFlag 	 = 0;
						FRAMEHead_FLAG = 0;
			}	
		}	
 
		//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			  //使能USART中断
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); 		  	//清掉RXNE标志
		
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
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
		tmpRxTail = USART1_RxTail & USART1_RX_BUFFER_MASK;	//计算接收缓存index
		USART1_RxTail = tmpRxTail +1; 				              //store new index
		
		ReadData[RxDataLen++] = USART1_RxBuf[tmpRxTail]; 	  //Copy接收到的数据
	}
	USART1_RxHead=0;
	USART1_RxTail=0;
	uart1_buff.rx_len = RxDataLen;		
  	
  return RxDataLen;
}
//*/
#endif  //#if EN_USART1_RX   		//如果使能了接收   	 

u8 USART1_SendTxData(u8 *SendData, u8 DataLen)
{	
	u8 i;
	
	USART1->SR;  //USART_GetFlagStatus(USART1, USART_FLAG_TC); //解决第一个字符发送失败的问题
	RS485_Se1();

	for(i = 0; i < DataLen; i++)
	{
		USART_SendData(USART1, SendData[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);		
	}
	RS485_Re1();
	
	return i;
}


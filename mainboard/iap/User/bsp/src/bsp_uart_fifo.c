#include "bsp.h"

/* 定义每个串口结构体变量 */

uint16_t RXDBUF[UART1_READ_BUF_SIZE]={0}; //接收缓存区
uint16_t TXDBUF[UART1_WRITE_BUF_SIZE]={0};	//发送缓存区

/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{

	InitHardUart();		/* 配置串口的硬件参数(波特率等) */

	RS485_InitTXE();	/* 配置RS485芯片的发送使能硬件，配置为推挽输出 */

	ConfigUartNVIC();	/* 配置串口中断 */
	
	RS485_SendBefor(); //初始化485设置为发送模式
}


/* 如果是RS485通信，请按如下格式编写函数， 我们仅举了 USART3作为RS485的例子 */

/*
*********************************************************************************************************
*	函 数 名: RS485_InitTXE
*	功能说明: 配置RS485发送使能口线 TXE
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_InitTXE(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_RS485_TXEN, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
	GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBefor
*	功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
	RS485_TX_EN();	/* 切换RS485收发芯片为发送模式 */
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendOver
*	功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
	RS485_RX_EN();	/* 切换RS485收发芯片为接收模式 */
}




/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;


		/* 第1步：打开GPIO和USART部件的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_DeInit(USART1);					  	//复位USART1
	
	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */

	USART_Cmd(USART1, ENABLE);		/* 使能串口 */
	USART_GetFlagStatus(USART1, USART_FLAG_TC);		//解决第一个字节无法发送的问题，详情看官方RM0033文档的TC跟TXE 标志位
	USART1_DMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)TXDBUF);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);


}

/*
*********************************************************************************************************
*	函 数 名: ConfigUartNVIC
*	功能说明: 配置串口中断优先级
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

/********************************************
*函数名：USART1_DMA_Config
*功能描述：USART DMA配置
*输入参数：DMA_CHx:DMA通道，cpar:外设地址,cmar:存储器地址 
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void USART1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	bsp_DelayUS(5);
	DMA_DeInit(DMA_CHx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;				//设定外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;					//设定存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;				//数据传输方向从存储器到外设
	DMA_InitStructure.DMA_BufferSize = 64;						//DMA4通道缓存大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//存储器地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 	//外设数据宽度为HalfWord
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 		//存储器数据宽度为HalfWord
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;				//通道具有中优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//非内存到内存模式
	DMA_Init(DMA_CHx,&DMA_InitStructure);
	
}

/********************************************
*函数名：USART_DMA_Enable
*功能描述：开启一次DMA传输
*输入参数：DMA_CHx:DMA通道，len:数据长度 
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void USART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len)
{
	DMA_Cmd(DMA_CHx,DISABLE);					//关闭DMA通道
	DMA_SetCurrDataCounter(DMA_CHx,len);				//配置数据传输量
	DMA_Cmd(DMA_CHx,ENABLE);					//开启DMA通道
}



/********************************************
*函数名：Send_Cmd
*功能描述：发送命令到灯具
*输入参数：(u16)length：数据长度
*输出参数：无
*返回值：无
*其他说明：
********************************************/

void Send_Lamp_Cmd(u16 length)
{
			GPIOA->CRH &= 0xFFFFFF0F;				//配置PA9为推挽输出模式，输出Break信号
			GPIOA->CRH |= 0x00000030;
			GPIOA->BRR = GPIO_Pin_9;
			bsp_DelayUS(100);
			GPIOA->BSRR = GPIO_Pin_9;
			bsp_DelayUS(5);
			GPIOA->CRH &= 0xFFFFFF0F;				//恢复PA9为复用推挽输出模式
			GPIOA->CRH |= 0x000000B0;
			bsp_DelayUS(5);

			USART_DMA_Enable(DMA1_Channel4,length);
			bsp_DelayMS(5);
	
			while(DMA_GetFlagStatus(DMA1_FLAG_TC4) != SET);		//判断数据是否发送完毕
			DMA_ClearFlag(DMA1_FLAG_TC4);				//清除标志位
}


/*
*********************************************************************************************************
*	函 数 名: USART_BufferSend
*	功能说明: 
*	形    参: USARTx: 端口号
*			  		pBuffer: 待发送的数据缓冲区
*			  		NumByteToSend : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void USART_BufferSend(USART_TypeDef* USARTx, u8* pBuffer, u16 NumByteToSend)
{
	u16 temp;
    while(NumByteToSend--)
    {
//				RS485_SendBefor();							//发送前485设置为发送模式
//			  bsp_DelayUS(1200);												//延时1.2ms
        temp = (u16)(*pBuffer)&0x1FF;	                              //一个字节，16bit，2B
        USART_SendData(USARTx,temp);								  //选择发送端口
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);  //等待发送结束，结束后标志位清0
        pBuffer++;													  
    }
//				RS485_SendOver();  											//发送完成需要把485设置为接收模式
//		    bsp_DelayUS(1200);												//延时1.2ms
}




/*
*********************************************************************************************************
*	函 数 名: USART_BufferSend9Bit
*	功能说明: 这个函数是为发9bit数据准备的，USART_BufferSend是发的8bit,两个函数形参类型不一样的
*	形    参: USARTx: 端口号(COM1 - COM6)
*			  		pBuffer: 待发送的数据缓冲区
*			  		NumByteToSend : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void USART_BufferSend9Bit(USART_TypeDef* USARTx, u16* pBuffer, u16 NumByteToSend)
{
	u16 temp;
    while(NumByteToSend--)
    {
//				RS485_SendBefor();							//发送前485设置为发送模式
//			  bsp_DelayUS(1200);												//延时1.2ms
        temp = (u16)(*pBuffer)&0x1FF;	                              //一个字节，16bit，2B
        USART_SendData(USARTx,temp);								  //选择发送端口
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);  //等待发送结束，结束后标志位清0
        pBuffer++;													  
    }
//				RS485_SendOver();  											//发送完成需要把485设置为接收模式
//		    bsp_DelayUS(1200);												//延时1.2ms
}


/*
*********************************************************************************************************
*	函 数 名: USART_SendByte
*	功能说明: 串口发送函数
*	形    参: USARTx : 串口设备
*			  		data : 存放读取数据的指针
*	返 回 值: 无
*********************************************************************************************************
*/
void USART_SendByte(USART_TypeDef* USARTx,u16 data)
{
//   RS485_SendBefor();												//发送之前需要把485设置为发送模式
//	 bsp_DelayUS(1200);												//延时1.2ms
   USART_SendData(USARTx,data);
   while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);//等待发送结束，结束后标志位清0
//	 RS485_SendOver();  											//发送完成需要把485设置为接收模式
//	 bsp_DelayUS(1200);												//延时1.2ms
}


/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	形    参: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t countdata=0;	//接收缓存区位数
uint8_t getdataok=0;	//接收到数据标志位
static void Uart1IRQ(void)
{
		
	/* 处理接收中断  */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;
		
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);/*关闭通信*/
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);		  //清除标志
		
		ch = (u8)USART_ReceiveData(USART1);				  			//接收数据
		RXDBUF[0]=ch;
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);/*打开通信*/
		
//		if(RXDBUF[0]==0xfe)
//		{
//			getdataok=1;
//		
//		}
		
		

	
	}
}

/*
*********************************************************************************************************
*	函 数 名: USART1_IRQHandler
*	功能说明: USART中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void USART1_IRQHandler(void)
{
	
	Uart1IRQ();
	
}



/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{

	/* 写一个字节到USART1 */
	USART_SendData(USART1, (uint8_t) ch);
	/* 等待发送结束 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	return ch;

}

/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART1);

}





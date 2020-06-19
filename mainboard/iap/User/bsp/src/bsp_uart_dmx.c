#include "bsp.h"

/* ����ÿ�����ڽṹ����� */

uint16_t Dmx_RXDBUF[UART2_READ_BUF_SIZE]={0}; //���ջ�����
uint16_t Dmx_TXDBUF[UART2_WRITE_BUF_SIZE]={0};	//���ͻ�����

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitUart
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_bsp_InitUart(void)
{

	Dmx_InitHardUart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */

	Dmx_RS485_InitTXE();	/* ����RS485оƬ�ķ���ʹ��Ӳ��������Ϊ������� */

	Dmx_ConfigUartNVIC();	/* ���ô����ж� */
	
	Dmx_RS485_SendBefor(); //��ʼ��485����Ϊ����ģʽ
}


/* �����RS485ͨ�ţ��밴���¸�ʽ��д������ ���ǽ����� USART3��ΪRS485������ */

/*
*********************************************************************************************************
*	�� �� ��: RS485_InitTXE
*	����˵��: ����RS485����ʹ�ܿ��� TXE
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_RS485_InitTXE(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(Dmx_RCC_RS485_TXEN, ENABLE);	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */
	GPIO_InitStructure.GPIO_Pin = Dmx_PIN_RS485_TXEN;
	GPIO_Init(Dmx_PORT_RS485_TXEN, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBefor
*	����˵��: ��������ǰ��׼������������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendBefor = RS485_SendBefor
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_RS485_SendBefor(void)
{
	Dmx_RS485_TX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendOver
*	����˵��: ����һ�����ݽ�������ƺ�������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendOver = RS485_SendOver
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_RS485_SendOver(void)
{
	Dmx_RS485_RX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}




/*
*********************************************************************************************************
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Dmx_InitHardUart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;


		/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_DeInit(USART2);					  	//��λUSART2
	
	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART2_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */

	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */
	USART_GetFlagStatus(USART2, USART_FLAG_TC);		//�����һ���ֽ��޷����͵����⣬���鿴�ٷ�RM0033�ĵ���TC��TXE ��־λ
	Dmx_USART2_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)Dmx_TXDBUF);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);


}

/*
*********************************************************************************************************
*	�� �� ��: ConfigUartNVIC
*	����˵��: ���ô����ж����ȼ�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Dmx_ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

/********************************************
*��������Dmx_USART2_DMA_Config
*����������USART DMA����
*���������DMA_CHx:DMAͨ����cpar:�����ַ,cmar:�洢����ַ 
*�����������
*����ֵ����
*����˵����
********************************************/
void Dmx_USART2_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	bsp_DelayUS(5);
	DMA_DeInit(DMA_CHx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;				//�趨�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;					//�趨�洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;				//���ݴ��䷽��Ӵ洢��������
	DMA_InitStructure.DMA_BufferSize = 64;						//DMA4ͨ�������С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//�洢����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 	//�������ݿ��ΪHalfWord
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 		//�洢�����ݿ��ΪHalfWord
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;				//ͨ�����и����ȼ����ȴ�ӡ��ͨ����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;					//���ڴ浽�ڴ�ģʽ
	DMA_Init(DMA_CHx,&DMA_InitStructure);
	
}

/********************************************
*��������USART_DMA_Enable
*��������������һ��DMA����
*���������DMA_CHx:DMAͨ����len:���ݳ��� 
*�����������
*����ֵ����
*����˵����
********************************************/
void Dmx_USART2_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len)
{
	DMA_Cmd(DMA_CHx,DISABLE);					//�ر�DMAͨ��
	DMA_SetCurrDataCounter(DMA_CHx,len);				//�������ݴ�����
	DMA_Cmd(DMA_CHx,ENABLE);					//����DMAͨ��
}



/********************************************
*��������Send_Cmd
*������������������ƾ�
*���������(u16)length�����ݳ���
*�����������
*����ֵ����
*����˵����
********************************************/

void Dmx_Send_Lamp_Cmd(u16 length)
{
			GPIOA->CRL &= 0xFFFFF0FF;				//����PA9Ϊ�������ģʽ�����Break�ź�
			GPIOA->CRL |= 0x00000300;
			GPIOA->BRR = GPIO_Pin_2;
			bsp_DelayUS(100);
			GPIOA->BSRR = GPIO_Pin_2;
			bsp_DelayUS(5);
			GPIOA->CRL &= 0xFFFFF0FF;				//�ָ�PA9Ϊ�����������ģʽ
			GPIOA->CRL |= 0x00000B00;
			bsp_DelayUS(5);

			USART_DMA_Enable(DMA1_Channel7,length);
			bsp_DelayMS(5);
	
			while(DMA_GetFlagStatus(DMA1_FLAG_TC7) != SET);		//�ж������Ƿ������
			DMA_ClearFlag(DMA1_FLAG_TC7);				//�����־λ
			
}


/*
*********************************************************************************************************
*	�� �� ��: USART_BufferSend
*	����˵��: 
*	��    ��: USARTx: �˿ں�
*			  		pBuffer: �����͵����ݻ�����
*			  		NumByteToSend : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_USART_BufferSend(USART_TypeDef* USARTx, u8* pBuffer, u8 NumByteToSend)
{
	u16 temp;
    while(NumByteToSend--)
    {
//				RS485_SendBefor();							//����ǰ485����Ϊ����ģʽ
//			  bsp_DelayUS(1200);												//��ʱ1.2ms
        temp = (u16)(*pBuffer)&0x1FF;	                              //һ���ֽڣ�16bit��2B
        USART_SendData(USARTx,temp);								  //ѡ���Ͷ˿�
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);  //�ȴ����ͽ������������־λ��0
        pBuffer++;													  
    }
//				RS485_SendOver();  											//���������Ҫ��485����Ϊ����ģʽ
//		    bsp_DelayUS(1200);												//��ʱ1.2ms
}




/*
*********************************************************************************************************
*	�� �� ��: USART_BufferSend9Bit
*	����˵��: ���������Ϊ��9bit����׼���ģ�USART_BufferSend�Ƿ���8bit,���������β����Ͳ�һ����
*	��    ��: USARTx: �˿ں�(COM1 - COM6)
*			  		pBuffer: �����͵����ݻ�����
*			  		NumByteToSend : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_USART_BufferSend9Bit(USART_TypeDef* USARTx, u16* pBuffer, u16 NumByteToSend)
{
	u16 temp;
    while(NumByteToSend--)
    {
//				RS485_SendBefor();							//����ǰ485����Ϊ����ģʽ
//			  bsp_DelayUS(1200);												//��ʱ1.2ms
        temp = (u16)(*pBuffer)&0x1FF;	                              //һ���ֽڣ�16bit��2B
        USART_SendData(USARTx,temp);								  //ѡ���Ͷ˿�
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);  //�ȴ����ͽ������������־λ��0
        pBuffer++;													  
    }
//				RS485_SendOver();  											//���������Ҫ��485����Ϊ����ģʽ
//		    bsp_DelayUS(1200);												//��ʱ1.2ms
}


/*
*********************************************************************************************************
*	�� �� ��: USART_SendByte
*	����˵��: ���ڷ��ͺ���
*	��    ��: USARTx : �����豸
*			  		data : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Dmx_USART_SendByte(USART_TypeDef* USARTx,u16 data)
{
//   RS485_SendBefor();												//����֮ǰ��Ҫ��485����Ϊ����ģʽ
//	 bsp_DelayUS(1200);												//��ʱ1.2ms
   USART_SendData(USARTx,data);
   while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);//�ȴ����ͽ������������־λ��0
//	 RS485_SendOver();  											//���������Ҫ��485����Ϊ����ģʽ
//	 bsp_DelayUS(1200);												//��ʱ1.2ms
}


/*
*********************************************************************************************************
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Dmx_countdata=0;	//���ջ�����λ��
uint8_t Dmx_getdataok=0;	//���յ����ݱ�־λ
static void Dmx_Uart2IRQ(void)
{
		
	/* ��������ж�  */
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;
		
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);/*�ر�ͨ��*/
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);		  //�����־
		
		ch = (u8)USART_ReceiveData(USART2);				  			//��������
		Dmx_RXDBUF[0]=ch;
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);/*��ͨ��*/
		
//		if(RXDBUF[0]==0xfe)
//		{
//			Dmx_getdataok=1;
//		
//		}
		
		

	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: USART2_IRQHandler
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void USART2_IRQHandler(void)
{
	
	Dmx_Uart2IRQ();
	
}





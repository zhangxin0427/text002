/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f10x.h"



/* RS485оƬ����ʹ��GPIO, PA11 */
#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOA
#define PORT_RS485_TXEN  GPIOA
#define PIN_RS485_TXEN	 GPIO_Pin_11

#define RS485_RX_EN()	PORT_RS485_TXEN->BRR = PIN_RS485_TXEN
#define RS485_TX_EN()	PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN



#define UART1_BAUD			115200
#define	UART1_WRITE_BUF_SIZE  10	
#define	UART1_READ_BUF_SIZE  10



/* �����豸�ṹ�� */

void bsp_InitUart(void);
void RS485_InitTXE(void);
void RS485_SendOver(void);
void RS485_SendBefor(void);
static void InitHardUart(void);
void USART_SendByte(USART_TypeDef* USARTx,u16 data);
void USART_BufferSend(USART_TypeDef* USARTx, u8* pBuffer, u16 NumByteToSend);
void USART_BufferSend9Bit(USART_TypeDef* USARTx, u16* pBuffer, u16 NumByteToSend);
void USART1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
void USART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len);
void Send_Lamp_Cmd(u16 length);
static void Uart1IRQ(void);
static void ConfigUartNVIC(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

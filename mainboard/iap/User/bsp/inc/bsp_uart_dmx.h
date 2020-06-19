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

#ifndef _BSP_USART_DMX_H_
#define _BSP_USART_DMX_H_

#include "stm32f10x.h"



/* RS485оƬ����ʹ��GPIO, PG10 */
#define Dmx_RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOG
#define Dmx_PORT_RS485_TXEN  GPIOG
#define Dmx_PIN_RS485_TXEN	 GPIO_Pin_10

#define Dmx_RS485_RX_EN()	Dmx_PORT_RS485_TXEN->BRR = Dmx_PIN_RS485_TXEN
#define Dmx_RS485_TX_EN()	Dmx_PORT_RS485_TXEN->BSRR = Dmx_PIN_RS485_TXEN



#define UART2_BAUD			250000
#define	UART2_WRITE_BUF_SIZE  1634	
#define	UART2_READ_BUF_SIZE  10


extern uint16_t Dmx_RXDBUF[UART2_READ_BUF_SIZE];
extern uint16_t Dmx_TXDBUF[UART2_WRITE_BUF_SIZE];


/* �����豸�ṹ�� */

void Dmx_bsp_InitUart(void);
void Dmx_RS485_InitTXE(void);
void Dmx_RS485_SendOver(void);
void Dmx_RS485_SendBefor(void);
static void Dmx_InitHardUart(void);
void Dmx_USART_SendByte(USART_TypeDef* USARTx,u16 data);
void Dmx_USART_BufferSend(USART_TypeDef* USARTx, u8* pBuffer, u8 NumByteToSend);
void Dmx_USART_BufferSend9Bit(USART_TypeDef* USARTx, u16* pBuffer, u16 NumByteToSend);
void Dmx_USART2_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
void Dmx_USART2_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len);
void Dmx_Send_Lamp_Cmd(u16 length);
static void Dmx_Uart1IRQ(void);
static void Dmx_ConfigUartNVIC(void);

#endif



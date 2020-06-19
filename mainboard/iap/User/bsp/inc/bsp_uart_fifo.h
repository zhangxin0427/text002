/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f10x.h"



/* RS485芯片发送使能GPIO, PA11 */
#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOA
#define PORT_RS485_TXEN  GPIOA
#define PIN_RS485_TXEN	 GPIO_Pin_11

#define RS485_RX_EN()	PORT_RS485_TXEN->BRR = PIN_RS485_TXEN
#define RS485_TX_EN()	PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN



#define UART1_BAUD			115200
#define	UART1_WRITE_BUF_SIZE  10	
#define	UART1_READ_BUF_SIZE  10



/* 串口设备结构体 */

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

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

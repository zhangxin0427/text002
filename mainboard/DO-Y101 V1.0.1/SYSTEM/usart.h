#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h"  
#include "timer.h"


//////////////////////////////////////////////////////////////////////////////////	 
#define USART_RX_BUFFER_SIZE 128
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)

#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
//#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������2����

//#define	RS485_Re()		GPIO_ResetBits(GPIOA,GPIO_Pin_1)		//PA1
//#define	RS485_Se()		GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define	RS485_Re1()		GPIO_ResetBits(GPIOA,GPIO_Pin_8)		//PA8
#define	RS485_Se1()		GPIO_SetBits(GPIOA,GPIO_Pin_8)


struct uart_data_buff {
		u8  rx_data_buff[USART_RX_BUFFER_SIZE];
		u8  tx_data_buff[USART_RX_BUFFER_SIZE];
		u8  rx_len;
		u8  tx_len;
                      };

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart1_init(u32 bound);
//void uart2_init(u32 bound);

u8 USART1_ReadRxData(u8 *ReadData);
//u8 USART2_ReadRxData(u8 *ReadData);
u8 USART1_SendTxData(u8 *SendData, u8 DataLen);
//u8 USART2_SendTxData1(u8 *SendData, u8 DataLen);

#endif



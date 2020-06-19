
#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "bsp.h"



#define RCC_W5500_RST 	 RCC_APB2Periph_GPIOC
#define PORT_W5500_RST  GPIOC
#define PIN_W5500_RST   GPIO_Pin_0

#define W5500_RESET()	  PORT_W5500_RST->BRR = PIN_W5500_RST
#define W5500_SETBIT()	PORT_W5500_RST->BSRR = PIN_W5500_RST

void SPI1_Configuration(void);					//SPI1���ú���
void SPI1_WriteByte(u8 TxData);					//SPI1дһ�ֽ�
u8 SPI1_ReadByte(void);						//SPI1��һ�ֽ�
void SPI1_CrisEnter(void);					//SPI1�����ٽ���
void SPI1_CrisExit(void);					//SPI1�˳��ٽ���
void SPI1_CS_Select(void);					//SPI1Ƭѡʹ��
void SPI1_CS_Deselect(void);					//SPI1Ƭѡʧ��

#endif

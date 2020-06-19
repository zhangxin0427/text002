
#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "bsp.h"



#define RCC_W5500_RST 	 RCC_APB2Periph_GPIOC
#define PORT_W5500_RST  GPIOC
#define PIN_W5500_RST   GPIO_Pin_0

#define W5500_RESET()	  PORT_W5500_RST->BRR = PIN_W5500_RST
#define W5500_SETBIT()	PORT_W5500_RST->BSRR = PIN_W5500_RST

void SPI1_Configuration(void);					//SPI1配置函数
void SPI1_WriteByte(u8 TxData);					//SPI1写一字节
u8 SPI1_ReadByte(void);						//SPI1读一字节
void SPI1_CrisEnter(void);					//SPI1进入临界区
void SPI1_CrisExit(void);					//SPI1退出临界区
void SPI1_CS_Select(void);					//SPI1片选使能
void SPI1_CS_Deselect(void);					//SPI1片选失能

#endif

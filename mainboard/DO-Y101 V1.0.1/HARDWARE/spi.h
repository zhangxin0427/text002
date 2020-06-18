#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "stdio.h"	
#include "oled.h"
#include "gprs.h" 
#include "crc.h"
#include "usart.h"	
//////////////////////////////////////////////////////////////////////////////////	 
								  
//////////////////////////////////////////////////////////////////////////////////

#define	SPI_CS 		PBout(12)  		//W25QXX的片选信号

extern u8 SPI2_step;
extern u8 SPI_RX_BUF[128];
extern u8 SPI_TX_BUF[128];
extern u8 spi_delaytime;
 				  	    													  
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void SPI2_WriteNBytes(u8* pBuffer,u16 NumByteToWrite);   //SPI2写入NByte的数据
void SPI2_WR_COM(void);            //发送获取传感器数据或者校准传感器指令
void SPI2_ReadNBytes(void);       //SPI2读取NByte的数据
void spi_key_wr(void);
		 
#endif


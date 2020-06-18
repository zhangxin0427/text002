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

#define	SPI_CS 		PBout(12)  		//W25QXX��Ƭѡ�ź�

extern u8 SPI2_step;
extern u8 SPI_RX_BUF[128];
extern u8 SPI_TX_BUF[128];
extern u8 spi_delaytime;
 				  	    													  
void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
void SPI2_WriteNBytes(u8* pBuffer,u16 NumByteToWrite);   //SPI2д��NByte������
void SPI2_WR_COM(void);            //���ͻ�ȡ���������ݻ���У׼������ָ��
void SPI2_ReadNBytes(void);       //SPI2��ȡNByte������
void spi_key_wr(void);
		 
#endif


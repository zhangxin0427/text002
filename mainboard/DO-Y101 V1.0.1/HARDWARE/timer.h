#ifndef __TIMER_H
#define __TIMER_H	
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "crc.h"

//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 	

extern u16 time_min;
extern u8  time_sec;
extern u8  time_ms;
extern u16 Frequency_value;

#define STM32_FLASH_SIZE 64 	 	      	//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	  //STM32 FLASH����ʼ��ַ
//FLASH������ֵ

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM2_IRQHandler(void);

//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr);

//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������(16λ����)
void Test_Write(u32 WriteAddr,u16 WriteData);

//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);

//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���)
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);

#endif


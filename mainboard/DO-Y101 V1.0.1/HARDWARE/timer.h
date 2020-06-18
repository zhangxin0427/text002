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

#define STM32_FLASH_SIZE 64 	 	      	//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	  //STM32 FLASH的起始地址
//FLASH解锁键值

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM2_IRQHandler(void);

//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr);

//WriteAddr:起始地址
//WriteData:要写入的数据(16位数据)
void Test_Write(u32 WriteAddr,u16 WriteData);

//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);

//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数)
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);

#endif


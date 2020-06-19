#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H
#include "bsp_sys.h" 

#define		STM32_FLASH_BASE	0x08000000			//STM32 FLASH的起始地址
#define		STM32_FLASH_SAVE_ADDR	0x08030000			//设置FLASH保存地址

struct EPRM
{	
	u16 DsplMenu;				//当前菜单
	u16 DsplSelect_2;			//二级菜单选项			
	u16 DsplSelect_3;			//三级菜单选项
	u16 DsplSelect_4;			//四级菜单选项
	u16 Chip;				//芯片类型
	u16 ID;					//本机ID
	u16 NetSeg[3];				//网段
	u16 FrameSpeed;				//帧速
	u16 DataSequence;			//数据排列顺序
	u16 FirstPowerOn;			//初次上电标志
	u16 WiFiMode;				//WiFi模式
	u16 PixelNum;				//像素点数
	u16 Status;				//系统状态
};

u16 STM32_FLASH_ReadHalfWord(u32 faddr);					//读取指定地址的半字
void STM32_FLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);		//从指定地址开始读取指定长度的数据
void STM32_FLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STM32_FLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);	//不检查的写入指定长度的数据
void STM32_FLASH_EPRM_READ(struct EPRM *pEP);					//读取保存的EPRM参数
void STM32_FLASH_EPRM_WRITE(struct EPRM *pEP);					//写入EPRM参数到FLASH中
void FlashDataWrite(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
void FlashDataRead(u32 ReadAddr, u16 *pBuffer,u16 NumToRead);	//从指定地址开始读取指定长度的数据

#endif

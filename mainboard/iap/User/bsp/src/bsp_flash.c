#include "bsp.h"

#define		STM32_SECTOR_SIZE	2048

/********************************************
*函数名：STM32_FLASH_ReadHalfWord
*功能描述：读取指定地址的字节（16位数据）
*输入参数：(u32)faddr：读地址（此地址要为2的倍数）
*输出参数：无
*返回值：对应的数据
*其他说明：
********************************************/
u16 STM32_FLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr;
}

/********************************************
*函数名：STM32_FLASH_Read
*功能描述：从指定地址开始读取指定长度的数据
*输入参数：(u32)ReadAddr：起始地址	(u16*)pBuffer：数据指针		(u16)NumToRead：半字数
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void STM32_FLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
	u16 i;
	for(i = 0;i < NumToRead;i++)
	{
		pBuffer[i] = STM32_FLASH_ReadHalfWord(ReadAddr);	//读取1个字节
		ReadAddr += 2;						//偏移1个字节
	}
}

/********************************************
*函数名：STM32_FLASH_Write
*功能描述：从指定地址开始写入指定长度的数据
*输入参数：(u32)WriteAddr：起始地址	(u16*)pBuffer：数据指针		(u16)NumToWrite：半字数
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void STM32_FLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{	
	u32 Sector_Pos;										//扇区地址0~127
	u32 Offset_Addr;									//去掉0x08000000后的地址
	if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * 256)))	//非法地址
		return;
	FLASH_Unlock();										//Flash解锁
	Offset_Addr = WriteAddr - STM32_FLASH_BASE;						//实际偏移地址
	Sector_Pos = Offset_Addr/STM32_SECTOR_SIZE;						//扇区地址0~127
	FLASH_ErasePage(Sector_Pos*STM32_SECTOR_SIZE + STM32_FLASH_BASE);			//擦除这个扇区
	STM32_FLASH_Write_NoCheck(WriteAddr,pBuffer,NumToWrite);				//写入扇区
	FLASH_Lock();										//Flash上锁		
}

/********************************************
*函数名：STM32_FLASH_Write_NoCheck
*功能描述：不检查的写入指定长度的数据
*输入参数：(u32)WriteAddr：起始地址	(u16*)pBuffer：数据指针		(u16)NumToWrite：半字数
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void STM32_FLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u16 i;
	
	for(i = 0;i < NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
		WriteAddr += 2;
		
	}
}
/********************************************
*函数名：STM32_FLASH_EPRM_READ
*功能描述：读取保存的EPRM参数
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void STM32_FLASH_EPRM_READ(struct EPRM *pEP)
{
	u16 temp[14];
	STM32_FLASH_Read(STM32_FLASH_SAVE_ADDR,temp,14);
	pEP->DsplMenu = (temp[0] == 0xFFFF)?1:temp[0];
	pEP->DsplSelect_2 = (temp[1] == 0xFFFF)?1:temp[1];
	pEP->DsplSelect_3 = (temp[2] == 0xFFFF)?1:temp[2];
	pEP->DsplSelect_4 = (temp[3] == 0xFFFF)?0:temp[3];
	pEP->Chip = (temp[4] == 0xFFFF)?1:temp[4];
	pEP->ID = (temp[5] == 0xFFFF)?1:temp[5];
	pEP->FrameSpeed = (temp[6] == 0xFFFF)?25:temp[6];
	pEP->DataSequence = (temp[7] == 0xFFFF)?1:temp[7];
	pEP->FirstPowerOn = (temp[8] == 0xFFFF)?1:temp[8];
	pEP->WiFiMode = (temp[9] == 0xFFFF)?1:temp[9];
	pEP->PixelNum = (temp[10] == 0xFFFF)?170:temp[10];
	pEP->NetSeg[0] = (temp[11] == 0xFFFF)?192:temp[11];
	pEP->NetSeg[1] = (temp[12] == 0xFFFF)?168:temp[12];
	pEP->NetSeg[2] = (temp[13] == 0xFFFF)?1:temp[13];
	
}

/********************************************
*函数名：STM32_FLASH_EPRM_WRITE
*功能描述：写入EPRM参数到FLASH中
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void STM32_FLASH_EPRM_WRITE(struct EPRM *pEP)
{
	u16 temp[14];
	temp[0] = pEP->DsplMenu;
	temp[1] = pEP->DsplSelect_2;
	temp[2] = pEP->DsplSelect_3;
	temp[3] = pEP->DsplSelect_4;
	temp[4] = pEP->Chip;
	temp[5] = pEP->ID;
	temp[6] = pEP->FrameSpeed;
	temp[7] = pEP->DataSequence;
	temp[8] = pEP->FirstPowerOn;
	temp[9]	= pEP->WiFiMode;
	temp[10] = pEP->PixelNum;
	temp[11] = pEP->NetSeg[0];
	temp[12] = pEP->NetSeg[1];
	temp[13] = pEP->NetSeg[2];
	STM32_FLASH_Write(STM32_FLASH_SAVE_ADDR,temp,14);
}

/********************************************
*函数名：FlashDataWrite
*功能描述：从指定地址开始写入指定长度的数据
*输入参数：(u32)WriteAddr：起始地址	(u16*)pBuffer：数据指针		(u16)NumToWrite：半字数
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void FlashDataWrite(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
	
	STM32_FLASH_Write(WriteAddr, pBuffer, NumToWrite);

}

/********************************************
*函数名：FlashDataRead
*功能描述：从指定地址开始读取指定长度的数据
*输入参数：(u32)ReadAddr：起始地址	(u16*)pBuffer：数据指针		(u16)NumToRead：半字数
*输出参数：无
*返回值：无
*其他说明：
********************************************/

void FlashDataRead(u32 ReadAddr, u16 *pBuffer,u16 NumToRead)
{
	
	STM32_FLASH_Read(ReadAddr, pBuffer, NumToRead);

}

#include "bsp.h"

#define		STM32_SECTOR_SIZE	2048

/********************************************
*��������STM32_FLASH_ReadHalfWord
*������������ȡָ����ַ���ֽڣ�16λ���ݣ�
*���������(u32)faddr������ַ���˵�ַҪΪ2�ı�����
*�����������
*����ֵ����Ӧ������
*����˵����
********************************************/
u16 STM32_FLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr;
}

/********************************************
*��������STM32_FLASH_Read
*������������ָ����ַ��ʼ��ȡָ�����ȵ�����
*���������(u32)ReadAddr����ʼ��ַ	(u16*)pBuffer������ָ��		(u16)NumToRead��������
*�����������
*����ֵ����
*����˵����
********************************************/
void STM32_FLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
	u16 i;
	for(i = 0;i < NumToRead;i++)
	{
		pBuffer[i] = STM32_FLASH_ReadHalfWord(ReadAddr);	//��ȡ1���ֽ�
		ReadAddr += 2;						//ƫ��1���ֽ�
	}
}

/********************************************
*��������STM32_FLASH_Write
*������������ָ����ַ��ʼд��ָ�����ȵ�����
*���������(u32)WriteAddr����ʼ��ַ	(u16*)pBuffer������ָ��		(u16)NumToWrite��������
*�����������
*����ֵ����
*����˵����
********************************************/
void STM32_FLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{	
	u32 Sector_Pos;										//������ַ0~127
	u32 Offset_Addr;									//ȥ��0x08000000��ĵ�ַ
	if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * 256)))	//�Ƿ���ַ
		return;
	FLASH_Unlock();										//Flash����
	Offset_Addr = WriteAddr - STM32_FLASH_BASE;						//ʵ��ƫ�Ƶ�ַ
	Sector_Pos = Offset_Addr/STM32_SECTOR_SIZE;						//������ַ0~127
	FLASH_ErasePage(Sector_Pos*STM32_SECTOR_SIZE + STM32_FLASH_BASE);			//�����������
	STM32_FLASH_Write_NoCheck(WriteAddr,pBuffer,NumToWrite);				//д������
	FLASH_Lock();										//Flash����		
}

/********************************************
*��������STM32_FLASH_Write_NoCheck
*����������������д��ָ�����ȵ�����
*���������(u32)WriteAddr����ʼ��ַ	(u16*)pBuffer������ָ��		(u16)NumToWrite��������
*�����������
*����ֵ����
*����˵����
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
*��������STM32_FLASH_EPRM_READ
*������������ȡ�����EPRM����
*�����������
*�����������
*����ֵ����
*����˵����
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
*��������STM32_FLASH_EPRM_WRITE
*����������д��EPRM������FLASH��
*�����������
*�����������
*����ֵ����
*����˵����
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
*��������FlashDataWrite
*������������ָ����ַ��ʼд��ָ�����ȵ�����
*���������(u32)WriteAddr����ʼ��ַ	(u16*)pBuffer������ָ��		(u16)NumToWrite��������
*�����������
*����ֵ����
*����˵����
********************************************/
void FlashDataWrite(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
	
	STM32_FLASH_Write(WriteAddr, pBuffer, NumToWrite);

}

/********************************************
*��������FlashDataRead
*������������ָ����ַ��ʼ��ȡָ�����ȵ�����
*���������(u32)ReadAddr����ʼ��ַ	(u16*)pBuffer������ָ��		(u16)NumToRead��������
*�����������
*����ֵ����
*����˵����
********************************************/

void FlashDataRead(u32 ReadAddr, u16 *pBuffer,u16 NumToRead)
{
	
	STM32_FLASH_Read(ReadAddr, pBuffer, NumToRead);

}

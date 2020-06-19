#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H
#include "bsp_sys.h" 

#define		STM32_FLASH_BASE	0x08000000			//STM32 FLASH����ʼ��ַ
#define		STM32_FLASH_SAVE_ADDR	0x08030000			//����FLASH�����ַ

struct EPRM
{	
	u16 DsplMenu;				//��ǰ�˵�
	u16 DsplSelect_2;			//�����˵�ѡ��			
	u16 DsplSelect_3;			//�����˵�ѡ��
	u16 DsplSelect_4;			//�ļ��˵�ѡ��
	u16 Chip;				//оƬ����
	u16 ID;					//����ID
	u16 NetSeg[3];				//����
	u16 FrameSpeed;				//֡��
	u16 DataSequence;			//��������˳��
	u16 FirstPowerOn;			//�����ϵ��־
	u16 WiFiMode;				//WiFiģʽ
	u16 PixelNum;				//���ص���
	u16 Status;				//ϵͳ״̬
};

u16 STM32_FLASH_ReadHalfWord(u32 faddr);					//��ȡָ����ַ�İ���
void STM32_FLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);		//��ָ����ַ��ʼ��ȡָ�����ȵ�����
void STM32_FLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STM32_FLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);	//������д��ָ�����ȵ�����
void STM32_FLASH_EPRM_READ(struct EPRM *pEP);					//��ȡ�����EPRM����
void STM32_FLASH_EPRM_WRITE(struct EPRM *pEP);					//д��EPRM������FLASH��
void FlashDataWrite(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
void FlashDataRead(u32 ReadAddr, u16 *pBuffer,u16 NumToRead);	//��ָ����ַ��ʼ��ȡָ�����ȵ�����

#endif

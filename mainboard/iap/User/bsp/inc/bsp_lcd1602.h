
#ifndef __BSP_LCD1602_H
#define __BSP_LCD1602_H	 

#include "stm32f10x.h"
#include "bsp_ds3231.h"
#include "bsp_flash.h"

//LCD�˿ڶ���
#define 	LCD_RW_H 	PDout(10) = 1
#define 	LCD_RW_L	PDout(10) = 0 
#define 	LCD_RS_H	PDout(9) = 1
#define 	LCD_RS_L	PDout(9) = 0
#define 	LCD_EN_H	PEout(15) = 1
#define 	LCD_EN_L	PEout(15) = 0
#define 	LCD_DB0		PDout(8)
#define 	LCD_DB1		PEout(13)
#define 	LCD_DB2		PEout(14)
#define 	LCD_DB3		PEout(11)
#define 	LCD_DB4		PEout(12)
#define 	LCD_DB5		PGout(0)
#define 	LCD_DB6		PGout(1)
#define 	LCD_DB7		PFout(14)
#define 	LCD_OFF		PFout(15) = 1
#define 	LCD_ON		PFout(15) = 0

#define		System_Set	1
#define		SDCard_Check	2
#define		UDisk_Check	3
#define		Default_Play	4
#define		Other_Function	5
#define		UDisk_Play	7
#define 	SDCard_Play	8

#define 	Write_Address	1
#define		Test_Address	2

#define		All_Loop	1
#define		Single_Loop	2
#define		Single_Play	3

#define		SDCard_Init	 1
#define		SDCard_Complete	 2
#define		SDCard_Error	 3
#define		SDCard_File_Null 4

#define		UDisk_Init	 1
#define		UDisk_Complete	 2
#define		UDisk_Error	 3
#define		UDisk_File_Null  4

#define		LampInitAddress	 1
#define		LampNum		 2
#define		ChannelNum	 3
#define		StartAddress	 4

#define		_R		1
#define		_G		2
#define		_B		3
#define		_RGB		4
#define		_Flow		5
#define		_Black		6
#define		_Gradient	7

#define		ID_Set		1
#define		Chip_Set	2
#define		Frame_Speed	3
#define		Data_Sequence	4
#define		Time_Set	5
#define		WiFi_Set	6
#define		Config_Read	7
#define		Pixel_Set	8
#define		NetSeg_Set	9
#define		System_Init	10

#define		DMX1200		1
#define		MBI6020		2
#define		MBI6021		3
#define		MBI6023		4
#define		MBI6024		5
#define		MBI6030		6
#define		MBI6033		7

#define		LCD_RGB		1
#define		LCD_RBG		2
#define		LCD_GRB		3
#define		LCD_GBR		4
#define		LCD_BRG		5
#define		LCD_BGR		6
#define		LCD_RGBN		7
#define		LCD_RBGN		8
#define		LCD_GRBN		9
#define		LCD_GBRN		10
#define		LCD_BRGN		11
#define		LCD_BGRN		12

#define		WiFi_AP		1
#define		WiFi_STA	2

#define		Address_do	1
#define		Address_Error	2
#define		Address_Success	3

#define		Test_do		1
#define		Test_Error	2
#define		Test_Success	3
struct DSPL
{	
	u16 DsplMenu;					//��ǰ�˵�
	u16 DsplSelect_2;				//�����˵�ѡ��
	u16 DsplSelect_3;				//�����˵�ѡ��
	u16 DsplSelect_4;				//�ļ��˵�ѡ��
	u16 DsplSelect_5;				//�弶�˵�ѡ��
	u16 Chip;					//оƬ����
	u16 ID;						//����ID
	u16 NetSeg[3];					//����
	u16 BitCnt;					//��ǰ�޸�λ
	u16 DsplRefreshTime;				//��ʾˢ��ʱ�����
	u16 LedFlashTime;				//LED��˸��ʱ����
	u16 FrameSpeed;					//֡��
	u16 DataSequence;				//��������˳��
	u16 DsplSwitchTime;				//�����л�ʱ�����
	u16 WiFiMode;					//WiFIģʽ
	u8 FileName[50][20];				//���SD����Ŀ�ļ���
	u16 FileNum;					//��Ŀ�ļ�����
	u16 PixelNum;					//������ص�
	u16 _LampInitAddress;				//�ƾ���ʼ��ַ
	u16 _AddressLampNum;				//дַ�ƾ���
	u16  _ChannelNum;				//ͨ����
};

void LCD_GPIO_Init(void);							//LCD�˿ڳ�ʼ��
void LCD_Write_Cmd(unsigned char Command);					//д��LCD����
void LCD_Write_Data(unsigned char Data);					//д��LCD����
void LCD_Init(void);								//LCD��ʼ��
void LCD_Clear(void);								//LCD����
void LCD_Write_Str( char *p);				//д���ַ���
void LCD_Set_Addr(unsigned char n);						//�����ַ�����ַ
void LCD_Disp(struct DSPL *pDS,struct EPRM *pEP,RTC_Param *DS_RTC,RTC_Param *SDS_RTC);	//LCD��ʾ

#endif

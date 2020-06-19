
#ifndef __BSP_LCD1602_H
#define __BSP_LCD1602_H	 

#include "stm32f10x.h"
#include "bsp_ds3231.h"
#include "bsp_flash.h"

//LCD端口定义
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
	u16 DsplMenu;					//当前菜单
	u16 DsplSelect_2;				//二级菜单选项
	u16 DsplSelect_3;				//三级菜单选项
	u16 DsplSelect_4;				//四级菜单选项
	u16 DsplSelect_5;				//五级菜单选项
	u16 Chip;					//芯片类型
	u16 ID;						//本机ID
	u16 NetSeg[3];					//网段
	u16 BitCnt;					//当前修改位
	u16 DsplRefreshTime;				//显示刷新时间计数
	u16 LedFlashTime;				//LED闪烁定时计数
	u16 FrameSpeed;					//帧速
	u16 DataSequence;				//数据排列顺序
	u16 DsplSwitchTime;				//画面切换时间计数
	u16 WiFiMode;					//WiFI模式
	u8 FileName[50][20];				//存放SD卡节目文件名
	u16 FileNum;					//节目文件总数
	u16 PixelNum;					//输出像素点
	u16 _LampInitAddress;				//灯具起始地址
	u16 _AddressLampNum;				//写址灯具数
	u16  _ChannelNum;				//通道数
};

void LCD_GPIO_Init(void);							//LCD端口初始化
void LCD_Write_Cmd(unsigned char Command);					//写入LCD命令
void LCD_Write_Data(unsigned char Data);					//写入LCD数据
void LCD_Init(void);								//LCD初始化
void LCD_Clear(void);								//LCD清屏
void LCD_Write_Str( char *p);				//写入字符串
void LCD_Set_Addr(unsigned char n);						//设置字符串地址
void LCD_Disp(struct DSPL *pDS,struct EPRM *pEP,RTC_Param *DS_RTC,RTC_Param *SDS_RTC);	//LCD显示

#endif

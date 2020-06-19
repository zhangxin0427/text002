#ifndef __BSP_DS3231_H
#define __BSP_DS3231_H

#include "bsp_sys.h"
#include "mix.h"

#define DS3231_WriteAdd 	0xD0		//写模式
#define DS3231_ReadAdd 		0xD1		//读模式
#define DS3231_Second		0x00		//秒
#define DS3231_Minute		0x01		//分
#define DS3231_Hour		0x02		//时
#define DS3231_Week		0x03		//星期
#define DS3231_Day		0x04		//日
#define DS3231_Month		0x05		//月
#define DS3231_Year		0x06		//年
//闹铃1            
#define DS3231_ALARM1ECOND 	0x07    	//秒
#define DS3231_ALARM1MINUTE 	0x08    	//分
#define DS3231_ALARM1HOUR   	0x09    	//时
#define DS3231_ALARM1WEEK   	0x0A    	//星期/日
//闹铃2
#define DS3231_ALARM2MINUTE 	0x0b    	//分
#define DS3231_ALARM2HOUR   	0x0c    	//时
#define DS3231_ALARM2WEEK   	0x0d    	//星期/日
#define DS3231_CONTROL      	0x0e    	//控制寄存器
#define DS3231_STATUS       	0x0f    	//状态寄存器

#define DS3231_XTAL         	0x10    	//晶体老化补偿寄存器
#define DS3231_TEMPERATUREH 	0x11    	//温度寄存器高字节(高8位)
#define DS3231_TEMPERATUREL 	0x12    	//温度寄存器低字节(低2位)

#define BSY                 	2       	//忙
#define OSF                 	7       	//振荡器停止标志

#define DS3231_SCL_H		PBout(8) = 1	//SCL输出高电平
#define DS3231_SCL_L		PBout(8) = 0	//SCL输出低电平
#define DS3231_SDA_H		PBout(9) = 1	//SDA输出高电平
#define DS3231_SDA_L		PBout(9) = 0	//SDA输出低电平
#define DS3231_SDA_In		PBin(9)		//读取SDA电平


typedef struct{				//实时时钟参数结构体
	u8 Years;			//年
	u8 Months;			//月
	u8 Days;			//日
	u8 Weeks;			//星期
	u8 Hours;			//时
	u8 Minutes;			//分			
	u8 Seconds;			//秒
}RTC_Param;



void DS3231_GPIO_Init(void);				//DS3231端口初始化
void DS3231_SDA_Set_In(void);				//配置SDA端口为输入端口
void DS3231_SDA_Set_Out(void);				//配置SDA端口为输出端口
unsigned char BCD_TO_HEX(unsigned char BCD);		//BCD转换为HEX
unsigned char HEX_TO_BCD(unsigned char HEX);		//HEX转换为BCD
void Start_I2C(void);					//I2C总线启动信号
void Stop_I2C(void);					//I2C总线停止信号
void DS3231_SendByte(unsigned char data);		//DS3231字节发送
unsigned char DS3231_RcvByte(void);			//DS3231字节接收
void DS3231_I2C_ACK(unsigned char Ack_Flag);		//DS3231应答信号发送
unsigned char DS3231_SendData(unsigned char Addr,unsigned char Data);	//DS3231数据发送
unsigned char DS3231_RcvData(unsigned char Addr,unsigned char* Data);	//DS3231数据读取
unsigned char DS3231_Modify_Time(RTC_Param *DS_RTC);		//修改DS3231时间
unsigned char DS3231_Read_Time(RTC_Param *DS_RTC);
#endif

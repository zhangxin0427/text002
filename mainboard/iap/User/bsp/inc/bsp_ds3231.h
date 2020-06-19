#ifndef __BSP_DS3231_H
#define __BSP_DS3231_H

#include "bsp_sys.h"
#include "mix.h"

#define DS3231_WriteAdd 	0xD0		//дģʽ
#define DS3231_ReadAdd 		0xD1		//��ģʽ
#define DS3231_Second		0x00		//��
#define DS3231_Minute		0x01		//��
#define DS3231_Hour		0x02		//ʱ
#define DS3231_Week		0x03		//����
#define DS3231_Day		0x04		//��
#define DS3231_Month		0x05		//��
#define DS3231_Year		0x06		//��
//����1            
#define DS3231_ALARM1ECOND 	0x07    	//��
#define DS3231_ALARM1MINUTE 	0x08    	//��
#define DS3231_ALARM1HOUR   	0x09    	//ʱ
#define DS3231_ALARM1WEEK   	0x0A    	//����/��
//����2
#define DS3231_ALARM2MINUTE 	0x0b    	//��
#define DS3231_ALARM2HOUR   	0x0c    	//ʱ
#define DS3231_ALARM2WEEK   	0x0d    	//����/��
#define DS3231_CONTROL      	0x0e    	//���ƼĴ���
#define DS3231_STATUS       	0x0f    	//״̬�Ĵ���

#define DS3231_XTAL         	0x10    	//�����ϻ������Ĵ���
#define DS3231_TEMPERATUREH 	0x11    	//�¶ȼĴ������ֽ�(��8λ)
#define DS3231_TEMPERATUREL 	0x12    	//�¶ȼĴ������ֽ�(��2λ)

#define BSY                 	2       	//æ
#define OSF                 	7       	//����ֹͣ��־

#define DS3231_SCL_H		PBout(8) = 1	//SCL����ߵ�ƽ
#define DS3231_SCL_L		PBout(8) = 0	//SCL����͵�ƽ
#define DS3231_SDA_H		PBout(9) = 1	//SDA����ߵ�ƽ
#define DS3231_SDA_L		PBout(9) = 0	//SDA����͵�ƽ
#define DS3231_SDA_In		PBin(9)		//��ȡSDA��ƽ


typedef struct{				//ʵʱʱ�Ӳ����ṹ��
	u8 Years;			//��
	u8 Months;			//��
	u8 Days;			//��
	u8 Weeks;			//����
	u8 Hours;			//ʱ
	u8 Minutes;			//��			
	u8 Seconds;			//��
}RTC_Param;



void DS3231_GPIO_Init(void);				//DS3231�˿ڳ�ʼ��
void DS3231_SDA_Set_In(void);				//����SDA�˿�Ϊ����˿�
void DS3231_SDA_Set_Out(void);				//����SDA�˿�Ϊ����˿�
unsigned char BCD_TO_HEX(unsigned char BCD);		//BCDת��ΪHEX
unsigned char HEX_TO_BCD(unsigned char HEX);		//HEXת��ΪBCD
void Start_I2C(void);					//I2C���������ź�
void Stop_I2C(void);					//I2C����ֹͣ�ź�
void DS3231_SendByte(unsigned char data);		//DS3231�ֽڷ���
unsigned char DS3231_RcvByte(void);			//DS3231�ֽڽ���
void DS3231_I2C_ACK(unsigned char Ack_Flag);		//DS3231Ӧ���źŷ���
unsigned char DS3231_SendData(unsigned char Addr,unsigned char Data);	//DS3231���ݷ���
unsigned char DS3231_RcvData(unsigned char Addr,unsigned char* Data);	//DS3231���ݶ�ȡ
unsigned char DS3231_Modify_Time(RTC_Param *DS_RTC);		//�޸�DS3231ʱ��
unsigned char DS3231_Read_Time(RTC_Param *DS_RTC);
#endif

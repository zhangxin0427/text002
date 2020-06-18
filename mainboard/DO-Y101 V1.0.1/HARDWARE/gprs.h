#ifndef __GPRS_H
#define __GPRS_H	 
#include "sys.h" 
#include "delay.h"
#include "usart.h"	
#include "timer.h"
#include "string.h" 

//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 

//LED端口定义
#define BL100_Wakeup      PAout(6)	//    唤醒
#define BL100_Reset       PAout(7)	//    复位
#define RS3485_RE         PCout(12)	//    低电平接收，高电平发送

#define POWER_IN         PBin(14)	//    
#define POWER_OUT        PBout(13)	//    
#define PG_IN            PBin(12)	//    

extern u8  DATA_send_flag;    //0  代表传递为命令指令   1  代表为透传数据


void BL100_Init(void);//初始化	
void RS3485_Init(void);     //rs485_re  低电平接收，高电平发送
void power_on(void);
void power_off(void);
void RS485_SEND0neByte(uint8_t num);    //接收在线数据
void RS485_SEND(uint8_t *data , uint16_t num);
void  RS485_COM(void);           //在线式传感器
 
void EXTI_All_Init(void);                    //外部中断

#endif


















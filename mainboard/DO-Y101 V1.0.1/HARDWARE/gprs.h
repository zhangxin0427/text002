#ifndef __GPRS_H
#define __GPRS_H	 
#include "sys.h" 
#include "delay.h"
#include "usart.h"	
#include "timer.h"
#include "string.h" 

//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 

//LED�˿ڶ���
#define BL100_Wakeup      PAout(6)	//    ����
#define BL100_Reset       PAout(7)	//    ��λ
#define RS3485_RE         PCout(12)	//    �͵�ƽ���գ��ߵ�ƽ����

#define POWER_IN         PBin(14)	//    
#define POWER_OUT        PBout(13)	//    
#define PG_IN            PBin(12)	//    

extern u8  DATA_send_flag;    //0  ������Ϊ����ָ��   1  ����Ϊ͸������


void BL100_Init(void);//��ʼ��	
void RS3485_Init(void);     //rs485_re  �͵�ƽ���գ��ߵ�ƽ����
void power_on(void);
void power_off(void);
void RS485_SEND0neByte(uint8_t num);    //������������
void RS485_SEND(uint8_t *data , uint16_t num);
void  RS485_COM(void);           //����ʽ������
 
void EXTI_All_Init(void);                    //�ⲿ�ж�

#endif


















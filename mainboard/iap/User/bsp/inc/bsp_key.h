#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"

#define		KEY_ON	 	 0x00
#define		KEY_OFF	 	 0x01

#define		KSET_PRES	 0x01
#define		KUP_PRES	 0x02
#define		KDOWN_PRES 0x03
#define		KESC_PRES	 0x04


/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_InitKeyHard(void);
uint8_t bsp_KeyScan(void);

#endif



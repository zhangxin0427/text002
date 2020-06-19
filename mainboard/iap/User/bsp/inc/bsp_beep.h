#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "stm32f10x.h"


void bsp_InitBeep(void);
void bsp_BeepOn(uint8_t _no);
void bsp_BeepOff(uint8_t _no);



#endif



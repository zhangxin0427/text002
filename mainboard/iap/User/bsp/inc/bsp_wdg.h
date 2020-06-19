#ifndef __BSP_WDG_H
#define __BSP_WDG_H


void IWDG_Init(unsigned char prer,unsigned int rlr);		//初始化独立看门狗
void IWDG_Feed(void);						//喂独立看门狗

#endif	   

#include "bsp.h"


/* 按键口对应的RCC时钟 */
#define RCC_BEEP 	(RCC_APB2Periph_GPIOF)

#define GPIO_PORT_BEEP  GPIOF
#define GPIO_PIN_BEEP	GPIO_Pin_13


/*
*********************************************************************************************************
*	函 数 名: bsp_InitBeep
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitBeep(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_BEEP, ENABLE);


	bsp_BeepOff(1);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BEEP;
	GPIO_Init(GPIO_PORT_BEEP, &GPIO_InitStructure);


}


/*
*********************************************************************************************************
*	函 数 名: bsp_BeepOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_BeepOn(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_BEEP->BRR = GPIO_PIN_BEEP;
	}

}

/*
*********************************************************************************************************
*	函 数 名: bsp_BeepOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_BeepOff(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_BEEP->BSRR = GPIO_PIN_BEEP;
	}
}


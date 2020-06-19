#include "bsp.h"



/* 按键口对应的RCC时钟 */
#define RCC_ALL_KEY 	(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOF )

#define GPIO_PORT_K1    GPIOC
#define GPIO_PIN_K1	    GPIO_Pin_4

#define GPIO_PORT_K2    GPIOB
#define GPIO_PIN_K2	    GPIO_Pin_0

#define GPIO_PORT_K3    GPIOB
#define GPIO_PIN_K3	    GPIO_Pin_1

#define GPIO_PORT_K4    GPIOF
#define GPIO_PIN_K4	    GPIO_Pin_11




/*
*********************************************************************************************************
*	函 数 名: bsp_InitKey
*	功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyHard();		/* 初始化按键硬件 */
}



/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyHard
*	功能说明: 配置按键对应的GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 第1步：打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_KEY, ENABLE);

	/* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* 输入浮空模式 */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K1;
	GPIO_Init(GPIO_PORT_K1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K2;
	GPIO_Init(GPIO_PORT_K2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K3;
	GPIO_Init(GPIO_PORT_K3, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan
*	功能说明: 扫描所有按键。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_KeyScan(void)
{
	
	if(GPIO_ReadInputDataBit(GPIO_PORT_K1,GPIO_PIN_K1)==KEY_ON||GPIO_ReadInputDataBit(GPIO_PORT_K2,GPIO_PIN_K2)==KEY_ON|| \
		GPIO_ReadInputDataBit(GPIO_PORT_K3,GPIO_PIN_K3)==KEY_ON||GPIO_ReadInputDataBit(GPIO_PORT_K4,GPIO_PIN_K4)==KEY_ON	)
	{
		 bsp_DelayMS(20); 
		
//				bsp_BeepOn(1);
		
				if(GPIO_ReadInputDataBit(GPIO_PORT_K1,GPIO_PIN_K1)==KEY_ON)
				{
					while ( GPIO_ReadInputDataBit( GPIO_PORT_K1, GPIO_PIN_K1 ) == 0 );
					bsp_BeepOff(1);
					return KSET_PRES;
				}
				else 
				if(GPIO_ReadInputDataBit(GPIO_PORT_K2,GPIO_PIN_K2)==KEY_ON)
				{
					while ( GPIO_ReadInputDataBit( GPIO_PORT_K2, GPIO_PIN_K2 ) == 0 );
					bsp_BeepOff(1);
					return KUP_PRES;
				}
				if(GPIO_ReadInputDataBit(GPIO_PORT_K3,GPIO_PIN_K3)==KEY_ON)
				{
					while ( GPIO_ReadInputDataBit( GPIO_PORT_K3, GPIO_PIN_K3 ) == 0 );
					bsp_BeepOff(1);
					return KDOWN_PRES;
				}
				if(GPIO_ReadInputDataBit(GPIO_PORT_K4,GPIO_PIN_K4)==KEY_ON)
				{
					while ( GPIO_ReadInputDataBit( GPIO_PORT_K4, GPIO_PIN_K4 ) == 0 );
					bsp_BeepOff(1);
					return KESC_PRES;
				}
	}
	

	
	return 0;
	
	
	
}






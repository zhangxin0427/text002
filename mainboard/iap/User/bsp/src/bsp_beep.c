#include "bsp.h"


/* �����ڶ�Ӧ��RCCʱ�� */
#define RCC_BEEP 	(RCC_APB2Periph_GPIOF)

#define GPIO_PORT_BEEP  GPIOF
#define GPIO_PIN_BEEP	GPIO_Pin_13


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitBeep
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitBeep(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_BEEP, ENABLE);


	bsp_BeepOff(1);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BEEP;
	GPIO_Init(GPIO_PORT_BEEP, &GPIO_InitStructure);


}


/*
*********************************************************************************************************
*	�� �� ��: bsp_BeepOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
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
*	�� �� ��: bsp_BeepOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
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


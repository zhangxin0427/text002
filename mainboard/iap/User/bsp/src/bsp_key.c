#include "bsp.h"



/* �����ڶ�Ӧ��RCCʱ�� */
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
*	�� �� ��: bsp_InitKey
*	����˵��: ��ʼ������. �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyHard();		/* ��ʼ������Ӳ�� */
}



/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyHard
*	����˵��: ���ð�����Ӧ��GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��1������GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_ALL_KEY, ENABLE);

	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* ���븡��ģʽ */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K1;
	GPIO_Init(GPIO_PORT_K1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K2;
	GPIO_Init(GPIO_PORT_K2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_K3;
	GPIO_Init(GPIO_PORT_K3, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan
*	����˵��: ɨ�����а�����
*	��    ��:  ��
*	�� �� ֵ: ��
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






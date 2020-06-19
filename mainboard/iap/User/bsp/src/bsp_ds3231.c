#include "bsp.h"

u8 DS3231_ACK;								//Ӧ���źű�־

/********************************************
*��������DS3231_GPIO_Init
*����������DS3231�˿ڳ�ʼ��
*�����������
*�����������
*����ֵ����
*����˵����
	DS3231SCL------------PB8�����������
	DS3231SDA------------PB9�����������
	DS3231RST------------PE3�����������
	DS3231INT------------PE2��������������Ϊ�ⲿ�жϣ�
********************************************/
void DS3231_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/********************************************
*��������DS3231_SDA_Set_In
*��������������SDA�˿�Ϊ����˿�
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void DS3231_SDA_Set_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/********************************************
*��������DS3231_SDA_Set_Out
*��������������SDA�˿�Ϊ����˿�
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void DS3231_SDA_Set_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/********************************************
*��������EXTI2_IRQHandler
*�����������ж���2�жϴ�����
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

/********************************************
*��������BCD_TO_HEX
*����������BCDת��ΪHEX
*���������BCD:��ת����BCD��
*�����������
*����ֵ��HEX:ת�����HEX��
*����˵����
********************************************/
unsigned char BCD_TO_HEX(unsigned char BCD)
{
	unsigned char HEX;
	HEX = BCD&0x0F;
	BCD >>= 4;
	BCD &= 0x0F;
	BCD *= 10;
	HEX += BCD;
	return HEX;
}

/********************************************
*��������HEX_TO_BCD
*����������HEXת��ΪBCD
*���������HEX:��ת����HEX��
*�����������
*����ֵ��BCD:ת�����BCD��
*����˵����
********************************************/
unsigned char HEX_TO_BCD(unsigned char HEX)
{
	unsigned char BCD,i,j;
	i = HEX/10;
	j = HEX%10;
	BCD = j + (i<<4);
	return BCD;
}

/********************************************
*��������Start_I2C
*����������I2C���������ź�
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void Start_I2C(void)
{
	DS3231_SDA_H;
	bsp_DelayUS(5);
	DS3231_SCL_H;
	bsp_DelayUS(5);
	DS3231_SDA_L;
	bsp_DelayUS(5);
	DS3231_SCL_L;
	bsp_DelayUS(5);
}

/********************************************
*��������Stop_I2C
*����������I2C����ֹͣ�ź�
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void Stop_I2C(void)
{
	DS3231_SDA_L;
	bsp_DelayUS(5);
	DS3231_SCL_H;
	bsp_DelayUS(5);
	DS3231_SDA_H;
	bsp_DelayUS(5);
}

/********************************************
*��������DS3231_SendByte
*����������DS3231�ֽڷ���
*���������(unsigned char)data���贫�͵��ֽ�
*�����������
*����ֵ����
*����˵����
********************************************/
void DS3231_SendByte(unsigned char data)
{
	unsigned char BitCnt;
	for(BitCnt = 0;BitCnt < 8;BitCnt++)
	{
		if((data << BitCnt)&0x80)
			DS3231_SDA_H;			//�жϷ���λ
		else
			DS3231_SDA_L;
		bsp_DelayUS(1);
		DS3231_SCL_H;				//��ʱ����Ϊ�ߵ�ƽ��֪ͨ��������ʼ��������λ
		bsp_DelayUS(5);				//��֤ʱ�Ӹߵ�ƽ���ڴ���4us
		DS3231_SCL_L;
	}
	bsp_DelayUS(2);
	DS3231_SDA_H;					//��λ��������ͷ������ߣ�׼������Ӧ��
	DS3231_SDA_Set_In();				//����SDA�˿�Ϊ����˿�
	bsp_DelayUS(2);
	DS3231_SCL_H;
	bsp_DelayUS(3);
	if(DS3231_SDA_In == 1)				//�ж��Ƿ���յ�Ӧ���ź�
		DS3231_ACK = 0;				//��Ӧ���ź�
	else
		DS3231_ACK = 1;				//Ӧ���ź�
	DS3231_SCL_L;
	DS3231_SDA_Set_Out();				//�ָ�SDA�˿�Ϊ����˿�
	bsp_DelayUS(2);
}

/********************************************
*��������DS3231_RcvByte
*����������DS3231�ֽڽ���
*�����������
*���������(unsigned char)RcvByte�����յ����ֽ�
*����ֵ����
*����˵����
********************************************/
unsigned char DS3231_RcvByte(void)
{
	unsigned char RcvByte = 0;
	unsigned char BitCnt;
	
	DS3231_SDA_H;					//�ͷ�������
	DS3231_SDA_Set_In();				//����SDA�˿�Ϊ����˿�
	bsp_DelayUS(2);
	for(BitCnt = 0;BitCnt < 8;BitCnt++)
	{
		DS3231_SCL_L;				//��ʱ����Ϊ�ͣ�׼����������
		bsp_DelayUS(5);				//ʱ�ӵ͵�ƽ����Ҫ����4us
		DS3231_SCL_H;				//��ʱ����Ϊ�ߣ�ʹ�������ϵ�������Ч
		bsp_DelayUS(3);
		RcvByte = RcvByte << 1;
		if(DS3231_SDA_In == 1)			//������λ�����յ�����λ����RcvByte
			RcvByte += 1;
		bsp_DelayUS(5);
	}
	DS3231_SCL_L;					//�ͷ�ʱ����
	bsp_DelayUS(2);
	DS3231_SDA_Set_Out();				//�ָ�SDA�˿�Ϊ����˿�
	return RcvByte;
}

/********************************************
*��������DS3231_I2C_ACK
*����������DS3231Ӧ���źŷ���
*���������(unsigned char)Ack_Flag��Ӧ���źű�־
*�����������
*����ֵ����
*����˵����
********************************************/
void DS3231_I2C_ACK(unsigned char Ack_Flag)
{
	if(Ack_Flag == 0)
		DS3231_SDA_L;				//����Ӧ���ź�
	else
		DS3231_SDA_H;				//������Ӧ���ź�
	bsp_DelayUS(5);
	DS3231_SCL_H;
	bsp_DelayUS(5);
	DS3231_SCL_L;					//�ͷ�ʱ����
	bsp_DelayUS(2);
}

/********************************************
*��������DS3231_SendData
*����������DS3231���ݷ���
*���������(unsigned char)Addr��д���ַ	(unsigned char)Data��д������
*�����������
*����ֵ��0������ʧ��	1�����ͳɹ�
*����˵����
********************************************/
unsigned char DS3231_SendData(unsigned char Addr,unsigned char Data)
{
	Start_I2C();					//����I2C����
	DS3231_SendByte(DS3231_WriteAdd);		//дģʽ
	if(DS3231_ACK == 0)				//����Ƿ�Ӧ���źţ�����ʧ��
		return 0;
	DS3231_SendByte(Addr);				//д���ַ
	if(DS3231_ACK == 0)				//����Ƿ�Ӧ���źţ�����ʧ��
		return 0;
	DS3231_SendByte(Data);				//д������
	if(DS3231_ACK == 0)				//����Ƿ�Ӧ���źţ�����ʧ��
		return 0 ;
	Stop_I2C();					//ֹͣI2C����
	bsp_DelayUS(10);
	return 1;
}

/********************************************
*��������DS3231_RcvData
*����������DS3231���ݶ�ȡ
*���������(unsigned char)Addr����ȡ��ַ	(unsigned char*)Data����ȡ����	
*�����������
*����ֵ��0����ȡʧ��	1:��ȡ�ɹ�
*����˵����
********************************************/
unsigned char DS3231_RcvData(unsigned char Addr,unsigned char* Data)
{
	Start_I2C();					//����I2C����
	DS3231_SendByte(DS3231_WriteAdd);		//дģʽ
	if(DS3231_ACK == 0)				//����Ƿ�Ӧ���źţ�����ʧ��
		return 0;
	DS3231_SendByte(Addr);				//д���ȡ��ַ
	if(DS3231_ACK == 0)				//����Ƿ�Ӧ���źţ�����ʧ��
		return 0;
	
	Start_I2C();					//��������I2C����
	DS3231_SendByte(DS3231_ReadAdd);		//��ģʽ
	if(DS3231_ACK == 0)				//����Ƿ�Ӧ���źţ�����ʧ��
		return 0;
	*Data = DS3231_RcvByte();			//��ȡ����
	DS3231_I2C_ACK(1);				//��Ӧ���ź�
	Stop_I2C();					//ֹͣI2C����
	return 1;					//���سɹ�
}

/********************************************
*��������DS3231_Modify_Time
*�����������޸�DS3231ʱ��
*���������DS_RTC:ʱ������ṹ��
*�����������
*����ֵ��0���޸�ʧ��	1���޸ĳɹ�
*����˵����
********************************************/
unsigned char DS3231_Modify_Time(RTC_Param *DS_RTC)
{
	u8 temp = 0;
	
	temp = HEX_TO_BCD(DS_RTC->Years);	//�޸���
	if(DS3231_SendData(DS3231_Year,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Months);	//�޸���
	if(DS3231_SendData(DS3231_Month,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Days);		//�޸���
	if(DS3231_SendData(DS3231_Day,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Hours);	//�޸�ʱ
	if(DS3231_SendData(DS3231_Hour,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Minutes);	//�޸ķ�
	if(DS3231_SendData(DS3231_Minute,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Seconds);	//�޸���
	if(DS3231_SendData(DS3231_Second,temp) == 0)
		return 0;
	
	return 1;
}
/********************************************
*��������DS3231_Read_Time
*������������ȡDS3231ʱ��
*���������DS_RTC:ʱ������ṹ��
*�����������
*����ֵ��0����ȡʧ��	1����ȡ�ɹ�
*����˵����
********************************************/
unsigned char DS3231_Read_Time(RTC_Param *DS_RTC)
{
	u8 temp = 0;
	
	if(DS3231_RcvData(DS3231_Year,&temp) == 0)
		return 0;
	DS_RTC->Years = BCD_TO_HEX(temp);		//��ȡ��
	
	if(DS3231_RcvData(DS3231_Month,&temp) == 0)
		return 0;
	DS_RTC->Months = BCD_TO_HEX(temp);		//��ȡ��
	
	if(DS3231_RcvData(DS3231_Day,&temp) == 0)
		return 0;
	DS_RTC->Days = BCD_TO_HEX(temp);			//��ȡ��
	
	if(DS3231_RcvData(DS3231_Hour,&temp) == 0)
		return 0;
	temp &= 0x3F;					//24Сʱ��
	DS_RTC->Hours = BCD_TO_HEX(temp);		//��ȡʱ
	
	if(DS3231_RcvData(DS3231_Minute,&temp) == 0)
		return 0;
	DS_RTC->Minutes = BCD_TO_HEX(temp);		//��ȡ��
	
	if(DS3231_RcvData(DS3231_Second,&temp) == 0)
		return 0;
	DS_RTC->Seconds = BCD_TO_HEX(temp);		//��ȡ��
	
	return 1;
}

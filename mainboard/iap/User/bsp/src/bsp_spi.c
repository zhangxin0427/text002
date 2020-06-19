#include "bsp_spi.h"

/********************************************
*��������SPI1_RCC_Configuration
*����������ʹ��SPI1��GPIOAʱ��
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
static void SPI1_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOC,ENABLE);
}
/********************************************
*��������SPI1_GPIO_Configuration
*��������������SPI1����
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
static void SPI1_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*PA5->SCK,PA6->MISO,PA7->MOSI*/		 					 
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; 				//�����������
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	/*PA4->CS*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//�������
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	/*PC0->nRST*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//�������
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
}
/********************************************
*��������SPI1_Configuration
*��������������SPI1��ز���
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void SPI1_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI1_RCC_Configuration();						//ʱ������

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//Ԥ��Ƶ2��SPI1����36Mps
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;		//ȫ˫��
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;				//����ģʽ
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;				//��λ����λ
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;					//����״̬�͵�ƽ
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;				//�ӵ�һ��ʱ���ؿ�ʼ����
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;					//�ڲ�NSS�ź���SSIλ����NSS���ſ����������ã�
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;				//MSBλ����
	SPI_InitStruct.SPI_CRCPolynomial = 7;					//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1,&SPI_InitStruct);
	
	SPI1_GPIO_Configuration();						//SPI1��������

	SPI_SSOutputCmd(SPI1, ENABLE);						//SS���ʹ��
	SPI_Cmd(SPI1, ENABLE);
}
/********************************************
*��������SPI1_WriteByte
*����������дһ���ֽڵ�SPI����
*���������TxData��д���ֽ�
*�����������
*����ֵ����
*����˵����
********************************************/
void SPI1_WriteByte(uint8_t TxData)
{				 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������		  
	SPI1->DR=TxData;	 	  	//����һ��byte 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte  
	SPI1->DR;	
}
/********************************************
*��������SPI1_ReadByte
*������������SPI���߶�һ���ֽ�
*�����������
*�����������
*����ֵ����ȡ�����ֽ�
*����˵����
********************************************/
uint8_t SPI1_ReadByte(void)
{			 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������			  
	SPI1->DR=0xFF;	 	  		//����һ�������ݲ����������ݵ�ʱ�� 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte  
	return SPI1->DR;  						    
}
/********************************************
*��������SPI1_CrisEnter
*���������������ٽ���
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void SPI1_CrisEnter(void)
{
	__set_PRIMASK(1);
}
/********************************************
*��������SPI1_CrisExit
*�����������˳��ٽ���
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void SPI1_CrisExit(void)
{
	__set_PRIMASK(0);
}

/********************************************
*��������SPI1_CS_Select
*����������Ƭѡ�ź�����͵�ƽ
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void SPI1_CS_Select(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}
/********************************************
*��������SPI1_CS_Deselect
*����������Ƭѡ�ź�����ߵ�ƽ
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void SPI1_CS_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

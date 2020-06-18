#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
							  
//////////////////////////////////////////////////////////////////////////////////
 
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��

u8 SPI2_step=0;
u8 SPI_RX_BUF[128];
u8 SPI_TX_BUF[128];
u8 spi_delaytime=50;

void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB12/13/14/15����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12 ���� spi_cs
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
	 //SPI2 NVIC ����
//  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	
//	SPI2_ReadWriteByte(0xff);//��������		 
  SPI_CS=0;               //�͵�ƽ��Ч
}   
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI2,ENABLE); 

} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		
//	u8 retry=0;	
 // SPI_CS=0; 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
//		{
//		retry++;
//		if(retry>200)return 0;
//		}			  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
//	SPI_CS=1; 
//	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
//		{
//		retry++;
//		if(retry>200)return 0;
//		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����					    
}


void SPI2_WriteNBytes(u8* pBuffer,u16 NumByteToWrite)   //SPI2д��NByte������
{		
	u16 i;				 	
  for(i=0;i<NumByteToWrite;i++){SPI2_ReadWriteByte(pBuffer[i]);delay_ms(10);}//ѭ��д�� 			    
}

void SPI2_WR_COM(void)            //���ͻ�ȡ���������ݻ���У׼������ָ��
{
	u8 tempcrc[64];
	u8 i;
	u16 crcnum;
	if(Read_CGQ_flag==1)           //���ͻ�ȡ����������
	{ 
		Read_CGQ_flag=0;
		SPI_TX_BUF[0]=0x40;SPI_TX_BUF[1]=0x03;SPI_TX_BUF[2]=0x02;
		if((NOW_temp>=1)&&(NOW_temp<=8))
		{SPI_TX_BUF[3]=1<<(8-NOW_temp);SPI_TX_BUF[4]=0;}
		else if((NOW_temp>=9)&&(NOW_temp<=16))
		{SPI_TX_BUF[3]=0;SPI_TX_BUF[4]=1<<(16-NOW_temp);}
		for(i=0;i<2;i++)
		{
			tempcrc[i]=SPI_TX_BUF[3+i];
		}
		crcnum=CRC16(tempcrc,2);
		SPI_TX_BUF[5]=crcnum/256;
		SPI_TX_BUF[6]=crcnum%256;
		SPI2_WriteNBytes(SPI_TX_BUF,7); 
    UART1Write(SPI_TX_BUF,7);		
		Read_CGQ_ok=1;
	}
	if(Mark_CGQ_flag==1)            //����У׼������ָ��
	{  
		Mark_CGQ_flag=0;
		SPI_TX_BUF[0]=0x40;SPI_TX_BUF[1]=0x06;SPI_TX_BUF[2]=0x03;
		if((NOW_temp>=1)&&(NOW_temp<=8))
		{SPI_TX_BUF[3]=1<<(8-NOW_temp);SPI_TX_BUF[4]=0;}
		else if((NOW_temp>=9)&&(NOW_temp<=16))
		{SPI_TX_BUF[3]=0;SPI_TX_BUF[4]=1<<(16-NOW_temp);}
		SPI_TX_BUF[5]=Mark_CGQ_fans;
		for(i=0;i<3;i++)
		{
			tempcrc[i]=SPI_TX_BUF[3+i];
		}
		crcnum=CRC16(tempcrc,3);
		SPI_TX_BUF[6]=crcnum/256;
		SPI_TX_BUF[7]=crcnum%256;
		SPI2_WriteNBytes(SPI_TX_BUF,8); 
    UART1Write(SPI_TX_BUF,8);	
    Mark_CGQ_ok	=1;	
	}
}

void SPI2_ReadNBytes(void)   //SPI2��ȡNByte������
{		
	u16 i;
	u8 n=0;
  u8 res=0;
	u8 num_len=0;
  res=SPI2_ReadWriteByte(0xff); delay_ms(spi_delaytime);	
	UART1Write0neByte(res);  
	if(SPI2_step==0)
	{
		if(res==0x40) {res=SPI2_ReadWriteByte(0xff);delay_ms(spi_delaytime);	SPI2_step=1;UART1Write0neByte(res);}
	}
	if(SPI2_step==1)
	{
		if(res==0x03) {num_len=SPI2_ReadWriteByte(0xff);delay_ms(spi_delaytime);	SPI2_step=2;UART1Write0neByte(num_len);}
		else if(res==0x06) {num_len=SPI2_ReadWriteByte(0xff);delay_ms(spi_delaytime);	SPI2_step=3;UART1Write0neByte(num_len);}
		else {SPI2_step=0;}
	}
	if(SPI2_step==2) 
	{
		for(i=0;i<num_len+2;i++)
		{
			SPI_RX_BUF[i]=SPI2_ReadWriteByte(0xff);delay_ms(spi_delaytime);	UART1Write0neByte(SPI_RX_BUF[i]);
		}
		for(i=0;i<8;i++)
		{ 
			if((SPI_RX_BUF[0]<<i)&0x80)  
			{
				CGQ_val[i][0]=1;
				CGQ_val[i][1]=SPI_RX_BUF[2+n];
				CGQ_val[i][2]=SPI_RX_BUF[3+n];
				CGQ_val[i][3]=SPI_RX_BUF[4+n];
				n=n+3;
			}
			else    {CGQ_val[i][0]=0;}
		}
		for(i=0;i<8;i++)
		{
			if((SPI_RX_BUF[1]<<i)&0x80) 
			{
				CGQ_val[i+8][0]=1;
				CGQ_val[i+8][1]=SPI_RX_BUF[2+n];
				CGQ_val[i+8][2]=SPI_RX_BUF[3+n];
				CGQ_val[i+8][3]=SPI_RX_BUF[4+n];
				n=n+3;
			}
			else    {CGQ_val[i+8][0]=0;}
		}	
		Read_CGQ_ok=0;
	} 
	else if(SPI2_step==3) 
	{
		for(i=0;i<num_len;i++)
		{
			SPI_RX_BUF[i]=SPI2_ReadWriteByte(0xff);delay_ms(spi_delaytime);	UART1Write0neByte(SPI_RX_BUF[i]);
		}	
    Mark_CGQ_ok=0;		
	}
	SPI2_step=0;
}


void spi_key_wr(void)
{
	u8 temp_key[8]={0x40,0x06,0x03,0x00,0x00,0x01,0x00,0x00};
	u8 temp_crc[8];
	u8 i;
	u16 crcnum;
	u8 res;
	u8 temp_back=0;
	u8 times=0;
  if((key_ok26_flag==1)||(key_ok27_flag==1))
	{
		key_ok26_flag=0;key_ok27_flag=0;
		temp_key[5]=0x01;
		for(i=0;i<3;i++)
	  {
		  temp_crc[i]=temp_key[i+3];
	  }
	  crcnum=CRC16(temp_crc,3);
	  temp_key[6]=crcnum/256;
	  temp_key[7]=crcnum%256;
	  SPI2_WriteNBytes(temp_key,8);  
	  UART1Write(temp_key,8);
		while(temp_back==0)
		{ times++;
		  res=SPI2_ReadWriteByte(0xff); delay_ms(spi_delaytime);UART1Write0neByte(res);
			if(res==0x40) 
			{
				 res=SPI2_ReadWriteByte(0xff); delay_ms(spi_delaytime);UART1Write0neByte(res);
				 if(res==0x06)   {temp_back=1;break;}
			}
			if(times%10==0) {SPI2_WriteNBytes(temp_key,8);UART1Write(temp_key,8); }
			if(times>=100) {times=0;break;}
		}
	}
	else if((key_back26_flag==1)||(key_back27_flag==1))
	{
		key_back26_flag=0;key_back27_flag=0;
		temp_key[5]=0x02;
		for(i=0;i<3;i++)
	  {
		  temp_crc[i]=temp_key[i+3];
	  }
	  crcnum=CRC16(temp_crc,3);
	  temp_key[6]=crcnum/256;
	  temp_key[7]=crcnum%256;
	  SPI2_WriteNBytes(temp_key,8);  
	  UART1Write(temp_key,8);
		while(temp_back==0)
		{ times++;
		  res=SPI2_ReadWriteByte(0xff); delay_ms(spi_delaytime);UART1Write0neByte(res);
			if(res==0x40) 
			{
				 res=SPI2_ReadWriteByte(0xff); delay_ms(spi_delaytime);UART1Write0neByte(res);
				 if(res==0x06)   {temp_back=1;break;}
				 if(times%10==0) {SPI2_WriteNBytes(temp_key,8);UART1Write(temp_key,8); }
			   if(times>=100)  {times=0;break;}
			}
		}
	}	
}

























#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
							  
//////////////////////////////////////////////////////////////////////////////////
 
//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25Q64/NRF24L01						  
//SPI口初始化
//这里针是对SPI2的初始化

u8 SPI2_step=0;
u8 SPI_RX_BUF[128];
u8 SPI_TX_BUF[128];
u8 spi_delaytime=50;

void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB12/13/14/15上拉
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  // PB12 推挽 spi_cs
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
	 //SPI2 NVIC 配置
//  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	
//	SPI2_ReadWriteByte(0xff);//启动传输		 
  SPI_CS=0;               //低电平有效
}   
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI2,ENABLE); 

} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
//	u8 retry=0;	
 // SPI_CS=0; 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); //检查指定的SPI标志位设置与否:发送缓存空标志位
//		{
//		retry++;
//		if(retry>200)return 0;
//		}			  
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
//	SPI_CS=1; 
//	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
//		{
//		retry++;
//		if(retry>200)return 0;
//		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据					    
}


void SPI2_WriteNBytes(u8* pBuffer,u16 NumByteToWrite)   //SPI2写入NByte的数据
{		
	u16 i;				 	
  for(i=0;i<NumByteToWrite;i++){SPI2_ReadWriteByte(pBuffer[i]);delay_ms(10);}//循环写数 			    
}

void SPI2_WR_COM(void)            //发送获取传感器数据或者校准传感器指令
{
	u8 tempcrc[64];
	u8 i;
	u16 crcnum;
	if(Read_CGQ_flag==1)           //发送获取传感器数据
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
	if(Mark_CGQ_flag==1)            //发送校准传感器指令
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

void SPI2_ReadNBytes(void)   //SPI2读取NByte的数据
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

























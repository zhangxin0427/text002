#include "bsp_spi.h"

/********************************************
*函数名：SPI1_RCC_Configuration
*功能描述：使能SPI1、GPIOA时钟
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
static void SPI1_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOC,ENABLE);
}
/********************************************
*函数名：SPI1_GPIO_Configuration
*功能描述：配置SPI1引脚
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
static void SPI1_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*PA5->SCK,PA6->MISO,PA7->MOSI*/		 					 
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; 				//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	/*PA4->CS*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	/*PC0->nRST*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//推挽输出
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
}
/********************************************
*函数名：SPI1_Configuration
*功能描述：配置SPI1相关参数
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void SPI1_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI1_RCC_Configuration();						//时钟配置

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//预分频2，SPI1速率36Mps
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;		//全双工
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;				//主机模式
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;				//八位数据位
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;					//空闲状态低电平
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;				//从第一个时钟沿开始采样
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;					//内部NSS信号由SSI位管理（NSS引脚可以用作它用）
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;				//MSB位优先
	SPI_InitStruct.SPI_CRCPolynomial = 7;					//CRC值计算的多项式
	SPI_Init(SPI1,&SPI_InitStruct);
	
	SPI1_GPIO_Configuration();						//SPI1引脚配置

	SPI_SSOutputCmd(SPI1, ENABLE);						//SS输出使能
	SPI_Cmd(SPI1, ENABLE);
}
/********************************************
*函数名：SPI1_WriteByte
*功能描述：写一个字节到SPI总线
*输入参数：TxData：写入字节
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void SPI1_WriteByte(uint8_t TxData)
{				 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//等待发送区空		  
	SPI1->DR=TxData;	 	  	//发送一个byte 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //等待接收完一个byte  
	SPI1->DR;	
}
/********************************************
*函数名：SPI1_ReadByte
*功能描述：从SPI总线读一个字节
*输入参数：无
*输出参数：无
*返回值：读取到的字节
*其他说明：
********************************************/
uint8_t SPI1_ReadByte(void)
{			 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//等待发送区空			  
	SPI1->DR=0xFF;	 	  		//发送一个空数据产生输入数据的时钟 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //等待接收完一个byte  
	return SPI1->DR;  						    
}
/********************************************
*函数名：SPI1_CrisEnter
*功能描述：进入临界区
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void SPI1_CrisEnter(void)
{
	__set_PRIMASK(1);
}
/********************************************
*函数名：SPI1_CrisExit
*功能描述：退出临界区
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void SPI1_CrisExit(void)
{
	__set_PRIMASK(0);
}

/********************************************
*函数名：SPI1_CS_Select
*功能描述：片选信号输出低电平
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void SPI1_CS_Select(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}
/********************************************
*函数名：SPI1_CS_Deselect
*功能描述：片选信号输出高电平
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void SPI1_CS_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

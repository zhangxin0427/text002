#include "bsp.h"

u8 DS3231_ACK;								//应答信号标志

/********************************************
*函数名：DS3231_GPIO_Init
*功能描述：DS3231端口初始化
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
	DS3231SCL------------PB8（推挽输出）
	DS3231SDA------------PB9（推挽输出）
	DS3231RST------------PE3（推挽输出）
	DS3231INT------------PE2（上拉输入配置为外部中断）
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
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/********************************************
*函数名：DS3231_SDA_Set_In
*功能描述：配置SDA端口为输入端口
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
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
*函数名：DS3231_SDA_Set_Out
*功能描述：配置SDA端口为输出端口
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
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
*函数名：EXTI2_IRQHandler
*功能描述：中断线2中断处理函数
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

/********************************************
*函数名：BCD_TO_HEX
*功能描述：BCD转换为HEX
*输入参数：BCD:需转换的BCD码
*输出参数：无
*返回值：HEX:转换后的HEX码
*其他说明：
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
*函数名：HEX_TO_BCD
*功能描述：HEX转换为BCD
*输入参数：HEX:需转换的HEX码
*输出参数：无
*返回值：BCD:转换后的BCD码
*其他说明：
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
*函数名：Start_I2C
*功能描述：I2C总线启动信号
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
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
*函数名：Stop_I2C
*功能描述：I2C总线停止信号
*输入参数：无
*输出参数：无
*返回值：无
*其他说明：
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
*函数名：DS3231_SendByte
*功能描述：DS3231字节发送
*输入参数：(unsigned char)data：需传送的字节
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void DS3231_SendByte(unsigned char data)
{
	unsigned char BitCnt;
	for(BitCnt = 0;BitCnt < 8;BitCnt++)
	{
		if((data << BitCnt)&0x80)
			DS3231_SDA_H;			//判断发送位
		else
			DS3231_SDA_L;
		bsp_DelayUS(1);
		DS3231_SCL_H;				//置时钟线为高电平，通知被控器开始接收数据位
		bsp_DelayUS(5);				//保证时钟高电平周期大于4us
		DS3231_SCL_L;
	}
	bsp_DelayUS(2);
	DS3231_SDA_H;					//八位发送完后释放数据线，准备接收应答
	DS3231_SDA_Set_In();				//设置SDA端口为输入端口
	bsp_DelayUS(2);
	DS3231_SCL_H;
	bsp_DelayUS(3);
	if(DS3231_SDA_In == 1)				//判断是否接收到应答信号
		DS3231_ACK = 0;				//非应答信号
	else
		DS3231_ACK = 1;				//应答信号
	DS3231_SCL_L;
	DS3231_SDA_Set_Out();				//恢复SDA端口为输出端口
	bsp_DelayUS(2);
}

/********************************************
*函数名：DS3231_RcvByte
*功能描述：DS3231字节接收
*输入参数：无
*输出参数：(unsigned char)RcvByte：接收到的字节
*返回值：无
*其他说明：
********************************************/
unsigned char DS3231_RcvByte(void)
{
	unsigned char RcvByte = 0;
	unsigned char BitCnt;
	
	DS3231_SDA_H;					//释放数据线
	DS3231_SDA_Set_In();				//设置SDA端口为输入端口
	bsp_DelayUS(2);
	for(BitCnt = 0;BitCnt < 8;BitCnt++)
	{
		DS3231_SCL_L;				//置时钟线为低，准备接受数据
		bsp_DelayUS(5);				//时钟低电平周期要大于4us
		DS3231_SCL_H;				//置时钟线为高，使数据线上的数据有效
		bsp_DelayUS(3);
		RcvByte = RcvByte << 1;
		if(DS3231_SDA_In == 1)			//读数据位，接收到数据位放入RcvByte
			RcvByte += 1;
		bsp_DelayUS(5);
	}
	DS3231_SCL_L;					//释放时钟线
	bsp_DelayUS(2);
	DS3231_SDA_Set_Out();				//恢复SDA端口为输出端口
	return RcvByte;
}

/********************************************
*函数名：DS3231_I2C_ACK
*功能描述：DS3231应答信号发送
*输入参数：(unsigned char)Ack_Flag：应答信号标志
*输出参数：无
*返回值：无
*其他说明：
********************************************/
void DS3231_I2C_ACK(unsigned char Ack_Flag)
{
	if(Ack_Flag == 0)
		DS3231_SDA_L;				//发出应答信号
	else
		DS3231_SDA_H;				//发出非应答信号
	bsp_DelayUS(5);
	DS3231_SCL_H;
	bsp_DelayUS(5);
	DS3231_SCL_L;					//释放时钟线
	bsp_DelayUS(2);
}

/********************************************
*函数名：DS3231_SendData
*功能描述：DS3231数据发送
*输入参数：(unsigned char)Addr：写入地址	(unsigned char)Data：写入数据
*输出参数：无
*返回值：0：发送失败	1：发送成功
*其他说明：
********************************************/
unsigned char DS3231_SendData(unsigned char Addr,unsigned char Data)
{
	Start_I2C();					//启动I2C总线
	DS3231_SendByte(DS3231_WriteAdd);		//写模式
	if(DS3231_ACK == 0)				//如果是非应答信号，返回失败
		return 0;
	DS3231_SendByte(Addr);				//写入地址
	if(DS3231_ACK == 0)				//如果是非应答信号，返回失败
		return 0;
	DS3231_SendByte(Data);				//写入数据
	if(DS3231_ACK == 0)				//如果是非应答信号，返回失败
		return 0 ;
	Stop_I2C();					//停止I2C总线
	bsp_DelayUS(10);
	return 1;
}

/********************************************
*函数名：DS3231_RcvData
*功能描述：DS3231数据读取
*输入参数：(unsigned char)Addr：读取地址	(unsigned char*)Data：读取数据	
*输出参数：无
*返回值：0：读取失败	1:读取成功
*其他说明：
********************************************/
unsigned char DS3231_RcvData(unsigned char Addr,unsigned char* Data)
{
	Start_I2C();					//启动I2C总线
	DS3231_SendByte(DS3231_WriteAdd);		//写模式
	if(DS3231_ACK == 0)				//如果是非应答信号，返回失败
		return 0;
	DS3231_SendByte(Addr);				//写入读取地址
	if(DS3231_ACK == 0)				//如果是非应答信号，返回失败
		return 0;
	
	Start_I2C();					//重新启动I2C总线
	DS3231_SendByte(DS3231_ReadAdd);		//读模式
	if(DS3231_ACK == 0)				//如果是非应答信号，返回失败
		return 0;
	*Data = DS3231_RcvByte();			//读取数据
	DS3231_I2C_ACK(1);				//非应答信号
	Stop_I2C();					//停止I2C总线
	return 1;					//返回成功
}

/********************************************
*函数名：DS3231_Modify_Time
*功能描述：修改DS3231时间
*输入参数：DS_RTC:时间参数结构体
*输出参数：无
*返回值：0：修改失败	1：修改成功
*其他说明：
********************************************/
unsigned char DS3231_Modify_Time(RTC_Param *DS_RTC)
{
	u8 temp = 0;
	
	temp = HEX_TO_BCD(DS_RTC->Years);	//修改年
	if(DS3231_SendData(DS3231_Year,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Months);	//修改月
	if(DS3231_SendData(DS3231_Month,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Days);		//修改日
	if(DS3231_SendData(DS3231_Day,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Hours);	//修改时
	if(DS3231_SendData(DS3231_Hour,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Minutes);	//修改分
	if(DS3231_SendData(DS3231_Minute,temp) == 0)
		return 0;
	
	temp = HEX_TO_BCD(DS_RTC->Seconds);	//修改秒
	if(DS3231_SendData(DS3231_Second,temp) == 0)
		return 0;
	
	return 1;
}
/********************************************
*函数名：DS3231_Read_Time
*功能描述：读取DS3231时间
*输入参数：DS_RTC:时间参数结构体
*输出参数：无
*返回值：0：读取失败	1：读取成功
*其他说明：
********************************************/
unsigned char DS3231_Read_Time(RTC_Param *DS_RTC)
{
	u8 temp = 0;
	
	if(DS3231_RcvData(DS3231_Year,&temp) == 0)
		return 0;
	DS_RTC->Years = BCD_TO_HEX(temp);		//读取年
	
	if(DS3231_RcvData(DS3231_Month,&temp) == 0)
		return 0;
	DS_RTC->Months = BCD_TO_HEX(temp);		//读取月
	
	if(DS3231_RcvData(DS3231_Day,&temp) == 0)
		return 0;
	DS_RTC->Days = BCD_TO_HEX(temp);			//读取日
	
	if(DS3231_RcvData(DS3231_Hour,&temp) == 0)
		return 0;
	temp &= 0x3F;					//24小时制
	DS_RTC->Hours = BCD_TO_HEX(temp);		//读取时
	
	if(DS3231_RcvData(DS3231_Minute,&temp) == 0)
		return 0;
	DS_RTC->Minutes = BCD_TO_HEX(temp);		//读取分
	
	if(DS3231_RcvData(DS3231_Second,&temp) == 0)
		return 0;
	DS_RTC->Seconds = BCD_TO_HEX(temp);		//读取秒
	
	return 1;
}

#include "24cxx.h"
#include "oled.h"

//////////////////////////////////////////////////////////////////////////////////	 
										  
//********************************************************************************
#define uchar unsigned char
#define uint unsigned int
 
/**********************************************************/          
#define  IIC_24C02  0x01   //是否为24c02 

u16 save_data_num=0;

//初始化IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能GPIOB时钟
	
  //GPIOB8,B9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		   //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
  GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9); 	//PC8,PC9 输出高
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
  	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
 	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  

	IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据  
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++ = AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

void AT24CXX_ROM(void)
{
	u8 iic_flag=0;
	u16 i;
	/*iic_flag=AT24CXX_ReadOneByte(IIC_ADDR);
	if(iic_flag!=0x55)
	{
		for(i=0;i<200;i++)
		{AT24CXX_WriteOneByte(i,0x00);delay_ms(10); }
		AT24CXX_WriteOneByte(IIC_ADDR,0x55); 
	}*/
	if(AT24CXX_ReadOneByte(0x64)!=0x25){						  //氨氮
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x65,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x66,0x00);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x67,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x68,0x0a);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x69,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x6A,0x28);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x6B,0x19);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x6C,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x6D,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x6E,0x32);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x6F,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x70,0xc8);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x71,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x72,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x73,0x90);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x74,0x96);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x75,0x02);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x76,0x58);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x77,0xC8);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x78,0x03);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x79,0x20);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x7A,0xDC);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x7B,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x7C,0x4C);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x7D,0xfa);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x7E,0x07);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x7F,0xD0);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x64,0x25);
	  delay_ms(5);
								                     }
	 if(AT24CXX_ReadOneByte(0x80)!=0x25){						  //亚硝酸盐
   delay_ms(5);
	  AT24CXX_WriteOneByte(0x81,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x82,0x00);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x83,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x84,0x0a);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x85,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x86,0x28);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x87,0x19);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x88,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x89,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x8A,0x32);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x8B,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x8C,0xc8);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x8D,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x8E,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x8F,0x90);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x90,0x96);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x91,0x02);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x92,0x58);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x93,0xC8);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x94,0x03);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x95,0x20);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x96,0xDC);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x97,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x98,0x4C);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x99,0xfa);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x9A,0x07);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x9B,0xD0);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x80,0x25);
	  delay_ms(5);
								                     }
		if(AT24CXX_ReadOneByte(0x9C)!=0x25){						  //总碱度
   delay_ms(5);
	  AT24CXX_WriteOneByte(0x9D,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0x9E,0x00);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0x9F,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA0,0x0a);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA1,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA2,0x28);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA3,0x19);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xA4,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA5,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA6,0x32);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xA7,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xA8,0xc8);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xA9,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xAA,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xAB,0x90);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xAC,0x96);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xAD,0x02);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xAE,0x58);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xAF,0xC8);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xB0,0x03);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xB1,0x20);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xB2,0xDC);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xB3,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xB4,0x4C);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xB5,0xfa);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xB6,0x07);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xB7,0xD0);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0x9C,0x25);
	  delay_ms(5);
								                     }	
if(AT24CXX_ReadOneByte(0xB8)!=0x25){						  //总硬度
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xB9,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xBA,0x00);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xBB,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xBC,0x0a);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xBD,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xBE,0x28);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xBF,0x19);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xC0,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC1,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC2,0x32);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xC3,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC4,0xc8);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xC5,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC6,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC7,0x90);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC8,0x96);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xC9,0x02);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xCA,0x58);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xCB,0xC8);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xCC,0x03);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xCD,0x20);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xCE,0xDC);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xCF,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xD0,0x4C);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xD1,0xfa);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xD2,0x07);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xD3,0xD0);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xB8,0x25);
	  delay_ms(5);
								                     }		
if(AT24CXX_ReadOneByte(0xD4)!=0x25){						  //钙离子
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xD5,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xD6,0x00);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xD7,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xD8,0x0a);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xD9,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xDA,0x28);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xDB,0x19);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xDC,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xDD,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xDE,0x32);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xDF,0x00);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xE0,0xc8);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xE1,0x64);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xE2,0x01);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xE3,0x90);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xE4,0x96);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xE5,0x02);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xE6,0x58);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xE7,0xC8);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xE8,0x03);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xE9,0x20);
    delay_ms(5);
	  AT24CXX_WriteOneByte(0xEA,0xDC);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xEB,0x04);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xEC,0x4C);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xED,0xfa);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xEE,0x07);
	  delay_ms(5);
	  AT24CXX_WriteOneByte(0xEF,0xD0);
	  delay_ms(5);
		AT24CXX_WriteOneByte(0xD4,0x25);
	  delay_ms(5);
								                     }
   for(i=0;i<9;i++){                    //读氨氮参数
		NH_Calibr.x_data[i]=AT24CXX_ReadOneByte(101+3*i);
		 delay_ms(3);
		NH_Calibr.y_datah[i]=AT24CXX_ReadOneByte(102+3*i);
		 delay_ms(3);
		NH_Calibr.y_datal[i]=AT24CXX_ReadOneByte(103+3*i);
		 delay_ms(3); 
	 }							 
	   for(i=0;i<9;i++){                    //读亚硝酸盐参数
		HNO_Calibr.x_data[i]=AT24CXX_ReadOneByte(129+3*i);
		 delay_ms(3);
		HNO_Calibr.y_datah[i]=AT24CXX_ReadOneByte(130+3*i);
		 delay_ms(3);
		HNO_Calibr.y_datal[i]=AT24CXX_ReadOneByte(131+3*i);
		 delay_ms(3); 
	 }	
		    for(i=0;i<9;i++){                    //读总碱度参数
		H_Calibr.x_data[i]=AT24CXX_ReadOneByte(157+3*i);
		 delay_ms(3);
		H_Calibr.y_datah[i]=AT24CXX_ReadOneByte(158+3*i);
		 delay_ms(3);
		H_Calibr.y_datal[i]=AT24CXX_ReadOneByte(159+3*i);
		 delay_ms(3); 
	 }	
				   for(i=0;i<9;i++){                    //读总硬度参数
		B_Calibr.x_data[i]=AT24CXX_ReadOneByte(185+3*i);
		 delay_ms(3);
		B_Calibr.y_datah[i]=AT24CXX_ReadOneByte(186+3*i);
		 delay_ms(3);
		B_Calibr.y_datal[i]=AT24CXX_ReadOneByte(187+3*i);
		 delay_ms(3); 
	 }	
					    for(i=0;i<9;i++){                    //读钙离子参数
		Ca_Calibr.x_data[i]=AT24CXX_ReadOneByte(213+3*i);
		 delay_ms(3);
		Ca_Calibr.y_datah[i]=AT24CXX_ReadOneByte(214+3*i);
		 delay_ms(3);
		Ca_Calibr.y_datal[i]=AT24CXX_ReadOneByte(215+3*i);
		 delay_ms(3); 
	 }	
	 
	 
	 
	gs_u16_RRatio=AT24CXX_ReadOneByte(GS_Ratio_ADDR)*256+AT24CXX_ReadOneByte(GS_Ratio_ADDR+1);
	gs_u16_GRatio=AT24CXX_ReadOneByte(GS_Ratio_ADDR+2)*256+AT24CXX_ReadOneByte(GS_Ratio_ADDR+3);
	gs_u16_BRatio=AT24CXX_ReadOneByte(GS_Ratio_ADDR+4)*256+AT24CXX_ReadOneByte(GS_Ratio_ADDR+5);
}


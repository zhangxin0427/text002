#include "24cxx.h"
#include "oled.h"

//////////////////////////////////////////////////////////////////////////////////	 
										  
//********************************************************************************
#define uchar unsigned char
#define uint unsigned int
 
/**********************************************************/          
#define  IIC_24C02  0x01   //�Ƿ�Ϊ24c02 

u16 save_data_num=0;

//��ʼ��IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //ʹ��GPIOBʱ��
	
  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		   //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB
  GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9); 	//PC8,PC9 �����
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
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
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
  	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
 	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  

	IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д����  
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++ = AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
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
	if(AT24CXX_ReadOneByte(0x64)!=0x25){						  //����
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
	 if(AT24CXX_ReadOneByte(0x80)!=0x25){						  //��������
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
		if(AT24CXX_ReadOneByte(0x9C)!=0x25){						  //�ܼ��
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
if(AT24CXX_ReadOneByte(0xB8)!=0x25){						  //��Ӳ��
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
if(AT24CXX_ReadOneByte(0xD4)!=0x25){						  //������
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
   for(i=0;i<9;i++){                    //����������
		NH_Calibr.x_data[i]=AT24CXX_ReadOneByte(101+3*i);
		 delay_ms(3);
		NH_Calibr.y_datah[i]=AT24CXX_ReadOneByte(102+3*i);
		 delay_ms(3);
		NH_Calibr.y_datal[i]=AT24CXX_ReadOneByte(103+3*i);
		 delay_ms(3); 
	 }							 
	   for(i=0;i<9;i++){                    //���������β���
		HNO_Calibr.x_data[i]=AT24CXX_ReadOneByte(129+3*i);
		 delay_ms(3);
		HNO_Calibr.y_datah[i]=AT24CXX_ReadOneByte(130+3*i);
		 delay_ms(3);
		HNO_Calibr.y_datal[i]=AT24CXX_ReadOneByte(131+3*i);
		 delay_ms(3); 
	 }	
		    for(i=0;i<9;i++){                    //���ܼ�Ȳ���
		H_Calibr.x_data[i]=AT24CXX_ReadOneByte(157+3*i);
		 delay_ms(3);
		H_Calibr.y_datah[i]=AT24CXX_ReadOneByte(158+3*i);
		 delay_ms(3);
		H_Calibr.y_datal[i]=AT24CXX_ReadOneByte(159+3*i);
		 delay_ms(3); 
	 }	
				   for(i=0;i<9;i++){                    //����Ӳ�Ȳ���
		B_Calibr.x_data[i]=AT24CXX_ReadOneByte(185+3*i);
		 delay_ms(3);
		B_Calibr.y_datah[i]=AT24CXX_ReadOneByte(186+3*i);
		 delay_ms(3);
		B_Calibr.y_datal[i]=AT24CXX_ReadOneByte(187+3*i);
		 delay_ms(3); 
	 }	
					    for(i=0;i<9;i++){                    //�������Ӳ���
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


#include "stm32f10x_gpio.h"
#include "delay.h"

#define  HDS1_HIGH 					GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define  HDS1_LOW  					GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define  HDS2_HIGH 					GPIO_SetBits(GPIOB,GPIO_Pin_2)
#define  HDS2_LOW  					GPIO_ResetBits(GPIOB,GPIO_Pin_2)

#define  HCS_HIGH 					GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define  HCS_LOW  					GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define  HRW_HIGH 					GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define  HRW_LOW  					GPIO_ResetBits(GPIOB,GPIO_Pin_4)

#define  HCNTL1_HIGH 				GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define  HCNTL1_LOW  				GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define  HCNTL0_HIGH 				GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define  HCNTL0_LOW  				GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define  HBIL_HIGH 					GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define  HBIL_LOW  					GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define  Bit_Mask           0x8000

signed short cc[4];

void HPI_HDCTR_Config()
  {
    //����PB7/HBIL  PB6/HCNTL0  PB5/HCNTL1  PB4/HRW  PB3/HCS  PB2/HDS2  PB1/HDS1
		  GPIO_InitTypeDef GPIO_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //ʹ��GPIOBʱ�
	    GPIO_InitStructure.GPIO_Pin 	= (GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);              
      GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP; 		 //������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC
		
		  HDS2_HIGH;
			HCS_LOW;//ʹ��HPI
	}

void HPI_HDWR_Config()
	{
		//���� PB8/HD0  PB9/HD1  PB10/HD2  PB11/HD3  PB12/HD4  PB13/HD5  PB14/HD6  PB15/HD7
		  GPIO_InitTypeDef GPIO_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //ʹ��GPIOBʱ�
	    GPIO_InitStructure.GPIO_Pin 	= (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);              
      GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP; 		 //������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC
	}
		
void HPI_HDRD_Ccnfig()
	{
			//���� PB8/HD0  PB9/HD1  PB10/HD2  PB11/HD3  PB12/HD4  PB13/HD5  PB14/HD6  PB15/HD7
		  GPIO_InitTypeDef GPIO_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //ʹ��GPIOBʱ�
	    GPIO_InitStructure.GPIO_Pin 	= (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);              
      GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING; 		 //������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC
	}


//=============================================================//
//==					 HPIA�Ĵ�����  					     									 ==//
//=============================================================//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											                       ==//
//==����ֵ:	  ��											                       ==//
//=============================================================//
u16 HPIA_Read()
		{
			u16 HL_WORD;
			HPI_HDRD_Ccnfig();
			
			HCNTL0_LOW;
			HCNTL1_HIGH;//�����ɶ�дHPIA
			
			HBIL_LOW;//��ǰ�ǵ�һ���ֽ�
			HRW_HIGH;//����Ҫ���ѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			
			//дHPIA ���ֽ�
			HL_WORD=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);								//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 	//дHD6λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//дHD5λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//дHD4λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//дHD3λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//дHD2λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//дHD1λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//дHD0λ
			
			HDS1_HIGH;//�������ݲ���
			HBIL_HIGH;//��ǰ�ǵڶ��ֽ�
			HRW_HIGH;//����Ҫ��ѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			
			//дHPIA ���ֽ�
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//дHD7λ
			
			HDS1_HIGH;//�������ݲ���
			
			return HL_WORD;
    }

//=============================================================//
//==					            HPIA�Ĵ���д  					     			 ==//
//=============================================================//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											                       ==//
//==����ֵ:	  ��											                       ==//
//=============================================================//	
  void HPIA_Write(u16 W_data)
		{
			u16 tmp;
			tmp=W_data;
			HPI_HDWR_Config();

			HCNTL0_LOW;
			HCNTL1_HIGH;//�����ɶ�дHPIA
			
			HBIL_LOW;//��ǰ�ǵ�һ�ֽ�
			HRW_LOW;//����Ҫ��дѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
	    //д��λ�ֽ�
			if ((tmp&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7д
			if ((tmp&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6д
			if ((tmp&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5д
			if ((tmp&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4д
			if ((tmp&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3д
			if ((tmp&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2д
	    if ((tmp&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1д
			if ((tmp&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0д
					
			HDS1_HIGH;//�������ݲ���
			
			HBIL_HIGH;//��ǰ�ǵڶ��ֽ�
			HRW_LOW;//����Ҫ��дѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			//д���λ�ֽ�
			if (((tmp<<8)&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15);
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7д
			if (((tmp<<8)&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6д
			if (((tmp<<8)&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5д
			if (((tmp<<8)&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4д
			if (((tmp<<8)&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3д
			if (((tmp<<8)&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2д
	    if (((tmp<<8)&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1д
			if (((tmp<<8)&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0д
				
			HDS1_HIGH;//�������ݲ���
		}	
	
//=============================================================//
//==					           HPIC�Ĵ�����  					     				 ==//
//=============================================================//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											                       ==//
//==����ֵ:	  ��											                       ==//
//=============================================================//
u16 HPIC_Read()
		{
			u16 HL_WORD;
			HPI_HDRD_Ccnfig();
			
			HCNTL0_LOW;
			HCNTL1_LOW;//�����ɶ�дHPIC
			
			HBIL_LOW;//��ǰ�ǵ�һ���ֽ�
			HRW_HIGH;//����Ҫ���ѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			
			//дHPIC���ֽ�
			HL_WORD=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);								//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 	//дHD6λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//дHD5λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//дHD4λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//дHD3λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//дHD2λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//дHD1λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//дHD0λ
			
			HDS1_HIGH;//�����������ݲ���
			HBIL_HIGH;//��ǰ�ǵڶ��ֽ�
			HRW_HIGH;//����Ҫ��ѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			
			//дHPIC���ֽ�
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);		//дHD6λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//дHD5λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//дHD4λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//дHD3λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//дHD2λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//дHD1λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//дHD0λ
			
			HDS1_HIGH;//�������ݲ���
			
			return HL_WORD;
    }

//=============================================================//
//==					            HPIC�Ĵ���д  					     			 ==//
//=============================================================//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											                       ==//
//==����ֵ:	  ��											                       ==//
//=============================================================//	
  void HPIC_Write(u16 W_data)
		{
			u16 tmp;
			tmp=W_data;
			HPI_HDWR_Config();
			
			HCNTL0_LOW;
			HCNTL1_LOW;//�����ɶ�дHPIC
			
			HBIL_LOW;//��ǰ�ǵ�һ�ֽ�
			HRW_LOW;//����Ҫ��дѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
	    //д��λ�ֽ�
			if ((tmp&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7д
			if ((tmp&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6д
			if ((tmp&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5д
			if ((tmp&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4д
			if ((tmp&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3д
			if ((tmp&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2д
	    if ((tmp&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1д
			if ((tmp&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0д
					
			HDS1_HIGH;//�������ݲ���
			
			HBIL_HIGH;//��ǰ�ǵڶ��ֽ�
			HRW_LOW;//����Ҫ��дѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			//д���λ�ֽ�
			if (((tmp<<8)&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15);
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7д
			if (((tmp<<8)&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6д
			if (((tmp<<8)&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5д
			if (((tmp<<8)&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4д
			if (((tmp<<8)&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3д
			if (((tmp<<8)&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2д
	    if (((tmp<<8)&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1д
			if (((tmp<<8)&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0д
				
			HDS1_HIGH;//�������ݲ���
		}		


//=============================================================//
//==					 HPID�Ĵ�����  					     									 ==//
//=============================================================//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											                       ==//
//==����ֵ:	  ��											                       ==//
//=============================================================//
u16 HPID_Read()
		{
			u16 HL_WORD;
			HPI_HDRD_Ccnfig();
			
			HCNTL0_HIGH;
			HCNTL1_LOW;//�����ɶ�дHPID
			
			HBIL_LOW;//��ǰ�ǵ�һ���ֽ�
			HRW_HIGH;//����Ҫ���ѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			
			//дHPID ���ֽ�
			HL_WORD=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);								//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 	//дHD6λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//дHD5λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//дHD4λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//дHD3λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//дHD2λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//дHD1λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//дHD0λ
			
			HDS1_HIGH;//�����������ݲ���
			
			HBIL_HIGH;//��ǰ�ǵڶ��ֽ�
			HRW_HIGH;//����Ҫ��ѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			//дHPID ���ֽ�
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//дHD7λ
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//дHD7λ
			
			HDS1_HIGH;//�������ݲ���
			
			return HL_WORD;
    }
	
//=============================================================//
//==					            HPID�Ĵ���д  					     			 ==//
//=============================================================//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											                       ==//
//==����ֵ:	  ��											                       ==//
//=============================================================//	
  void HPID_Write(u16 W_data)
		{
			u16 tmp;
			tmp=W_data;
			HPI_HDWR_Config();
			
			HCNTL0_HIGH;
			HCNTL1_LOW;//�����ɶ�дHPID
			
			HBIL_LOW;//��ǰ�ǵ�һ�ֽ�
			HRW_LOW;//����Ҫ��дѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
	    //д��λ�ֽ�
			if ((tmp&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7д
			if ((tmp&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6д
			if ((tmp&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5д
			if ((tmp&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4д
			if ((tmp&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3д
			if ((tmp&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2д
	    if ((tmp&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1д
			if ((tmp&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0д
					
			HDS1_HIGH;//�������ݲ���
			
			HBIL_HIGH;//��ǰ�ǵڶ��ֽ�
			HRW_LOW;//����Ҫ��дѡͨHPI
			HDS1_LOW;//��ʼ���ݲ���
			//д���λ�ֽ�
			if (((tmp<<8)&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15);
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7д
			if (((tmp<<8)&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6д
			if (((tmp<<8)&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5д
			if (((tmp<<8)&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4д
			if (((tmp<<8)&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3д
			if (((tmp<<8)&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2д
	    if (((tmp<<8)&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1д
			if (((tmp<<8)&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0д
				
			HDS1_HIGH;//�������ݲ���
		}	
	
		
void dspcode_load(const char *dspcode)
{   
		//u16 m;
		u16 i;
		u8  len;
		//u16 tmp;
		//u16 mb[];
		
		while(1)
		{
				u16 address;
				len=*dspcode++;
				if(len==0) break;
				address=(*dspcode++)*256;
				address=address+(*dspcode++);
				address--;
				//HPIA_Write(address);
				//m = HPIA_Read();
				dspcode++;
				for(i=0;i<len/2;i++)
				{
					 u16 Wcode,Rcode,Kload;
					 HPIA_Write(address);
					 Wcode=(*dspcode++)*256;
					 Wcode=Wcode+(*dspcode++);
					 for(Kload=0;Kload<2;Kload++)
					 {
						 HPIA_Write(address);
						 HPID_Write(Wcode);
						 Rcode = HPID_Read();
						 if(Wcode==Rcode) 
							 { 
								 address = HPIA_Read();
								 address--;
								 break;
							 } 
					 }  
					 if(Kload==2) 
					 {
						 GPIO_SetBits(GPIOC,GPIO_Pin_8); 
						 while(1) {}
					 }	 
					 delay_ms(1);
				}
				dspcode++;	
		}			
	}
			
void load_dspcode(u16 start_adr,const char *dspcode)		
{		
    HPI_HDCTR_Config();//����DSP�������
		HPIC_Write(0x0808);
		dspcode_load(dspcode);
		HPIA_Write(0x007E);
		HPID_Write(start_adr);
}	

void DSP_PHREAD(u16 flag_adr,u16 updata_adr,u16 dndata_adr,u16 resdata_adr)	
{
			//signed short cc_up[512];
			//signed short cc_dn[512];
			delay_ms(500);
			HPIA_Write(flag_adr);
			HPID_Write(0x5A5A);
			
			/*HPIA_Write(updata_adr);
			delay_ms(10);
			for(i=0;i<512;i++)
			cc_up[i]= HPID_Read();
			
			HPIA_Write(dndata_adr);
			delay_ms(10);
			for(i=0;i<512;i++)
			cc_dn[i]= HPID_Read();*/

			HPIA_Write(resdata_adr);
			cc[0]= HPID_Read();
			cc[1]= HPID_Read();
			cc[2]= HPID_Read();
			cc[3]= HPID_Read();
			
			HPIA_Write(flag_adr);
			HPID_Write(0x5A5A);
}

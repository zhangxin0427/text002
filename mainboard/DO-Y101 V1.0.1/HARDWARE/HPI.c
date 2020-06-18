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
    //²âÊÔPB7/HBIL  PB6/HCNTL0  PB5/HCNTL1  PB4/HRW  PB3/HCS  PB2/HDS2  PB1/HDS1
		  GPIO_InitTypeDef GPIO_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //Ê¹ÄÜGPIOBÊ±Ö
	    GPIO_InitStructure.GPIO_Pin 	= (GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);              
      GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 
      GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯GPIOC
		
		  HDS2_HIGH;
			HCS_LOW;//Ê¹ÄÜHPI
	}

void HPI_HDWR_Config()
	{
		//²âÊÔ PB8/HD0  PB9/HD1  PB10/HD2  PB11/HD3  PB12/HD4  PB13/HD5  PB14/HD6  PB15/HD7
		  GPIO_InitTypeDef GPIO_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //Ê¹ÄÜGPIOBÊ±Ö
	    GPIO_InitStructure.GPIO_Pin 	= (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);              
      GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 
      GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯GPIOC
	}
		
void HPI_HDRD_Ccnfig()
	{
			//²âÊÔ PB8/HD0  PB9/HD1  PB10/HD2  PB11/HD3  PB12/HD4  PB13/HD5  PB14/HD6  PB15/HD7
		  GPIO_InitTypeDef GPIO_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	       //Ê¹ÄÜGPIOBÊ±Ö
	    GPIO_InitStructure.GPIO_Pin 	= (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);              
      GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING; 		 //ÍÆÍìÊä³
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		 
      GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯GPIOC
	}


//=============================================================//
//==					 HPIA¼Ä´æÆ÷¶Á  					     									 ==//
//=============================================================//
//==Èë¿Ú²ÎÊý: ÎÞ											 											 ==//
//==³ö¿Ú²ÎÊý: ÎÞ											                       ==//
//==·µ»ØÖµ:	  ÎÞ											                       ==//
//=============================================================//
u16 HPIA_Read()
		{
			u16 HL_WORD;
			HPI_HDRD_Ccnfig();
			
			HCNTL0_LOW;
			HCNTL1_HIGH;//Ö÷»ú¿É¶ÁÐ´HPIA
			
			HBIL_LOW;//µ±Ç°ÊÇµÚÒ»¸ö×Ö½Ú
			HRW_HIGH;//Ö÷»úÒªÇó¶ÁÑ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			
			//Ð´HPIA ¸ß×Ö½Ú
			HL_WORD=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);								//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 	//Ð´HD6Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//Ð´HD5Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//Ð´HD4Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//Ð´HD3Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//Ð´HD2Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//Ð´HD1Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//Ð´HD0Î»
			
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			HBIL_HIGH;//µ±Ç°ÊÇµÚ¶þ×Ö½Ú
			HRW_HIGH;//Ö÷»úÒªÇóÑ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			
			//Ð´HPIA µÍ×Ö½Ú
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//Ð´HD7Î»
			
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			
			return HL_WORD;
    }

//=============================================================//
//==					            HPIA¼Ä´æÆ÷Ð´  					     			 ==//
//=============================================================//
//==Èë¿Ú²ÎÊý: ÎÞ											 											 ==//
//==³ö¿Ú²ÎÊý: ÎÞ											                       ==//
//==·µ»ØÖµ:	  ÎÞ											                       ==//
//=============================================================//	
  void HPIA_Write(u16 W_data)
		{
			u16 tmp;
			tmp=W_data;
			HPI_HDWR_Config();

			HCNTL0_LOW;
			HCNTL1_HIGH;//Ö÷»ú¿É¶ÁÐ´HPIA
			
			HBIL_LOW;//µ±Ç°ÊÇµÚÒ»×Ö½Ú
			HRW_LOW;//Ö÷»úÒªÇóÐ´Ñ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
	    //Ð´¸ßÎ»×Ö½Ú
			if ((tmp&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7Ð´
			if ((tmp&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6Ð´
			if ((tmp&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5Ð´
			if ((tmp&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4Ð´
			if ((tmp&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3Ð´
			if ((tmp&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2Ð´
	    if ((tmp&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1Ð´
			if ((tmp&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0Ð´
					
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			
			HBIL_HIGH;//µ±Ç°ÊÇµÚ¶þ×Ö½Ú
			HRW_LOW;//Ö÷»úÒªÇóÐ´Ñ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			//Ð´ÈëµÍÎ»×Ö½Ú
			if (((tmp<<8)&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15);
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7Ð´
			if (((tmp<<8)&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6Ð´
			if (((tmp<<8)&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5Ð´
			if (((tmp<<8)&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4Ð´
			if (((tmp<<8)&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3Ð´
			if (((tmp<<8)&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2Ð´
	    if (((tmp<<8)&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1Ð´
			if (((tmp<<8)&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0Ð´
				
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
		}	
	
//=============================================================//
//==					           HPIC¼Ä´æÆ÷¶Á  					     				 ==//
//=============================================================//
//==Èë¿Ú²ÎÊý: ÎÞ											 											 ==//
//==³ö¿Ú²ÎÊý: ÎÞ											                       ==//
//==·µ»ØÖµ:	  ÎÞ											                       ==//
//=============================================================//
u16 HPIC_Read()
		{
			u16 HL_WORD;
			HPI_HDRD_Ccnfig();
			
			HCNTL0_LOW;
			HCNTL1_LOW;//Ö÷»ú¿É¶ÁÐ´HPIC
			
			HBIL_LOW;//µ±Ç°ÊÇµÚÒ»¸ö×Ö½Ú
			HRW_HIGH;//Ö÷»úÒªÇó¶ÁÑ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			
			//Ð´HPIC¸ß×Ö½Ú
			HL_WORD=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);								//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 	//Ð´HD6Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//Ð´HD5Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//Ð´HD4Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//Ð´HD3Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//Ð´HD2Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//Ð´HD1Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//Ð´HD0Î»
			
			HDS1_HIGH;//½áÊøÊý¾ÝÊý¾Ý²Ù×÷
			HBIL_HIGH;//µ±Ç°ÊÇµÚ¶þ×Ö½Ú
			HRW_HIGH;//Ö÷»úÒªÇóÑ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			
			//Ð´HPICµÍ×Ö½Ú
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);		//Ð´HD6Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//Ð´HD5Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//Ð´HD4Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//Ð´HD3Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//Ð´HD2Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//Ð´HD1Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//Ð´HD0Î»
			
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			
			return HL_WORD;
    }

//=============================================================//
//==					            HPIC¼Ä´æÆ÷Ð´  					     			 ==//
//=============================================================//
//==Èë¿Ú²ÎÊý: ÎÞ											 											 ==//
//==³ö¿Ú²ÎÊý: ÎÞ											                       ==//
//==·µ»ØÖµ:	  ÎÞ											                       ==//
//=============================================================//	
  void HPIC_Write(u16 W_data)
		{
			u16 tmp;
			tmp=W_data;
			HPI_HDWR_Config();
			
			HCNTL0_LOW;
			HCNTL1_LOW;//Ö÷»ú¿É¶ÁÐ´HPIC
			
			HBIL_LOW;//µ±Ç°ÊÇµÚÒ»×Ö½Ú
			HRW_LOW;//Ö÷»úÒªÇóÐ´Ñ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
	    //Ð´¸ßÎ»×Ö½Ú
			if ((tmp&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7Ð´
			if ((tmp&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6Ð´
			if ((tmp&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5Ð´
			if ((tmp&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4Ð´
			if ((tmp&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3Ð´
			if ((tmp&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2Ð´
	    if ((tmp&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1Ð´
			if ((tmp&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0Ð´
					
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			
			HBIL_HIGH;//µ±Ç°ÊÇµÚ¶þ×Ö½Ú
			HRW_LOW;//Ö÷»úÒªÇóÐ´Ñ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			//Ð´ÈëµÍÎ»×Ö½Ú
			if (((tmp<<8)&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15);
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7Ð´
			if (((tmp<<8)&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6Ð´
			if (((tmp<<8)&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5Ð´
			if (((tmp<<8)&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4Ð´
			if (((tmp<<8)&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3Ð´
			if (((tmp<<8)&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2Ð´
	    if (((tmp<<8)&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1Ð´
			if (((tmp<<8)&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0Ð´
				
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
		}		


//=============================================================//
//==					 HPID¼Ä´æÆ÷¶Á  					     									 ==//
//=============================================================//
//==Èë¿Ú²ÎÊý: ÎÞ											 											 ==//
//==³ö¿Ú²ÎÊý: ÎÞ											                       ==//
//==·µ»ØÖµ:	  ÎÞ											                       ==//
//=============================================================//
u16 HPID_Read()
		{
			u16 HL_WORD;
			HPI_HDRD_Ccnfig();
			
			HCNTL0_HIGH;
			HCNTL1_LOW;//Ö÷»ú¿É¶ÁÐ´HPID
			
			HBIL_LOW;//µ±Ç°ÊÇµÚÒ»¸ö×Ö½Ú
			HRW_HIGH;//Ö÷»úÒªÇó¶ÁÑ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			
			//Ð´HPID ¸ß×Ö½Ú
			HL_WORD=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);								//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 	//Ð´HD6Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//Ð´HD5Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//Ð´HD4Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//Ð´HD3Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//Ð´HD2Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//Ð´HD1Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//Ð´HD0Î»
			
			HDS1_HIGH;//½áÊøÊý¾ÝÊý¾Ý²Ù×÷
			
			HBIL_HIGH;//µ±Ç°ÊÇµÚ¶þ×Ö½Ú
			HRW_HIGH;//Ö÷»úÒªÇóÑ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			//Ð´HPID µÍ×Ö½Ú
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);		//Ð´HD7Î»
			HL_WORD=(HL_WORD<<1)|GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);		//Ð´HD7Î»
			
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			
			return HL_WORD;
    }
	
//=============================================================//
//==					            HPID¼Ä´æÆ÷Ð´  					     			 ==//
//=============================================================//
//==Èë¿Ú²ÎÊý: ÎÞ											 											 ==//
//==³ö¿Ú²ÎÊý: ÎÞ											                       ==//
//==·µ»ØÖµ:	  ÎÞ											                       ==//
//=============================================================//	
  void HPID_Write(u16 W_data)
		{
			u16 tmp;
			tmp=W_data;
			HPI_HDWR_Config();
			
			HCNTL0_HIGH;
			HCNTL1_LOW;//Ö÷»ú¿É¶ÁÐ´HPID
			
			HBIL_LOW;//µ±Ç°ÊÇµÚÒ»×Ö½Ú
			HRW_LOW;//Ö÷»úÒªÇóÐ´Ñ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
	    //Ð´¸ßÎ»×Ö½Ú
			if ((tmp&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7Ð´
			if ((tmp&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6Ð´
			if ((tmp&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5Ð´
			if ((tmp&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4Ð´
			if ((tmp&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3Ð´
			if ((tmp&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2Ð´
	    if ((tmp&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1Ð´
			if ((tmp&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0Ð´
					
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
			
			HBIL_HIGH;//µ±Ç°ÊÇµÚ¶þ×Ö½Ú
			HRW_LOW;//Ö÷»úÒªÇóÐ´Ñ¡Í¨HPI
			HDS1_LOW;//¿ªÊ¼Êý¾Ý²Ù×÷
			//Ð´ÈëµÍÎ»×Ö½Ú
			if (((tmp<<8)&0x8000)==0x8000)  GPIO_SetBits(GPIOB,GPIO_Pin_15);
					else GPIO_ResetBits(GPIOB,GPIO_Pin_15);//HD7Ð´
			if (((tmp<<8)&0x4000)==0x4000)  GPIO_SetBits(GPIOB,GPIO_Pin_14); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_14);//HD6Ð´
			if (((tmp<<8)&0x2000)==0x2000)  GPIO_SetBits(GPIOB,GPIO_Pin_13); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_13);//HD5Ð´
			if (((tmp<<8)&0x1000)==0x1000)  GPIO_SetBits(GPIOB,GPIO_Pin_12); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_12);//HD4Ð´
			if (((tmp<<8)&0x0800)==0x0800)  GPIO_SetBits(GPIOB,GPIO_Pin_11); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_11);//HD3Ð´
			if (((tmp<<8)&0x0400)==0x0400)  GPIO_SetBits(GPIOB,GPIO_Pin_10); 
					else GPIO_ResetBits(GPIOB,GPIO_Pin_10);//HD2Ð´
	    if (((tmp<<8)&0x0200)==0x0200)  GPIO_SetBits(GPIOB,GPIO_Pin_9);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_9); //HD1Ð´
			if (((tmp<<8)&0x0100)==0x0100)  GPIO_SetBits(GPIOB,GPIO_Pin_8);  
					else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //HD0Ð´
				
			HDS1_HIGH;//½áÊøÊý¾Ý²Ù×÷
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
    HPI_HDCTR_Config();//¼ÓÔØDSP³ÌÐò´úÂë
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

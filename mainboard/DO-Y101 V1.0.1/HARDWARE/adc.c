 #include "adc.h"
 #include "delay.h"
 #include "fft.h"
 #include "stm32_dsp.h"
 #include "fft.h"
 #include "math.h"
 #include "RS485.h"
 #include "dac1.h"
//////////////////////////////////////////////////////////////////////////////////	 
u32 databuf[200];
//float RingBuffer[10];
volatile u16 min,max;
static u16 combuffer[200];
#define NPT 1024
u32 ADC_Value[NPT/2];
u32 lBufInArray[NPT];

u32 lBufOutArray[NPT];
u32 lBufMagArray[NPT];
float PI2=6.28318530717959;
//float PI=3.14159265;
u32 Fs=192000;
extern float ccf;
extern u16   MODBUS_DATA_FRAME[];
#define AVG_BUFFMV_LEN    10
u8 Empty=1;
extern u16 phase_buffer[10];
////////////////////////////////////////////////////////////////////////////////// 


/********************�������Сֵ����******************/
void find_max_min(u16 *combuffer,u8 comlen)
{
	u8 i;
	max=min=0;
	for(i=0;i<comlen;i++)
    {
			if(combuffer[min]>combuffer[i])
				min=i;
			if( (combuffer[max]<combuffer[i])||(combuffer[max]==combuffer[i]) )
        max=i;
    }
}

/**********************���ݴ���*************************/
u16 dataprocessing(u16 *combuffer,u8 comlen)
{
	u8 i;
	float buff=0;
	u16 temp;
	find_max_min((u16 *)combuffer,comlen);
	for(i=0;i<comlen;i++)
  {
			if( (i!=max)&&(i!=min) )
			buff+=combuffer[i];
   }
	temp=buff/(comlen-2);
	return temp;
}
		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��8																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}

//���ADCֵ
//ch:ͨ��ֵ 8
u16 Get_Adc(u8 ch)   
{		
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
  	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_71Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ1.5����	  			    
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
  // delay_us(2);
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}




u16 Get_Adc_fft(u8 ch)
{
	//int i=0,j=0;
	unsigned short i=0,j=0,t,k;
		signed short lX,lY,a,b;
    float X,Y,Mag,phase,c,d,wn,wn1;
		u16 bb;


/*
  ��    ע:��lBufInArray�����У�ÿ�����ݵĸ�16λ�洢�������ݵ�ʵ����
          ��16λ�洢�������ݵ��鲿(����Ϊ0)  	
*/	
    Dac1_Test();
 		delay_ms(25);
    		for(i=0;i<NPT;i++)
		{
			/*	
			wn=0.54-0.46*cos((2 * PI*i )/(NPT-1) );     
				wn1=wn*(float)Get_Adc(ch);
				lBufInArray[i]=(u16)wn1<<16;
			*/
		   lBufInArray[i]=Get_Adc(ch) << 16;

		}
		Dac1_turn_off();

	cr4_fft_1024_stm32(lBufOutArray, lBufInArray, NPT);   
	for(j=0;j<NPT;j++)
		{
        lX  = (lBufOutArray[j] << 16) >> 16;//����     
        lY  = (lBufOutArray[j] >> 16);//ʵ��      
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;   
        Mag = sqrt(X * X + Y * Y) / NPT;
				
				if(j == 0)
            lBufMagArray[j] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[j] = (unsigned long)(Mag * 65536);


		}
		
		for(t=10;t<500;t++)
		{
			databuf[t-10]=lBufMagArray[t];
		}
		for(k=0;k<490;k++)
		{
			if(databuf[max]<databuf[k])
				
		 					max=k;
		}   
	
		
		
		
		    a  = (lBufOutArray[max+10] << 16) >> 16;//����
        b  = (lBufOutArray[max+10] >> 16);//ʵ��  
				c = NPT * ((float)a) / 32768;//����
				d = NPT * ((float)b) / 32768;//ʵ��

				
		    phase=atan(d/c);
				if(d>=0)
				{
					if(c>=0)
						;
					else
						phase+=PI;						
				}
				else
				{
					if(c>=0)
						phase+=PI2;
					else
						phase+=PI;
				}
		
				phase=phase*18000/PI;
 				bb=(u16)phase;

	//			phase=100*c/d;
    return ((u16)phase);                                       
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u16 t,i,j; 
	u16 buffer[11];
//  u16 temp;
  float TempValue,temp;	
	for(t=0;t<times;t++)
	{
		buffer[t]=Get_Adc_fft(ch);
//		delay_ms(1);
	}
	
	for (j=0;j<times-1;j++)
  {
    for (i=0;i<times-j-1;i++)
    {
      if ( buffer[i]>buffer[i+1] )
      {
        temp=buffer[i];
        buffer[i]=buffer[i+1]; 
        buffer[i+1]=temp;  
      }
    }
  }
	TempValue=buffer[(times-1)/2];	
	return (TempValue);
 // TempValue = combuffer[(times-1)/2];
	//dataprocessing((u16 *)combuffer,times);
	
}

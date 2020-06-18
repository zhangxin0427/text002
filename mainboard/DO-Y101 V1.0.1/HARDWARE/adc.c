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


/********************找最大最小值索引******************/
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

/**********************数据处理*************************/
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
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道8																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}

//获得ADC值
//ch:通道值 8
u16 Get_Adc(u8 ch)   
{		
  	//设置指定ADC的规则组通道，一个序列，采样时间
  	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_71Cycles5 );	//ADC1,ADC通道,采样时间为1.5周期	  			    
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
  // delay_us(2);
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}




u16 Get_Adc_fft(u8 ch)
{
	//int i=0,j=0;
	unsigned short i=0,j=0,t,k;
		signed short lX,lY,a,b;
    float X,Y,Mag,phase,c,d,wn,wn1;
		u16 bb;


/*
  备    注:在lBufInArray数组中，每个数据的高16位存储采样数据的实部，
          低16位存储采样数据的虚部(总是为0)  	
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
        lX  = (lBufOutArray[j] << 16) >> 16;//虚数     
        lY  = (lBufOutArray[j] >> 16);//实数      
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
	
		
		
		
		    a  = (lBufOutArray[max+10] << 16) >> 16;//虚数
        b  = (lBufOutArray[max+10] >> 16);//实数  
				c = NPT * ((float)a) / 32768;//虚数
				d = NPT * ((float)b) / 32768;//实数

				
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

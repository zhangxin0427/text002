#include "dac1.h"
#include "math.h"	 
#include "stm32f10x.h"
void Dac1_Test(void)
{
#if DAC1_DMA==1


    Dac1_Init();
			
/*
	  while(1)
		{
	
		}
#else
		u16 i=0;
    Dac1_Init();
//		Dac1_Set_Vol(2000);	
		while(1)
		{     
				for(i=0;i<DAC1_SIZE;i++)
				{
					 DAC_SetChannel1Data(DAC_Align_12b_R,DAC1_SIN_BUF[i]);
				} 
		}*/
#endif
			
}

void Dac1_turn_off(void)
{
	DAC_Cmd(DAC_Channel_1, DISABLE);    
}
 
u16 DAC1_SIN_BUF[DAC1_SIZE];  //�������ݻ����� 
 
//�������Ҳ������У�����DAC1_SIZE���ɡ�
void Dac1_Creat_Buf(void)
{
    u32 i=0; 
    float inc=2*PI/DAC1_SIZE;        //����������һ������DAC_SIN_BUF���㣩
    float outdata=0;
    for(i=0;i<DAC1_SIZE;i++)
    {
        outdata=2047*(1+sin(inc*i));    //������dots����Ϊ���ڵ�ÿ�����ֵ���Ŵ�2048������ƫ�Ƶ���������
//        printf("%f\r\n",outdata);
        DAC1_SIN_BUF[i]=outdata;
    }  
} 

//DACͨ��1�����ʼ��
void Dac1_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		DAC_InitTypeDef  DAC_InitType;
#if DAC1_DMA==1
	  DMA_InitTypeDef  DMA_InitStructure;
	  Dac1_Creat_Buf();
#endif
 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	                     //ʹ��DACͨ��ʱ�� 
#if DAC1_DMA==1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	                       //ʹ��DMA����
#endif
 
		//My_GPIO_Init(GPIOA,GPIO_Pin_4,GPIO_AD_IN,GPIO_P_DOWN,GPIO_50MHz);         //ģ������ ���� 100m
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��PORTAͨ��ʱ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // �˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ģ������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
 
		DAC_InitType.DAC_Trigger=DAC_Trigger_None;	                               //��ʹ�ô������� TEN1=0
		DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;                   //��ʹ�ò��η���
		DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;         //���Ρ���ֵ����
		DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	                 //DAC1�������ر� BOFF1=1
		DAC_Init(DAC_Channel_1,&DAC_InitType);	                                   //��ʼ��DACͨ��1
  
		DAC_Cmd(DAC_Channel_1, ENABLE);                                            //ʹ��DAC1

		DAC_SetChannel1Data(DAC_Align_12b_R, 0);                                   //12λ�Ҷ������ݸ�ʽ����DACֵ
#if DAC1_DMA==1
		DMA_DeInit(DMA1_Channel3);                                                 //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DAC->DHR12R1;             //DMA�������ַ
		DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)DAC1_SIN_BUF;              //DMA�ڴ����ַ
		DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;          //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
		DMA_InitStructure.DMA_BufferSize         = DAC1_SIZE;                      //DMAͨ����DMA����Ĵ�С
		DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;      //�����ַ�Ĵ�������
		DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;           //�ڴ��ַ�Ĵ�������
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���ݿ��Ϊ8λ
		DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;    //���ݿ��Ϊ8λ
		DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;              //����������ģʽ
		DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;            //DMAͨ�� xӵ�������ȼ� 
		DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);                               //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	  	
	 
	  DAC1_TIM3_Init(DAC1_TIME-1,71);                                            //10*72/72000000=10us������һ���㣬DAC_SIN_SIZE(100)����=1ms  1K 
		
		DMA_Cmd(DMA1_Channel3, ENABLE);                                            //ʹ��ͨ��,����һ��DMA����
#endif

}
#if DAC1_DMA==1
void DAC1_TIM3_Init(u16 arr,u16 psc)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        //ʱ��ʹ��
		
		//��ʱ��TIM3��ʼ��
		TIM_TimeBaseStructure.TIM_Period = arr;                     //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =psc;                   //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  	TIM_ITConfig(TIM3,TIM_DIER_UDE,ENABLE);                     //TIM3->DIER|=1<<8;//�������ģʽ/ʹ�ø����¼���Ϊ�������
		 
		TIM_Cmd(TIM3, ENABLE);                                      //ʹ��TIMx
}
#endif


//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void Dac1_Set_Vol(u16 vol)
{
		float temp=vol*4095/3300;
		DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

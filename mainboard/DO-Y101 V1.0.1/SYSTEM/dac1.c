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
 
u16 DAC1_SIN_BUF[DAC1_SIZE];  //发送数据缓冲区 
 
//产生正弦波函序列，根据DAC1_SIZE生成。
void Dac1_Creat_Buf(void)
{
    u32 i=0; 
    float inc=2*PI/DAC1_SIZE;        //计算增量（一个周期DAC_SIN_BUF个点）
    float outdata=0;
    for(i=0;i<DAC1_SIZE;i++)
    {
        outdata=2047*(1+sin(inc*i));    //计算以dots个点为周期的每个点的值，放大2048倍，并偏移到正数区域。
//        printf("%f\r\n",outdata);
        DAC1_SIN_BUF[i]=outdata;
    }  
} 

//DAC通道1输出初始化
void Dac1_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		DAC_InitTypeDef  DAC_InitType;
#if DAC1_DMA==1
	  DMA_InitTypeDef  DMA_InitStructure;
	  Dac1_Creat_Buf();
#endif
 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	                     //使能DAC通道时钟 
#if DAC1_DMA==1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	                       //使能DMA传输
#endif
 
		//My_GPIO_Init(GPIOA,GPIO_Pin_4,GPIO_AD_IN,GPIO_P_DOWN,GPIO_50MHz);         //模拟输入 下拉 100m
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能PORTA通道时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // 端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //模拟输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
 
		DAC_InitType.DAC_Trigger=DAC_Trigger_None;	                               //不使用触发功能 TEN1=0
		DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;                   //不使用波形发生
		DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;         //屏蔽、幅值设置
		DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	                 //DAC1输出缓存关闭 BOFF1=1
		DAC_Init(DAC_Channel_1,&DAC_InitType);	                                   //初始化DAC通道1
  
		DAC_Cmd(DAC_Channel_1, ENABLE);                                            //使能DAC1

		DAC_SetChannel1Data(DAC_Align_12b_R, 0);                                   //12位右对齐数据格式设置DAC值
#if DAC1_DMA==1
		DMA_DeInit(DMA1_Channel3);                                                 //将DMA的通道1寄存器重设为缺省值
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DAC->DHR12R1;             //DMA外设基地址
		DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)DAC1_SIN_BUF;              //DMA内存基地址
		DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;          //数据传输方向，从内存读取发送到外设
		DMA_InitStructure.DMA_BufferSize         = DAC1_SIZE;                      //DMA通道的DMA缓存的大小
		DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;      //外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;           //内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;    //数据宽度为8位
		DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;              //工作在正常模式
		DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;            //DMA通道 x拥有中优先级 
		DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                //DMA通道x没有设置为内存到内存传输
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);                               //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器	  	
	 
	  DAC1_TIM3_Init(DAC1_TIME-1,71);                                            //10*72/72000000=10us传传输一个点，DAC_SIN_SIZE(100)个点=1ms  1K 
		
		DMA_Cmd(DMA1_Channel3, ENABLE);                                            //使能通道,开启一次DMA传输
#endif

}
#if DAC1_DMA==1
void DAC1_TIM3_Init(u16 arr,u16 psc)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);        //时钟使能
		
		//定时器TIM3初始化
		TIM_TimeBaseStructure.TIM_Period = arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
		TIM_TimeBaseStructure.TIM_Prescaler =psc;                   //设置用来作为TIMx时钟频率除数的预分频值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //根据指定的参数初始化TIMx的时间基数单位

  	TIM_ITConfig(TIM3,TIM_DIER_UDE,ENABLE);                     //TIM3->DIER|=1<<8;//触发输出模式/使用更新事件作为触发输出
		 
		TIM_Cmd(TIM3, ENABLE);                                      //使能TIMx
}
#endif


//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void Dac1_Set_Vol(u16 vol)
{
		float temp=vol*4095/3300;
		DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12位右对齐数据格式设置DAC值
}

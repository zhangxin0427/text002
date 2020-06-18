#include "sys.h"
#include "AIC23B.h"	
#include "crc.h"
#include "timer.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 	


AIC23B_Config	MyConfig = {
		CODEC_LLIVC_RMK(							//	Left line input channel volume control
			CODEC_LLIVC_LRS_ENABLE,			//	ʹ������ͨ��ͬʱ�޸�
			CODEC_LLIVC_LIM_NORMAL,			//	��ͨ��ʹ����ͨģʽ(������)
			CODEC_LLIVC_LIV_OF(0x17)		//	��Ƶ��������ΪĬ��ֵ
			),
		CODEC_RLIVC_RMK(							//	Right line input channel volume control
			CODEC_RLIVC_RLS_ENABLE,			//	��ͨ������ͨ����ͬ
			CODEC_RLIVC_RIM_MUTE,
			CODEC_LLIVC_LIV_OF(0x00)
			),
		CODEC_LHPVC_RMK(							//	Left Channel Headphone Volume Control
			CODEC_LHPVC_LRS_ENABLE,			//	ʹ�����Ҷ���ͨ��ͬʱ�޸�
			CODEC_LHPVC_LZC_DEFAULT,		//	ʹ�ܹ�����
			CODEC_LHPVC_LHV_OF(0x00)			//	���������������
			),
		CODEC_RHPVC_RMK(							//	Right Channel Headphone Volume Control
			CODEC_RHPVC_RLS_ENABLE,			//	��ͨ������ͨ����ͬ
			CODEC_RHPVC_RZC_DEFAULT,			
			CODEC_RHPVC_RHV_OF(0x00)
			),
		CODEC_AAPC_RMK(								//	Analog Audio Path Control
			CODEC_AAPC_STA_DEFAULT,
			CODEC_AAPC_STE_DEFAULT,			//	�رղ���
			CODEC_AAPC_DAC_DEFAULT,			//	ʹ��DAC,����ʹ��,����McBSP���͵������޷�ת������Ƶ
			CODEC_AAPC_BYP_DISABLE,			//	�ر�Bypass
			CODEC_AAPC_INSEL_DEFAULT,		//	ѡ����Ƶ����
			CODEC_AAPC_MICM_DEFAULT,
			CODEC_AAPC_MICB_DEFAULT			//	��ѡ��microphone,��ΪĬ��ֵ
			),
		CODEC_DAPC_RMK(								//	Digital audio path control
			CODEC_DAPC_DACM_DISABLE,		//	�ر�DAC soft mute
			CODEC_DAPC_DEEMP_DISABLE,
			CODEC_DAPC_ADCHP_ENABLE			//	ʹ��ADC��ͨ�˲�
			),
		CODEC_PDC_RMK(								//	Power down control
			CODEC_PDC_OFF_DEFAULT,			//	ȫ��ʹ��
			CODEC_PDC_CLK_DEFAULT,
			CODEC_PDC_OSC_DEFAULT,
			CODEC_PDC_OUT_DEFAULT,
			CODEC_PDC_DAC_DEFAULT,
			CODEC_PDC_ADC_ON,
			CODEC_PDC_MIC_ON,
			CODEC_PDC_LINE_ON
			),
		CODEC_DAIF_RMK(									//	Digital audio interface format
			CODEC_DAIF_MS_MASTER,					//	AIC23BΪ����ģʽ
			CODEC_DAIF_LRSWAP_DEFAULT,		//	��ֹ����ͨ������
			CODEC_DAIF_LRP_DELAY2CYCLE,		//	Ƭѡ�����غ�ĵڶ���ʱ��������MSB��Ч
			CODEC_DAIF_IWL_16BIT,					//	���ݳ���Ϊ16λ
			CODEC_DAIF_FOR_DSP						//	���ݸ�ʽΪDSPģʽ
			),
		CODEC_SRC_RMK(									//	Sample rate control
			CODEC_SRC_CLKOUT_DEFAULT,
			CODEC_SRC_CLKIN_DEFAULT,			//	�������ʱ�Ӷ�����ʱ��Ƶ��(12MHz)
			CODEC_SRC_SR_OF(7),						//	�����ʿ���
			CODEC_SRC_BOSR_DEFAULT,				//	BOSR == 0
			CODEC_SRC_MODE_USB				    //	ʱ��ģʽΪ��ͨ(��USBģʽ)
			),
		CODEC_DIA_RMK(									//	Digital Interface Activation
			CODEC_DIA_ACT_ACTIVE
			),
		CODEC_RST_RMK(									//	Reset Register
			CODEC_RST_RES_RESET
			),
};

ErrorStatus HSEStartUpStatus;


void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK/2 */
    RCC_PCLK2Config(RCC_HCLK_Div2); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
 
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}
 
void SPI1_Init(void)
{
 SPI_InitTypeDef SPI_InitStructure;
 GPIO_InitTypeDef GPIO_InitStructure;
	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_SPI1, ENABLE);	
 

 GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
 GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;		 
 GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
 GPIO_ResetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

 GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_4;
 GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;		 
 GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC
	
	
 SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
 SPI_InitStructure.SPI_Mode			=SPI_Mode_Master;
 SPI_InitStructure.SPI_DataSize	=SPI_DataSize_16b;
 SPI_InitStructure.SPI_CPOL			=SPI_CPOL_High;	
 SPI_InitStructure.SPI_CPHA			=SPI_CPHA_2Edge;
 SPI_InitStructure.SPI_NSS			=SPI_NSS_Soft;
 SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_256;
 SPI_InitStructure.SPI_FirstBit					=SPI_FirstBit_MSB;
 SPI_InitStructure.SPI_CRCPolynomial		=7;
 SPI_Init(SPI1,&SPI_InitStructure);
	
 SPI_CalculateCRC(SPI1,DISABLE);
 SPI_Cmd(SPI1, ENABLE);
 GPIO_SetBits(GPIOC,GPIO_Pin_4);
} 

void SPI1_WriteOneWordAIC23B(u16 para)
{
	 u8 retry = 0;
	 GPIO_SetBits(GPIOC,GPIO_Pin_4);
	 while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET)
	 {
		retry++;
		if(retry > 200)
			return;
	 }
	 delay_ms(1);	;
	 /* Send SPI1 data */	
	 GPIO_ResetBits(GPIOC,GPIO_Pin_4); 
	 SPI_I2S_SendData(SPI1, para);	
	 GPIO_SetBits(GPIOC,GPIO_Pin_4);
}


//=============================================================//
//==					                 ����Codec	  					       ==//
//=============================================================//
//==����˵��: McBSP��AIC23B�����ݽӿ�ͨ��,AIC23Bʹ��DSP Mode ==//
//==		  AIC23B��Ϊ����ģʽ,DSPΪ�ӻ�					 						 ==//
//==��ڲ���: ��											 											 ==//
//==���ڲ���: ��											 											 ==//
//==����ֵ:	  ��											 											 ==//
//=============================================================//
void Codec_Setup(AIC23B_Config *Config)
{
	SPI1_WriteOneWordAIC23B(Config->pdc);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->daif);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->aapc);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->dapc);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->aapc);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->src);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->llivc);	//	ʹ������ͨ��ͬʱ�޸�ʱ,ֻ��Ҫ������ͨ������
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->lhpvc);
	//delay_ms(1);
	SPI1_WriteOneWordAIC23B(Config->dia);
	//delay_ms(1);
}



//=============================================================//
//==					 CODEC��ʼ��	  					 ==//
//=============================================================//
//==��ڲ���: ��											 ==//
//==���ڲ���: ��											 ==//
//==����ֵ:	  ��											 ==//
//=============================================================//
void Codec_Init(void)
{
	Codec_Setup(&MyConfig);
}

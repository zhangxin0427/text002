#include "includes.h"


AIC23B_Config	MyConfig = {
		CODEC_LLIVC_RMK(					//	Left line input channel volume control
			CODEC_LLIVC_LRS_ENABLE,			//	ʹ������ͨ��ͬʱ�޸�
			CODEC_LLIVC_LIM_NORMAL,			//	��ͨ��ʹ����ͨģʽ(������)
			CODEC_LLIVC_LIV_DEFAULT			//	��Ƶ��������ΪĬ��ֵ
			),
		CODEC_RLIVC_RMK(					//	Right line input channel volume control
			CODEC_RLIVC_RLS_ENABLE,			//	��ͨ������ͨ����ͬ
			CODEC_RLIVC_RIM_NORMAL,
			CODEC_RLIVC_RIV_DEFAULT
			),
		CODEC_LHPVC_RMK(					//	Left Channel Headphone Volume Control
			CODEC_LHPVC_LRS_ENABLE,			//	ʹ�����Ҷ���ͨ��ͬʱ�޸�
			CODEC_LHPVC_LZC_DEFAULT,		//	ʹ�ܹ�����
			CODEC_LHPVC_LHV_DEFAULT			//	���������������
			),
		CODEC_RHPVC_RMK(					//	Right Channel Headphone Volume Control
			CODEC_RHPVC_RLS_ENABLE,			//	��ͨ������ͨ����ͬ
			CODEC_RHPVC_RZC_DEFAULT,			
			CODEC_RHPVC_RHV_DEFAULT
			),
		CODEC_AAPC_RMK(						//	Analog Audio Path Control
			CODEC_AAPC_STA_DEFAULT,
			CODEC_AAPC_STE_DEFAULT,			//	�رղ���
			CODEC_AAPC_DAC_DEFAULT,			//	ʹ��DAC,����ʹ��,����McBSP���͵������޷�ת������Ƶ
			CODEC_AAPC_BYP_DISABLE,			//	�ر�Bypass
			CODEC_AAPC_INSEL_DEFAULT,		//	ѡ����Ƶ����
			CODEC_AAPC_MICM_DEFAULT,
			CODEC_AAPC_MICB_DEFAULT			//	��ѡ��microphone,��ΪĬ��ֵ
			),
		CODEC_DAPC_RMK(						//	Digital audio path control
			CODEC_DAPC_DACM_DEFAULT,		//	�ر�DAC soft mute
			CODEC_DAPC_DEEMP_DISABLE,
			CODEC_DAPC_ADCHP_ENABLE			//	ʹ��ADC��ͨ�˲�
			),
		CODEC_PDC_RMK(						//	Power down control
			CODEC_PDC_OFF_DEFAULT,			//	ȫ��ʹ��
			CODEC_PDC_CLK_DEFAULT,
			CODEC_PDC_OSC_DEFAULT,
			CODEC_PDC_OUT_DEFAULT,
			CODEC_PDC_DAC_DEFAULT,
			CODEC_PDC_ADC_ON,
			CODEC_PDC_MIC_ON,
			CODEC_PDC_LINE_ON
			),
		CODEC_DAIF_RMK(						//	Digital audio interface format
			CODEC_DAIF_MS_MASTER,			//	AIC23BΪ����ģʽ
			CODEC_DAIF_LRSWAP_DEFAULT,		//	��ֹ����ͨ������
			CODEC_DAIF_LRP_DELAY2CYCLE,		//	Ƭѡ�����غ�ĵڶ���ʱ��������MSB��Ч
			CODEC_DAIF_IWL_16BIT,			//	���ݳ���Ϊ16λ
			CODEC_DAIF_FOR_DSP				//	���ݸ�ʽΪDSPģʽ
			),
		CODEC_SRC_RMK(						//	Sample rate control
			CODEC_SRC_CLKOUT_DEFAULT,
			CODEC_SRC_CLKIN_DEFAULT,		//	�������ʱ�Ӷ�����ʱ��Ƶ��(12.288MHz)
			CODEC_SRC_SR_OF(6),				//	�����ʿ���
			CODEC_SRC_BOSR_DEFAULT,			//	BOSR == 0
			CODEC_SRC_MODE_DEFAULT			//	ʱ��ģʽΪ��ͨ(��USBģʽ)
			),
		CODEC_DIA_RMK(						//	Digital Interface Activation
			CODEC_DIA_ACT_ACTIVE
			),
		CODEC_RST_RMK(						//	Reset Register
			CODEC_RST_RES_RESET
			),
};




//=============================================================//
//==					   ����Codec	  					 ==//
//=============================================================//
//==����˵��: McBSP��AIC23B�����ݽӿ�ͨ��,AIC23Bʹ��DSP Mode ==//
//==		  AIC23B��Ϊ����ģʽ,DSPλ�ӻ�					 ==//
//==��ڲ���: ��											 ==//
//==���ڲ���: ��											 ==//
//==����ֵ:	  ��											 ==//
//=============================================================//
void Codec_Setup(AIC23B_Config *Config)
{
	//MyI2C_WriteOneByte2AIC(Config->rst,  1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->pdc,  1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->daif, 1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->aapc, 1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->dapc, 1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->aapc, 1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->src,  1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->llivc,1, AIC23B_ADDR, 1, 1000);	//	ʹ������ͨ��ͬʱ�޸�ʱ,ֻ��Ҫ������ͨ������
	//MyI2C_WriteOneByte2AIC(Config->rlivc,1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->lhpvc,1, AIC23B_ADDR, 1, 1000);
	//MyI2C_WriteOneByte2AIC(Config->rhpvc,1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->dia,  1, AIC23B_ADDR, 1, 1000);
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

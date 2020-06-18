#include "includes.h"


AIC23B_Config	MyConfig = {
		CODEC_LLIVC_RMK(					//	Left line input channel volume control
			CODEC_LLIVC_LRS_ENABLE,			//	使能左右通道同时修改
			CODEC_LLIVC_LIM_NORMAL,			//	左通道使用普通模式(不静音)
			CODEC_LLIVC_LIV_DEFAULT			//	音频输入音量为默认值
			),
		CODEC_RLIVC_RMK(					//	Right line input channel volume control
			CODEC_RLIVC_RLS_ENABLE,			//	右通道和左通道相同
			CODEC_RLIVC_RIM_NORMAL,
			CODEC_RLIVC_RIV_DEFAULT
			),
		CODEC_LHPVC_RMK(					//	Left Channel Headphone Volume Control
			CODEC_LHPVC_LRS_ENABLE,			//	使能左右耳机通道同时修改
			CODEC_LHPVC_LZC_DEFAULT,		//	使能过零检测
			CODEC_LHPVC_LHV_DEFAULT			//	耳机输出音量控制
			),
		CODEC_RHPVC_RMK(					//	Right Channel Headphone Volume Control
			CODEC_RHPVC_RLS_ENABLE,			//	右通道和左通道相同
			CODEC_RHPVC_RZC_DEFAULT,			
			CODEC_RHPVC_RHV_DEFAULT
			),
		CODEC_AAPC_RMK(						//	Analog Audio Path Control
			CODEC_AAPC_STA_DEFAULT,
			CODEC_AAPC_STE_DEFAULT,			//	关闭侧音
			CODEC_AAPC_DAC_DEFAULT,			//	使能DAC,必须使能,否则McBSP发送的数据无法转化成音频
			CODEC_AAPC_BYP_DISABLE,			//	关闭Bypass
			CODEC_AAPC_INSEL_DEFAULT,		//	选择音频输入
			CODEC_AAPC_MICM_DEFAULT,
			CODEC_AAPC_MICB_DEFAULT			//	不选择microphone,设为默认值
			),
		CODEC_DAPC_RMK(						//	Digital audio path control
			CODEC_DAPC_DACM_DEFAULT,		//	关闭DAC soft mute
			CODEC_DAPC_DEEMP_DISABLE,
			CODEC_DAPC_ADCHP_ENABLE			//	使能ADC高通滤波
			),
		CODEC_PDC_RMK(						//	Power down control
			CODEC_PDC_OFF_DEFAULT,			//	全部使能
			CODEC_PDC_CLK_DEFAULT,
			CODEC_PDC_OSC_DEFAULT,
			CODEC_PDC_OUT_DEFAULT,
			CODEC_PDC_DAC_DEFAULT,
			CODEC_PDC_ADC_ON,
			CODEC_PDC_MIC_ON,
			CODEC_PDC_LINE_ON
			),
		CODEC_DAIF_RMK(						//	Digital audio interface format
			CODEC_DAIF_MS_MASTER,			//	AIC23B为主机模式
			CODEC_DAIF_LRSWAP_DEFAULT,		//	禁止左右通道交换
			CODEC_DAIF_LRP_DELAY2CYCLE,		//	片选上升沿后的第二个时钟上升沿MSB有效
			CODEC_DAIF_IWL_16BIT,			//	数据长度为16位
			CODEC_DAIF_FOR_DSP				//	数据格式为DSP模式
			),
		CODEC_SRC_RMK(						//	Sample rate control
			CODEC_SRC_CLKOUT_DEFAULT,
			CODEC_SRC_CLKIN_DEFAULT,		//	输入输出时钟都是主时钟频率(12.288MHz)
			CODEC_SRC_SR_OF(6),				//	采样率控制
			CODEC_SRC_BOSR_DEFAULT,			//	BOSR == 0
			CODEC_SRC_MODE_DEFAULT			//	时钟模式为普通(非USB模式)
			),
		CODEC_DIA_RMK(						//	Digital Interface Activation
			CODEC_DIA_ACT_ACTIVE
			),
		CODEC_RST_RMK(						//	Reset Register
			CODEC_RST_RES_RESET
			),
};




//=============================================================//
//==					   设置Codec	  					 ==//
//=============================================================//
//==函数说明: McBSP与AIC23B的数据接口通信,AIC23B使用DSP Mode ==//
//==		  AIC23B作为主机模式,DSP位从机					 ==//
//==入口参数: 无											 ==//
//==出口参数: 无											 ==//
//==返回值:	  无											 ==//
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
	MyI2C_WriteOneByte2AIC(Config->llivc,1, AIC23B_ADDR, 1, 1000);	//	使能左右通道同时修改时,只需要配置左通道即可
	//MyI2C_WriteOneByte2AIC(Config->rlivc,1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->lhpvc,1, AIC23B_ADDR, 1, 1000);
	//MyI2C_WriteOneByte2AIC(Config->rhpvc,1, AIC23B_ADDR, 1, 1000);
	MyI2C_WriteOneByte2AIC(Config->dia,  1, AIC23B_ADDR, 1, 1000);
}





//=============================================================//
//==					 CODEC初始化	  					 ==//
//=============================================================//
//==入口参数: 无											 ==//
//==出口参数: 无											 ==//
//==返回值:	  无											 ==//
//=============================================================//
void Codec_Init(void)
{
	Codec_Setup(&MyConfig);
}

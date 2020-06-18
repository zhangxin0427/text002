#ifndef	_AIC23B_H
#define	_AIC23B_H

#include "stm32f10x.h"
#define Uint16 u16


typedef	struct _AIC23B_Config
{
	Uint16	llivc;	//Left line input channel volume control
	Uint16	rlivc;	//Right line input channel volume control
	Uint16	lhpvc;	//Left channel headphone volume control
	Uint16	rhpvc;	//Right channel headphone volume control
	Uint16	aapc;	//Analog audio path control
	Uint16	dapc;	//Digital audio path control
	Uint16	pdc;	//Power down control
	Uint16	daif;	//Digital audio interface format
	Uint16	src;	//Sample rate control
	Uint16	dia;	//Digital interface activation
	Uint16	rst;	//Reset register
}AIC23B_Config;


#define		AIC23B_ADDR				0x1A
//=============================================================//
//==					 ¼Ä´æÆ÷Ó³Éä±í	  					 ==//
//=============================================================//
#define		LLIVC		(0x0000<<9) 	//Left line input channel volume control
#define		RLIVC		(0x0001<<9) 	//Right line input channel volume control
#define		LHPVC		(0x0002<<9) 	//Left channel headphone volume control
#define		RHPVC		(0x0003<<9) 	//Right channel headphone volume control
#define		AAPC		(0x0004<<9) 	//Analog audio path control
#define		DAPC		(0x0005<<9) 	//Digital audio path control
#define		PDC			(0x0006<<9) 	//Power down control
#define		DAIF		(0x0007<<9) 	//Digital audio interface format
#define		SRC			(0x0008<<9) 	//Sample rate control
#define		DIA			(0x0009<<9) 	//Digital interface activation
#define		RST			(0x000F<<9) 	//Reset register

//=============================================================//
//==Left line input channel volume control
#define	CODEC_LLIVC_RMK(lrs,lim,liv) 	((Uint16)(LLIVC|lrs|lim|liv)) 

#define	CODEC_LLIVC_LRS_DISABLE			(0x0000<<8)
#define	CODEC_LLIVC_LRS_ENABLE			(0x0001<<8)
#define	CODEC_LLIVC_LRS_DEFAULT			CODEC_LLIVC_LRS_DISABLE

#define	CODEC_LLIVC_LIM_NORMAL			(0x0000<<7)
#define	CODEC_LLIVC_LIM_MUTE			(0x0001<<7)
#define	CODEC_LLIVC_LIM_DEFAULT			CODEC_LLIVC_LIM_MUTE

#define	CODEC_LLIVC_LIV_OF(x)			(x)	
#define	CODEC_LLIVC_LIV_DEFAULT			0x0017		


//=============================================================//
//==Right line input channel volume control
#define	CODEC_RLIVC_RMK(rls,rim,riv) 	((Uint16)(RLIVC|rls|rim|riv)) 

#define	CODEC_RLIVC_RLS_DISABLE			(0x0000<<8)
#define	CODEC_RLIVC_RLS_ENABLE			(0x0001<<8)
#define	CODEC_LLIVC_RLS_DEFAULT			CODEC_LLIVC_RLS_DISABLE

#define	CODEC_RLIVC_RIM_NORMAL			(0x0000<<7)
#define	CODEC_RLIVC_RIM_MUTE			(0x0001<<7)
#define	CODEC_LLIVC_RIM_DEFAULT			CODEC_LLIVC_RIM_MUTE

#define	CODEC_RLIVC_RIV_OF(x)			(x)	
#define	CODEC_RLIVC_RIV_DEFAULT			0x0017


//=============================================================//
//==Left Channel Headphone Volume Control
#define	CODEC_LHPVC_RMK(lrs,lzc,lhv)	((Uint16)(LHPVC|lrs|lzc|lhv))

#define	CODEC_LHPVC_LRS_DISABLE			(0x0000<<8)
#define	CODEC_LHPVC_LRS_ENABLE			(0x0001<<8)
#define	CODEC_LHPVC_LRS_DEFAULT			CODEC_LHPVC_LRS_DISABLE

#define	CODEC_LHPVC_LZC_OFF				(0x0000<<7)
#define	CODEC_LHPVC_LZC_ON				(0x0001<<7)
#define	CODEC_LHPVC_LZC_DEFAULT			CODEC_LHPVC_LZC_ON

#define	CODEC_LHPVC_LHV_OF(x)			(x)
#define	CODEC_LHPVC_LHV_DEFAULT			0x0079


//=============================================================//
//==Right Channel Headphone Volume Control
#define	CODEC_RHPVC_RMK(rls,rzc,rhv)	((Uint16)(RHPVC|rls|rzc|rhv))

#define	CODEC_RHPVC_RLS_DISABLE			(0x0000<<8)
#define	CODEC_RHPVC_RLS_ENABLE			(0x0001<<8)
#define	CODEC_RHPVC_RLS_DEFAULT			CODEC_RHPVC_RLS_DISABLE

#define	CODEC_RHPVC_RZC_OFF				(0x0000<<7)
#define	CODEC_RHPVC_RZC_ON				(0x0001<<7)
#define	CODEC_RHPVC_RZC_DEFAULT			CODEC_RHPVC_RZC_ON

#define	CODEC_RHPVC_RHV_OF(x)			(x)
#define	CODEC_RHPVC_RHV_DEFAULT			0x0079


//=============================================================//
//==Analog Audio Path Control
#define	CODEC_AAPC_RMK(sta,ste,dac,byp,insel,micm,micb)	\
					  ((Uint16)(AAPC|sta|ste|dac|byp|insel|micm|micb))

#define	CODEC_AAPC_STA_OF(x)			(x<<6)
#define	CODEC_AAPC_STA_DEFAULT			0x0000

#define	CODEC_AAPC_STE_DISABLE			(0x0000<<5)
#define	CODEC_AAPC_STE_ENABLE			(0x0001<<5)
#define	CODEC_AAPC_STE_DEFAULT			CODEC_AAPC_STE_DISABLE

#define	CODEC_AAPC_DAC_OFF				(0x0000<<4)
#define	CODEC_AAPC_DAC_ON				(0x0001<<4)
#define	CODEC_AAPC_DAC_DEFAULT			CODEC_AAPC_DAC_ON

#define	CODEC_AAPC_BYP_DISABLE			(0x0000<<3)
#define	CODEC_AAPC_BYP_ENABLE			(0x0001<<3)
#define	CODEC_AAPC_BYP_DEFAULT			CODEC_AAPC_BYP_ENABLE

#define	CODEC_AAPC_INSEL_LINE			(0x0000<<2)
#define	CODEC_AAPC_INSEL_MICPHONE		(0x0001<<2)
#define	CODEC_AAPC_INSEL_DEFAULT		CODEC_AAPC_INSEL_LINE

#define	CODEC_AAPC_MICM_NORMAL			(0x0000<<1)
#define	CODEC_AAPC_MICM_MUTE			(0x0001<<1)
#define	CODEC_AAPC_MICM_DEFAULT			CODEC_AAPC_MICM_MUTE

#define	CODEC_AAPC_MICB_0DB				0x0000
#define	CODEC_AAPC_MICB_20DB			0x0001
#define	CODEC_AAPC_MICB_DEFAULT			CODEC_AAPC_MICB_0DB


//=============================================================//
//==Digital Audio Path Control
#define	CODEC_DAPC_RMK(dacm,deemp,adchp)		\
						((Uint16)(DAPC|dacm|deemp|adchp))

#define	CODEC_DAPC_DACM_DISABLE			(0x0000<<3)
#define	CODEC_DAPC_DACM_ENABLE			(0x0001<<3)
#define	CODEC_DAPC_DACM_DEFAULT			CODEC_DAPC_DACM_DISABLE

#define	CODEC_DAPC_DEEMP_DISABLE		(0x0000<<1)
#define	CODEC_DAPC_DEEMP_32K			(0x0001<<1)
#define	CODEC_DAPC_DEEMP_44K1			(0x0002<<1)
#define	CODEC_DAPC_DEEMP_48K			(0x0003<<1)
#define	CODEC_DAPC_DEEMP_DEFAULT		CODEC_DAPC_DEEMP_44K1

#define	CODEC_DAPC_ADCHP_DISABLE		0x0000
#define	CODEC_DAPC_ADCHP_ENABLE			0x0001
#define	CODEC_DAPC_ADCHP_DEFAULT		CODEC_DAPC_ADCHP_DISABLE


//=============================================================//
//==Power Down Control
#define	CODEC_PDC_RMK(off,clk,osc,out,dac,adc,mic,line)		\
						((Uint16)(PDC|off|clk|osc|out|dac|adc|mic|line))

#define	CODEC_PDC_OFF_ON				(0x0000<<7)
#define	CODEC_PDC_OFF_OFF				(0x0001<<7)
#define	CODEC_PDC_OFF_DEFAULT			CODEC_PDC_OFF_ON

#define	CODEC_PDC_CLK_ON				(0x0000<<6)
#define	CODEC_PDC_CLK_OFF				(0x0001<<6)
#define	CODEC_PDC_CLK_DEFAULT			CODEC_PDC_CLK_ON

#define	CODEC_PDC_OSC_ON				(0x0000<<5)
#define	CODEC_PDC_OSC_OFF				(0x0001<<5)
#define	CODEC_PDC_OSC_DEFAULT			CODEC_PDC_OSC_ON

#define	CODEC_PDC_OUT_ON				(0x0000<<4)
#define	CODEC_PDC_OUT_OFF				(0x0001<<4)
#define	CODEC_PDC_OUT_DEFAULT			CODEC_PDC_OUT_ON

#define	CODEC_PDC_DAC_ON				(0x0000<<3)
#define	CODEC_PDC_DAC_OFF				(0x0001<<3)
#define	CODEC_PDC_DAC_DEFAULT			CODEC_PDC_DAC_ON

#define	CODEC_PDC_ADC_ON				(0x0000<<2)
#define	CODEC_PDC_ADC_OFF				(0x0001<<2)
#define	CODEC_PDC_ADC_DEFAULT			CODEC_PDC_ADC_OFF

#define	CODEC_PDC_MIC_ON				(0x0000<<1)
#define	CODEC_PDC_MIC_OFF				(0x0001<<1)
#define	CODEC_PDC_MIC_DEFAULT			CODEC_PDC_MIC_OFF

#define	CODEC_PDC_LINE_ON				0x0000
#define	CODEC_PDC_LINE_OFF				0x0001
#define	CODEC_PDC_LINE_DEFAULT			CODEC_PDC_LINE_OFF


//=============================================================//
//==Digital Audio Interface Format
#define	CODEC_DAIF_RMK(ms,lrswap,lrp,iwl,format)		\
						((Uint16)(DAIF|ms|lrswap|lrp|iwl|format))

#define	CODEC_DAIF_MS_SLAVE				(0x0000<<6)
#define	CODEC_DAIF_MS_MASTER			(0x0001<<6)
#define	CODEC_DAIF_MS_DEFAULT			CODEC_DAIF_MS_SLAVE

#define	CODEC_DAIF_LRSWAP_DISABLE		(0x0000<<5)
#define	CODEC_DAIF_LRSWAP_ENABLE		(0x0001<<5)
#define	CODEC_DAIF_LRSWAP_DEFAULT		CODEC_DAIF_LRSWAP_DISABLE


#define	CODEC_DAIF_LRP_DELAY1CYCLE		(0x0000<<4)
#define	CODEC_DAIF_LRP_DELAY2CYCLE		(0x0001<<4)
#define	CODEC_DAIF_LRP_LRCIN_HIGH		(0x0000<<4)
#define	CODEC_DAIF_LRP_LRCIN_LOW		(0x0001<<4)
#define	CODEC_DAIF_LRP_DEFAULT			CODEC_DAIF_LRP_DELAY1CYCLE

#define	CODEC_DAIF_IWL_16BIT			(0x0000<<2)
#define	CODEC_DAIF_IWL_20BIT			(0x0001<<2)
#define	CODEC_DAIF_IWL_24BIT			(0x0002<<2)
#define	CODEC_DAIF_IWL_32BIT			(0x0003<<2)
#define	CODEC_DAIF_IWL_DEFAULT			CODEC_DAIF_IWL_16BIT

#define	CODEC_DAIF_FOR_RIGHT_ALIGNED	0x0000
#define	CODEC_DAIF_FOR_LEFT_ALIGNED		0x0001
#define	CODEC_DAIF_FOR_IIS				0x0002
#define	CODEC_DAIF_FOR_DSP				0x0003



//=============================================================//
//==Sample Rate Control
#define	CODEC_SRC_RMK(clkout,clkin,sr,bosr,mode)			\
						((Uint16)(SRC|clkout|clkin|sr|bosr|mode))

#define	CODEC_SRC_CLKOUT_MCLK			(0x0000<<7)
#define	CODEC_SRC_CLKOUT_HALF_MCLK		(0x0001<<7)	
#define	CODEC_SRC_CLKOUT_DEFAULT		CODEC_SRC_CLKOUT_MCLK

#define	CODEC_SRC_CLKIN_MCLK			(0x0000<<6)
#define	CODEC_SRC_CLKIN_HALF_MCLK		(0x0001<<6)	
#define	CODEC_SRC_CLKIN_DEFAULT			CODEC_SRC_CLKIN_MCLK

#define	CODEC_SRC_SR_OF(x)				(x<<2)
#define	CODEC_SRC_SR_DEFAULT			0x0008

#define	CODEC_SRC_BOSR_250FS			(0x0000<<1)
#define	CODEC_SRC_BOSR_256FS			(0x0000<<1)
#define	CODEC_SRC_BOSR_272FS			(0x0001<<1)
#define	CODEC_SRC_BOSR_384FS			(0x0001<<1)
#define	CODEC_SRC_BOSR_DEFAULT			CODEC_SRC_BOSR_250FS

#define	CODEC_SRC_MODE_NORMAL			0x0000
#define	CODEC_SRC_MODE_USB				0x0001
#define	CODEC_SRC_MODE_DEFAULT			CODEC_SRC_MODE_NORMAL


//=============================================================//
//==Digital Interface Activation
#define	CODEC_DIA_RMK(act)				((Uint16)(DIA|act))

#define CODEC_DIA_ACT_INACTIVE			0x0000
#define CODEC_DIA_ACT_ACTIVE			0x0001
#define	CODEC_DIA_ACT_DEFAULT			CODEC_DIA_ACT_INACTIVE


//=============================================================//
//==Reset Register
#define	CODEC_RST_RMK(res)				((Uint16)(RST|res))

#define	CODEC_RST_RES_RESET				0x0000


extern void Codec_Setup(AIC23B_Config *Config);
extern void Codec_Init(void);


#endif


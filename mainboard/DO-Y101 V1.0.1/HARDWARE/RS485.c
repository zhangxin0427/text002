#include "RS485.h"
#include "sys.h"
#include "stdlib.h" 	 
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "math.h"
#include "adc.h"
#include "dac1.h"
/* Private variables ---------------------------------------------------------*/ 
extern struct uart_data_buff uart1_buff;
extern float ccf;
extern float RES;

		u32 RingBuffer[32];
		u32 SumVolt,SumVolt1;
		u8 RbIndex;

u16 MODBUS_DATA_FRAME[39] = 
{
 250, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1013, 0, 0, 0,
 0x0100, 0x0001, 0x0000,          	//硬件版本:V1.0.0; 软件版本:V1.0.0
 0x444f, 0x2d59, 0x3130, 0x3000,  	//型号: DO-Y100
 0x0c1c, 0x0107, 0x0900, 0x0100,  	//序列号: 型号:3100 ID:01 年周:1801 流水号:0001
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

u16 MODBUS_DATA_VER[DATA_LEN_VER] = {0x0100, 0x0001, 0x0000};        		//硬件版本:V1.0.0; 软件版本:V1.0.0
u16 MODBUS_DATA_PN[DATA_LEN_PN] 	= {0x444f, 0x2d59, 0x3130, 0x3000};  	//型号: DO-Y100
u16 MODBUS_DATA_SN[DATA_LEN_SN] 	= {0x0c1c, 0x0107, 0x0900, 0x0100};  	//序列号: 型号:3100 ID:01 年周:1801 流水号:0001						  

//配置数据

u8 gs_bEmpty = 1;


u8  gs_u8_Addr485 	 = 1;                 //传感器地址，[1-127], 默认:1
u8  gs_u8_Salinity 	 = 0;                 //盐度值，单位:1/1000, 范围[0-40], 默认:0
u16 gs_u16_Pressure  = 1013;             	//大气压力值，单位:0.1kPa, 默认:1013, 高字节在前

u16 gs_u16_CaliPhase = 9000;           		//零氧校准时的相位差值，单位:0.0001, 范围[0-13000], 默认:9000
u16 gs_u16_CaliT0 	 = 250;               //零氧校准时的温度值，单位:1/10℃, 范围[0-600], 默认:250

u16 gs_u16_CaliPhaseB  = 2300;            //标准时的相位值，单位:0.0001, 范围[0-13000], 默认:2300
u16 gs_u16_CaliTemp  = 250;             	//标准时的温度值，单位:1/10℃, 范围[0-600], 默认:250

u16 gs_u16_CaliT_BC  = 2000;              //温度补偿值，单位:0.0001℃，默认:0
u16 gs_u16_CaliT_BCFLAG   = 0;					  //温度补偿趋势标志
u16 gs_u16_CaliT_BCFactor = 10000;				//温度补偿系数（未使用）

u16 gs_u16_CaliDO    = 825;               //校准温度对应的饱和氧，单位:0.01
u16 gs_u16_CaliDO_BC = 0;                 //溶氧计算模型常数项，单位:0.01，默认:0
u16 gs_u16_TcFactor  = 41;              	//零氧相位温度系数，单位:0.0001, 默认:41

u16 gs_u16_KsvFactor = 3000;          		//Stern-Volmer系数, 单位:0.0001, 默认:0
u16 gs_u16_KsvFactor_T 		 		 = 168;     //Stern-Volmer系数随温度变化的系数，单位:0.0001, 默认:168

u16 gs_u16_DOFIXPHASE_Base 		 = 1400;    //相位延时基准值
u16 gs_u16_DOFIXPHASE_Factor_T = 0;       //相位延时温度系数
u16 gs_u16_DOFIXPHASE_Flag     = 0;       //相位延时随温度增加而增加，为1，否则为0；

//传感器原始数据采集
u16 gs_u16_PhaseDiff = 0;              		//荧光相位差值

//传感器计算
u16 gs_u16_Percent 	 = 1000;             		//溶解氧饱和度,单位:0.1
u16 gs_u16_TCDO    	 = 0;                   //溶解氧温度补偿后的值
u16 gs_u16_SCDO      = 0;                   //溶解氧盐度补偿后的值
u16 gs_u16_PCDO      = 0;                   //溶解氧大气压力补偿后的值

//温度采集
u16 gs_u16_TempValue = 250;               //传感器温度值，单位:1/10℃, 范围[0-600], 默认:250

//传感器警报状态
_Bool gs_b1_SnrErr   = 0;                  	//传感器异常
_Bool gs_b1_CalErr   = 0;                  	//校准异常

/* Private function prototypes -----------------------------------------------*/
void mem_copy_byte(u8 *source, u8 *direct, u8 length);
void FLASH_Load_Data(void);
void FDO_Phase_Acquisition(void);
void Temp_Value_Update(void);
u8 DO_Calibration(u8 DataRev_485);
void DO_Value_Calculation(void);
u16 Salinity_Compensation(void);
void Do_Value_Update(void);
void Comeback_err(u8 comend_ack);

////////////////////////////////////////////////////////////////////////////////// 
/** 拷贝字符型数组，内部函数 ****************************************************/
void mem_copy_byte(u8 *source, u8 *direct, u8 length)
{
	while(length--)
	{
		*direct++ = *source++;
	}
}

/** 更新FLASH数据，外部函数 *****************************************************/
void FLASH_Load_Data(void)
{
	u8  Addr485 	= 1;
	u8  Salinity 	= 0;
  u16 TCFactor 	= 0;
	u16 KsvFactor = 0;
	u16 Pressure 	= 1013; 
	u16 Data_Version[DATA_LEN_VER];
	
  //float  Cal_Value = 0;
	u8  	 i 				 = 0;
	
	//加载盐度值
	Salinity = (u8)STMFLASH_ReadHalfWord(FLASH_ADDR_SALINITY);
	if (40 < Salinity)
	{
	  gs_u8_Salinity = 40;
		MODBUS_DATA_FRAME[ADDR_SAL] = (u16)400;
	}
	else
	{
	  gs_u8_Salinity = Salinity;
		MODBUS_DATA_FRAME[ADDR_SAL] = (u16)(Salinity*10);
	}
	
	TCFactor = STMFLASH_ReadHalfWord(FLASH_ADDR_TCFACTOR);
	gs_u16_TcFactor = TCFactor;
	MODBUS_DATA_FRAME[ADDR_R5] = TCFactor;
	
	
	KsvFactor = STMFLASH_ReadHalfWord(FLASH_ADDR_KSVFACTOR);
	gs_u16_KsvFactor = KsvFactor;
	MODBUS_DATA_FRAME[ADDR_FACT] = KsvFactor;
	
	//加载大气压力值
	Pressure = STMFLASH_ReadHalfWord(FLASH_ADDR_PRESSURE);
	if (1075 < Pressure)  		//-500 Meter
	{
	  gs_u16_Pressure = 1075;
		MODBUS_DATA_FRAME[ADDR_PRE] = 1075;
	}
	else if (540 > Pressure)  //+5000 Meter
	{
	  gs_u16_Pressure = 540;
		MODBUS_DATA_FRAME[ADDR_PRE] = 540;
	}
	else
	{
	  gs_u16_Pressure = Pressure;
		MODBUS_DATA_FRAME[ADDR_PRE] = Pressure;
	}
	
	//加载485地址
	Addr485 = (u8)STMFLASH_ReadHalfWord(FLASH_ADDR_SNSR_ADDR);
	if ((1 > Addr485)||(127 < Addr485))
	{
	  gs_u8_Addr485 = 1;
		MODBUS_DATA_FRAME[ADDR_485] = 1;
	}
	else
	{
	  gs_u8_Addr485 = Addr485;
		MODBUS_DATA_FRAME[ADDR_485] = (u16)Addr485;
	}
	
	//加载传感器版本号
  STMFLASH_Read(FLASH_ADDR_SNSR_VER, Data_Version, DATA_LEN_VER);
	MODBUS_DATA_VER[0] = Data_Version[0];                   //更新高3位
	Data_Version[1] &= 0xff00;
	MODBUS_DATA_VER[1] |= Data_Version[1];

	for (i=0;i<DATA_LEN_VER;i++)
	{
		MODBUS_DATA_FRAME[ADDR_VER+i] = MODBUS_DATA_VER[i];  //更新MODBUS缓存
	}	
	
	//加载传感器型号
	STMFLASH_Read(FLASH_ADDR_SNSR_PN, MODBUS_DATA_PN, DATA_LEN_PN);		
	for (i=0;i<DATA_LEN_PN;i++)
	{
		MODBUS_DATA_FRAME[ADDR_PN+i] = MODBUS_DATA_PN[i];    //更新MODBUS缓存
	}
		
	//加载传感器序列号
	STMFLASH_Read(FLASH_ADDR_SNSR_SN, MODBUS_DATA_SN, DATA_LEN_SN);		
	for (i=0;i<DATA_LEN_SN;i++)
	{
		MODBUS_DATA_FRAME[ADDR_SN+i] = MODBUS_DATA_SN[i];     //更新MODBUS缓存
	}
	
	//加载标定零氧时的相位差值和温度值
	gs_u16_CaliPhase = STMFLASH_ReadHalfWord(FLASH_ADDR_CALIPH0);
	MODBUS_DATA_FRAME[ADDR_R1] = gs_u16_CaliPhase;   				//TEST
	if (0 < gs_u16_PhaseDiff)  															//范围判断
	{
	    ;
	}
	else
	{
	    ;
	}
	
	
	//加载实时温度补偿趋势标志
	gs_u16_CaliT_BCFLAG = STMFLASH_ReadHalfWord(FLASH_ADDR_CaliT_BCFLAG);      //温度补偿
	MODBUS_DATA_FRAME[ADDR_R10] = gs_u16_CaliT_BCFLAG;   					             //TEST
	
	//加载实时温度系数
	//gs_u16_CaliT0_BCFactor = STMFLASH_ReadHalfWord(FLASH_ADDR_CaliT0_BCFactor);   //温度补偿
	//MODBUS_DATA_FRAME[ADDR_R10] = gs_u16_CaliT0_BCFactor;   					            //TEST
	
	//加载实时温度校准时的值
	gs_u16_CaliT_BC = STMFLASH_ReadHalfWord(FLASH_ADDR_CaliT_BC);                   //温度补偿
	MODBUS_DATA_FRAME[ADDR_R11] = gs_u16_CaliT_BC;   					 //TEST
	
	//加载零氧校准时的温度值
	gs_u16_CaliT0 = STMFLASH_ReadHalfWord(FLASH_ADDR_CALIT0);
	MODBUS_DATA_FRAME[ADDR_R2] = gs_u16_CaliT0;   					 //TEST
	
	//加载饱和氧校准时的温度值
	gs_u16_CaliTemp = STMFLASH_ReadHalfWord(FLASH_ADDR_CALITEMP);	
	MODBUS_DATA_FRAME[ADDR_R4] = gs_u16_CaliTemp;
	
	//加载饱和氧校准时的相位值
	gs_u16_CaliPhaseB = STMFLASH_ReadHalfWord(FLASH_ADDR_CALITPHASE);	
	MODBUS_DATA_FRAME[ADDR_NCAIL] = gs_u16_CaliPhaseB;
	
	//加载延时相位基准值
	gs_u16_DOFIXPHASE_Base = STMFLASH_ReadHalfWord(FLASH_ADDR_DOFIXPHASE_Base);	
	MODBUS_DATA_FRAME[ADDR_R6] = gs_u16_DOFIXPHASE_Base;
	
  //加载延时相位温度系数 
	gs_u16_DOFIXPHASE_Factor_T = STMFLASH_ReadHalfWord(FLASH_ADDR_DOFIXPHASE_Factor_T);	
	MODBUS_DATA_FRAME[ADDR_R7] = gs_u16_DOFIXPHASE_Factor_T;
	
	//加载延时相位趋势标志  
	gs_u16_DOFIXPHASE_Flag = STMFLASH_ReadHalfWord(FLASH_ADDR_DOFIXPHASE_Flag);	
	MODBUS_DATA_FRAME[ADDR_R8] = gs_u16_DOFIXPHASE_Flag;
	
	//加载Stern-Volmer系数随温度变化的系数
	gs_u16_KsvFactor_T = STMFLASH_ReadHalfWord(FLASH_ADDR_KsvFactor_T);	
	MODBUS_DATA_FRAME[ADDR_R9] = gs_u16_KsvFactor_T;
	
	//不同温度下对应的饱和氧:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
	//得到测量温度下的饱和氧，保留2位小数
	//Cal_Value 	  = (-0.0075*(gs_u16_CaliTemp*gs_u16_CaliTemp*gs_u16_CaliTemp))/1000 + (0.7945*(gs_u16_CaliTemp*gs_u16_CaliTemp))/100 - (40.506*gs_u16_CaliTemp)/10 + 1460.1 + 0.5; 
	//gs_u16_CaliDO = (u16)Cal_Value;	
}

/** 荧光相位差获取，内部函数 ****************************************************/
void FDO_Phase_Acquisition(void)
{
		u8 i;			
		//获取荧光相位差值Phase，带4位小数
		float Red_fft1,Blue_fft1,sub,Red_fft2,Blue_fft2;
		u32 Sub_fft,Sub_fft_end;
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);//蓝光
		Blue_fft1=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1); 
		GPIO_SetBits(GPIOA,GPIO_Pin_3);  //红光
		Red_fft1=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1); 
		Red_fft2=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1);
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);//蓝光
		Blue_fft2=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1); 
		
		sub=(float)fabs(Red_fft1-Blue_fft1+Red_fft2-Blue_fft2)/2;
		Sub_fft=fabs(Red_fft1-Blue_fft1+Red_fft2-Blue_fft2)/2;

	/*
	
		if (1 == gs_bEmpty)   
		{
			for (i = 0; i < 32; i++)
			{
				RingBuffer[i] = (u32)Sub_fft;
			}
				
			SumVolt = Sub_fft * 32;
			gs_bEmpty = 0;
		}
		else               
		{
			SumVolt -= RingBuffer[RbIndex];
			SumVolt += Sub_fft;		
			RingBuffer[RbIndex] = Sub_fft;         
		
			RbIndex++;
			if (RbIndex >= 32)
			{
				RbIndex = 0;
			}
			else
			{
					;
			}
		} 
		SumVolt1 =SumVolt/32;
		Sub_fft_end = SumVolt1;	
		ccf=tan(PI*Sub_fft_end/18000)*10000;
	*/	
		
		ccf=tan(PI*Sub_fft/18000)*10000;
		gs_u16_PhaseDiff 	 = (u16)ccf;                                                                                                                                                   
     MODBUS_DATA_FRAME[ADDR_R3] = (u16)ccf;
}

/** 温度更新，外部函数 **********************************************************/
void Temp_Value_Update(void)
{
	u16 	TempValue = 0;
	float RseValue;

	//获取PT1000的电阻值RseValue
	RseValue = RES;
 
	if (PT1000_MIN_VALUE > RseValue) 
	{
			RseValue = PT1000_MIN_VALUE;    	//温度下限0℃对应的电阻值
	}
	else if (PT1000_MAX_VALUE < RseValue)
	{
			RseValue = PT1000_MAX_VALUE;    	//温度上限100℃对应的电阻值
	}
	else
	{
			;
	}	
	
	//温度值计算
	TempValue = (u16)((RseValue - 1000.0)/0.3851);

	if ((u16)DO_MAX_TEMP < TempValue)    //温度范围第一次限制[0-60℃],防止移动平均滤波时溢出
	{
		TempValue = (u16)DO_MAX_TEMP;  				
	}
	else
	{
		;			
	}
	
	RseValue = (float)(gs_u16_CaliT_BCFactor)/10000;
	
	//MODBUS_DATA_FRAME[ADDR_TEMP_tmp] = TempValue;
	
	if(0==gs_u16_CaliT_BCFLAG)
	{
		gs_u16_TempValue             		 = TempValue*RseValue + (gs_u16_CaliT_BC/1000);
	  MODBUS_DATA_FRAME[ADDR_TEMP]     = TempValue*RseValue + (gs_u16_CaliT_BC/1000);
	}
  else
  {
		gs_u16_TempValue             		 = TempValue*RseValue - (gs_u16_CaliT_BC/1000);
	  MODBUS_DATA_FRAME[ADDR_TEMP]     = TempValue*RseValue - (gs_u16_CaliT_BC/1000);
	}		
}
 
/** DO校准函数，内部函数 ********************************************************/
u8 DO_Calibration(u8 DataRev_485)
{
  u8  Ret = 0;
	u16 DO_FIX_PHASE;
	float Cal_Value;
	
	if (gs_u16_PhaseDiff > MAX_PhaseDiff)
	{
		//相位差超上限，校准异常
		gs_b1_CalErr = 1;
		return (Ret);
	}	
	else
	{
				;
	}
	
	switch(DataRev_485)
	{
	  case 0x01:          											         		//零氧校准
			Ret = 1;
			MODBUS_DATA_FRAME[ADDR_R1] = gs_u16_PhaseDiff;   		//TEST
			gs_u16_CaliPhase					 = gs_u16_PhaseDiff;   		//更新零氧校准时的相位差值
			Test_Write(FLASH_ADDR_CALIPH0, (u16)gs_u16_PhaseDiff);	
			
			MODBUS_DATA_FRAME[ADDR_R2] = gs_u16_TempValue;   		//TEST
			gs_u16_CaliT0  					   = gs_u16_TempValue;   		//更新零氧校准时的温度值
			Test_Write(FLASH_ADDR_CALIT0, (u16)gs_u16_TempValue);
		
		break;
			
		case 0x02:          											            //饱和氧校准
			MODBUS_DATA_FRAME[ADDR_NCAIL] = gs_u16_PhaseDiff;   //TEST
			gs_u16_CaliPhaseB					 		= gs_u16_PhaseDiff;   //更新零氧校准时的相位差值
			Test_Write(FLASH_ADDR_CALITPHASE, (u16)gs_u16_PhaseDiff);	
		
			gs_u16_CaliTemp            =  gs_u16_TempValue;
			MODBUS_DATA_FRAME[ADDR_R4] =  gs_u16_TempValue;		  //TEST
			Test_Write(FLASH_ADDR_CALITEMP, (u16)gs_u16_TempValue);
			
			//不同温度下对应的饱和氧:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
			//得到测量温度下的饱和氧，保留2位小数			
			Cal_Value 		= (-0.0075*(gs_u16_CaliTemp*gs_u16_CaliTemp*gs_u16_CaliTemp))/1000 + (0.7945*(gs_u16_CaliTemp*gs_u16_CaliTemp))/100 - (40.506*gs_u16_CaliTemp)/10 + 1460.1 + 0.5; 				
			gs_u16_CaliDO = (u16)Cal_Value;
						
			//溶解氧计算模型
			//根据Stern-Volmer方程,计算Stern-Volmer常数Ksv
			//F0/F = 1+Ksv*[Q]
			//Ksv = (F0/F-1)/[Q]
			//温度补偿，计算饱和氧校准温度下的tao_0值：gs_u16_CaliPhase
			if (gs_u16_CaliTemp > gs_u16_CaliT0)
			{
					Cal_Value = gs_u16_CaliPhase - ((gs_u16_CaliTemp - gs_u16_CaliT0)*gs_u16_TcFactor)/10;	
				  
			}
			else
			{
					Cal_Value = gs_u16_CaliPhase + ((gs_u16_CaliT0 - gs_u16_CaliTemp)*gs_u16_TcFactor)/10;		
			}	

      if(gs_u16_DOFIXPHASE_Flag == 0)
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base - (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;  //相位延时温度系数
			}		
			else
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base + (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;			
			}
			
			if (gs_u16_PhaseDiff < (u16)Cal_Value)
			{
				  Cal_Value = (Cal_Value - gs_u16_CaliPhaseB )/(gs_u16_CaliPhaseB - DO_FIX_PHASE);
					Cal_Value = ((Cal_Value*10000)/(gs_u16_CaliDO - gs_u16_CaliDO_BC))*100;          //计算斜率，保留4位小数		
			}
			else
			{
			    return (Ret);	
			}
					
			//FLASH存储			
			gs_u16_KsvFactor             = (u16)Cal_Value;
			MODBUS_DATA_FRAME[ADDR_FACT] = (u16)Cal_Value;
			Test_Write(FLASH_ADDR_KSVFACTOR, (u16)Cal_Value);
								
			//增加校准判断及状态返回
			/*
			Temp1 = (1.0 - (float)DO_CALI_RANGE) * Cal_Value;   //校准下限
			Temp2 = (1.0 + (float)DO_CALI_RANGE) * Cal_Value;   //校准上限
			if ((gs_u16_TCDO < Temp1) || (gs_u16_TCDO > Temp2))
			{
			    gs_b1_CalErr = 1;     //超出了校准范围，校准异常
					return (Ret);					
			}
			else
			{
			    Ret = 1;
					gs_b1_CalErr = 0;     //校准正常				  
			}
			*/
			Ret = 1;
			
		break;
			
		default:		
		break;
	}  //switch(DataRev_485)
	
	return (Ret);
}

/** 溶氧值计算，内部函数 ********************************************************/
void DO_Value_Calculation(void)
{
	float Cal_Value, Ratio, Sat_DO;
	u16 DO_FIX_PHASE;
	u32 tmp;
	FDO_Phase_Acquisition();    	//获取原始相位差值，带2位小数
	
	//不同温度下对应的饱和溶氧值:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
	//得到测量温度下的饱和氧，保留2位小数	
	Sat_DO = ((gs_u16_TempValue*gs_u16_TempValue*gs_u16_TempValue)*(-0.0075))/1000+((gs_u16_TempValue*gs_u16_TempValue)*0.7945)/100-(40.506*gs_u16_TempValue)/10+1460.1+0.5; 

	//温度补偿，计算当前温度下的tao_0,延时相位值：gs_u16_CaliPhase
	if (gs_u16_TempValue > gs_u16_CaliT0)
		{	
		 Cal_Value = gs_u16_CaliPhase - ((gs_u16_TempValue - gs_u16_CaliT0)*gs_u16_TcFactor)/10;		   
		}
	else
		{	
		 Cal_Value = gs_u16_CaliPhase + ((gs_u16_CaliT0 - gs_u16_TempValue)*gs_u16_TcFactor)/10;		
		}	
	//温度补偿，计算当前温度下的延时相位值：DO_FIX_PHASE
	if(gs_u16_DOFIXPHASE_Flag == 0)
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base - (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;  //相位延时温度系数
			}		
			else
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base + (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;			
			}
	
	if (gs_u16_PhaseDiff > (u16)Cal_Value)
			{
				//相位差大于零氧校准时的相位差
				gs_u16_TCDO                  = 0;
				gs_u16_Percent					     = 0; 	
				MODBUS_DATA_FRAME[ADDR_PERC] = 0;
				return;		
			}
	else
			{
				;
			}
	
	//根据Stern-Volmer方程,计算溶解氧浓度
	//F0/F = 1+Ksv*[Q]
	//[Q] = (F0/F-1)/Ksv	
	Cal_Value = (Cal_Value - gs_u16_PhaseDiff)/(gs_u16_PhaseDiff - DO_FIX_PHASE);
	//gs_u16_PhaseDiff：荧光相位差值；DO_FIX_PHASE：当前温度相位
	
	if (gs_u16_TempValue > gs_u16_CaliTemp)
		{
			tmp   = gs_u16_KsvFactor_T * (gs_u16_TempValue - gs_u16_CaliTemp);//gs_u16_KsvFactor_T：KSV的温度系数；gs_u16_TempValue：传感器温度值；gs_u16_CaliTemp：标准时的温度值
			Ratio = gs_u16_KsvFactor * exp(((float)tmp)/100000);//KSV系数
		}
	else
		{
			tmp   = gs_u16_KsvFactor_T * (gs_u16_CaliTemp - gs_u16_TempValue);
			Ratio = gs_u16_KsvFactor*exp(-((float)tmp)/100000);
		}	
	
	MODBUS_DATA_FRAME[ADDR_FACT]=	Ratio;  //ksv系数
	Cal_Value = ((Cal_Value*10000)/Ratio)*100;   		//带2位小数
	Cal_Value += gs_u16_CaliDO_BC;      						//带2位小数	
	
	//计算当前经温度补偿后的溶解氧值
	if ((u16)DO_MAX_VALUE < gs_u16_TCDO)    				//溶氧上限设定20mg/L
		{
	    gs_u16_TCDO = (u16)DO_MAX_VALUE;
		}
	else
		{
			gs_u16_TCDO = (u16)Cal_Value;
		}
	
	gs_u16_Percent 							 = (u16)((gs_u16_TCDO / Sat_DO)*1000); 	
	MODBUS_DATA_FRAME[ADDR_PERC] = (u16)((gs_u16_TCDO / Sat_DO)*1000); //饱和度
}

/** 盐度补偿，内部函数 **********************************************************/
u16 Salinity_Compensation(void)
{
	u16 Do_Value = 0;              //盐度补偿后的DO值
	float TempValue = 0;
	
	//DO_salt(T) = DO(theory) - Solid(T)*n                 (公式1)
	//Solid(T) = 10-3(0.037T2 - 2.912T +91.231), T[0, 40]  (公式2)
	//计算盐度因子 Solid_T :公式2 * 10000后, 数据范围:[340-925]
	//TempValue = (Solid_T * Salinity * 100) / 10000 + 0.5;  //乘以100保留2位小数
	
	TempValue = (((0.37*(gs_u16_TempValue*gs_u16_TempValue))/100 - (29.12*gs_u16_TempValue)/10 + 912.31 + 0.5)*gs_u8_Salinity)/100;
	
	if (gs_u16_TCDO > (u16)TempValue)
	{
	    Do_Value = gs_u16_TCDO - (u16)(TempValue);
	}
	else
	{
	    Do_Value = 0;
	}	
	
	//大气压力补偿
	//lnPw = 12.062 - 4039.558/(T+235.379)                 (公式3) 计算饱和水蒸汽压力，T单位℃，Pw单位bar = 100kPa
	//Cs' = Cs*(P-Pw)/(101.3-Pw)                           (公式4) 大气压力补偿后的溶氧值，大气压力单位:kPa

	TempValue  = 1000/exp(4039.558/(0.1*gs_u16_TempValue+235.379) - 12.062);
	TempValue  = ((gs_u16_Pressure-TempValue)/(1013-TempValue))*Do_Value;
	Do_Value   = (u16)TempValue;
	
	return (Do_Value);
}

/** 溶氧更新，外部函数 **********************************************************/
void Do_Value_Update(void)
{
	u16 Do_Value = 0;
	DO_Value_Calculation();//溶氧值计算
	Do_Value = Salinity_Compensation();//添加盐度补偿后的溶氧值
	
  if ((u16)DO_MAX_VALUE < Do_Value)                  //溶氧上限设定21mg/L
  {
	    gs_u16_SCDO 							 = (u16)DO_MAX_VALUE;
			MODBUS_DATA_FRAME[ADDR_DO] = (u16)DO_MAX_VALUE;
	}
	else
	{
	    gs_u16_SCDO 							 = (u16)Do_Value;
			MODBUS_DATA_FRAME[ADDR_DO] = (u16)Do_Value;
	}
}

/** UART1一帧数据接收完成，进行命令字解析，外部函数(main调用) *******************/
void UART1_com(void)
{
	u8  Ret=0;
	u8  i=0,j=0;
	u8  crc_datah, crc_datal;          //CRC校验码
	u8  comend_Len, comend_Buff[128];  //MODBUS帧数据BUFF
	u16 temp=0;
	float Cal_Value;
	u16 DO_FIX_PHASE;
	
	  comend_Len = uart1_buff.rx_len;
		mem_copy_byte((u8 *)uart1_buff.rx_data_buff, (u8 *)comend_Buff, comend_Len);//缓存数据
			
		//帧设备ID数据解析
		if (comend_Buff[0] != gs_u8_Addr485)	//如果不是本机设备地址码，则丢弃本帧数据
		{ 
			return ;
		}
		
		//CRC校验帧数据		
		temp = CRC16((u8 *)comend_Buff, (comend_Len-2));
		crc_datah = (u8)((temp>>8)&0x00ff);
		crc_datal = (u8)(temp&0x00ff);
		
		if ((comend_Buff[comend_Len-2] != crc_datah)||(comend_Buff[comend_Len-1] != crc_datal))
		{	
			//return ;	//CRC校验错误，则丢弃本帧数据
			Comeback_err(ERROR_CRC);
		}
		 
		//数据深度检查，若超出数据深度，则返回错误信息
		if (comend_Buff[3] > 32)
		{
			//return ;
			Comeback_err(ERROR_LEN);
		}
		
		switch(comend_Buff[1])
		{
			case 0x03:																				//读命令字
				 uart1_buff.tx_len = comend_Buff[5]*2+5;  			//数据总长度：地址1; 指令1; 数据长度1; 数据temp*2; CRC码2
				 delay_ms(2);
				 uart1_buff.tx_data_buff[0] = gs_u8_Addr485;		//复制数据帧头：地址
				 uart1_buff.tx_data_buff[1] = comend_Buff[1];		//复制数据帧头：功能码（指令）
				 uart1_buff.tx_data_buff[2] = comend_Buff[5]*2;	//需要返回的数据长度，comend_Buff[5]为数据长度，单位为WORD，所以乘以2
				 i = 3;       																	//索引号变为3
				 
				 for (j=0;j<comend_Buff[5];j++)
				 {
						uart1_buff.tx_data_buff[i++] = (u8)(MODBUS_DATA_FRAME[comend_Buff[3]+j]/256);   //高字节在前，comend_Buff[3]数据首地址
						uart1_buff.tx_data_buff[i++] = (u8)(MODBUS_DATA_FRAME[comend_Buff[3]+j]%256);   //低字节在后
				 }
				 
				 delay_ms(2);
				 temp = CRC16((u8 *)uart1_buff.tx_data_buff, (uart1_buff.tx_len-2));
				 uart1_buff.tx_data_buff[i++] = (u8)((temp>>8)&0x00ff);		//CRC校验高位
				 uart1_buff.tx_data_buff[i] 	= (u8)(temp&0x00ff); 		    //CRC校验低位
				 USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
						 
			break;
						
			case 0x06:															//写单个命令字
					uart1_buff.tx_len = comend_Len;     //返回数据帧长度
					for (i=0;i<comend_Len;i++)
					{
							uart1_buff.tx_data_buff[i] = comend_Buff[i];
					}
					//USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);

					temp = comend_Buff[4]*256 + comend_Buff[5];   //提取接收数据
					
					switch(comend_Buff[3])                        //数据首地址解析
					{
							case 0x07:		//校准指令
								switch(comend_Buff[5])
								{
										case 0x01:		//零氧校准
											Ret = DO_Calibration(0x01);										
											if (Ret != 0)
											{
												USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
											}
											else
											{
												//return ;
												Comeback_err(ERROR_CLI);
											}
										break;

										case 0x02:		//饱和氧校准
											Ret = DO_Calibration(0x02);										
											if (Ret != 0)
											{
												USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
											}
											else
											{
												//return ;
												Comeback_err(ERROR_CLI);
											}
										break;

										case 0x0A:		//恢复出厂默认设置
											gs_u8_Salinity = 0;
											MODBUS_DATA_FRAME[ADDR_SAL] = 0; 
											Test_Write(FLASH_ADDR_SALINITY, 0);
											
											gs_u16_Pressure = 1013;
											MODBUS_DATA_FRAME[ADDR_PRE] = 1013;										
											Test_Write(FLASH_ADDR_PRESSURE, 1013);
										
											gs_u16_TcFactor = 41;
										  MODBUS_DATA_FRAME[ADDR_R5] = 41;//TEST
											Test_Write(FLASH_ADDR_TCFACTOR, 41);
										
											///////////////////////////////////////////
											////标定零氧时的相关参数
											gs_u16_CaliPhase = 8800;
											MODBUS_DATA_FRAME[ADDR_R1] = 8800;//TEST
											Test_Write(FLASH_ADDR_CALIPH0, (u16)8800);
											
											gs_u16_CaliT0 = 250;
											MODBUS_DATA_FRAME[ADDR_R2] = 250;//TEST
											Test_Write(FLASH_ADDR_CALIT0, (u16)250);
											///////////////////////////////////////////
											
											///////////////////////////////////////////
											////标定饱和氧时的相关参数
											gs_u16_PhaseDiff = 3069;
											MODBUS_DATA_FRAME[ADDR_R3] = 3069;//TEST
											Test_Write(FLASH_ADDR_CALITPHASE, (u16)3069);
											
											gs_u16_CaliTemp = 250;
											MODBUS_DATA_FRAME[ADDR_R4] = 250;//TEST
											Test_Write(FLASH_ADDR_CALITEMP, (u16)250);
											///////////////////////////////////////////	
											
											///////////////////////////////////////////
											////系统固定参数
											gs_u16_DOFIXPHASE_Base = 1400;
											MODBUS_DATA_FRAME[ADDR_R6] = 1400;
											Test_Write(FLASH_ADDR_DOFIXPHASE_Base, gs_u16_DOFIXPHASE_Base);	
											
											gs_u16_DOFIXPHASE_Factor_T = 10;
											MODBUS_DATA_FRAME[ADDR_R7] = 10;
											Test_Write(FLASH_ADDR_DOFIXPHASE_Factor_T, gs_u16_DOFIXPHASE_Factor_T);	
											
											gs_u16_DOFIXPHASE_Flag = 0;
								      MODBUS_DATA_FRAME[ADDR_R8] = 0;
								      Test_Write(FLASH_ADDR_DOFIXPHASE_Flag, gs_u16_DOFIXPHASE_Flag);	
											
											gs_u16_KsvFactor_T = 165;
								      MODBUS_DATA_FRAME[ADDR_R9] = 165;
								      Test_Write(FLASH_ADDR_KsvFactor_T, gs_u16_KsvFactor_T);		
											///////////////////////////////////////////	
											
											//不同温度下对应的饱和氧:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
											//得到测量温度下的饱和氧，保留2位小数			
											Cal_Value 		= (-0.0075*(gs_u16_CaliTemp*gs_u16_CaliTemp*gs_u16_CaliTemp))/1000 + (0.7945*(gs_u16_CaliTemp*gs_u16_CaliTemp))/100 - (40.506*gs_u16_CaliTemp)/10 + 1460.1 + 0.5; 				
											gs_u16_CaliDO = (u16)Cal_Value;
						
											//溶解氧计算模型
											//根据Stern-Volmer方程,计算Stern-Volmer常数Ksv
											//F0/F = 1+Ksv*[Q]
											//Ksv = (F0/F-1)/[Q]
											//温度补偿，计算饱和氧校准温度下的tao_0值：gs_u16_CaliPhase
											if (gs_u16_CaliTemp > gs_u16_CaliT0)
												{
													Cal_Value = gs_u16_CaliPhase - ((gs_u16_CaliTemp - gs_u16_CaliT0)*gs_u16_TcFactor)/10;	
												}
											else
												{
													Cal_Value = gs_u16_CaliPhase + ((gs_u16_CaliT0 - gs_u16_CaliTemp)*gs_u16_TcFactor)/10;		
												}	

											if(gs_u16_DOFIXPHASE_Flag == 0)
												{
													DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base - (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;  //相位延时温度系数
												}		
											else
												{
													DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base + (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;			
												}
			
											if ((gs_u16_PhaseDiff < (u16)Cal_Value)&&(gs_u16_PhaseDiff > DO_FIX_PHASE))
											  {
													Cal_Value = (Cal_Value - gs_u16_PhaseDiff )/(gs_u16_PhaseDiff - DO_FIX_PHASE);
													Cal_Value = ((Cal_Value*10000)/(gs_u16_CaliDO - gs_u16_CaliDO_BC))*100;          //计算斜率，保留4位小数		
												}
											else
												{
													//return (Ret);	
												}
												
					          //Ret=1;
										//FLASH存储			
										gs_u16_KsvFactor             = (u16)Cal_Value;
										MODBUS_DATA_FRAME[ADDR_FACT] = (u16)Cal_Value;
										Test_Write(FLASH_ADDR_KSVFACTOR, (u16)Cal_Value);
	
										USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
										break;
										
										case 0x80:		//复位指令
											USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
											delay_ms(10);
											__set_FAULTMASK(1);  //关闭所有中断
											NVIC_SystemReset();  //复位函数
										break;
										
										case 0x68:		//升级指令
											USART1_SendTxData("\n\rUpdate file!", 15);
											iap_load_app(FLASH_Bootloader_ADDR);
										default: 
										break;
							}
							break;
								
							case 0x09:		//修改485地址
								if ((1 > temp)||(127 < temp))
								{
										gs_u8_Addr485 = 1;
								}
								else
								{
										gs_u8_Addr485 = comend_Buff[5];
								}
								                     
								MODBUS_DATA_FRAME[ADDR_485] = (u16)gs_u8_Addr485;       //更新全局变量
								Test_Write(FLASH_ADDR_SNSR_ADDR, (u16)gs_u8_Addr485);   //更新FLASH数据
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
							
							case 0x0A:		//修改盐度值, 单位1/1000,范围[0-400],默认0
							 	if (400 < temp)
								{
										gs_u8_Salinity = 40;
										MODBUS_DATA_FRAME[ADDR_SAL] = (u16)400;
								}
								else
								{
										gs_u8_Salinity = (u8)(temp/10);
										MODBUS_DATA_FRAME[ADDR_SAL] = temp;
								}
								Test_Write(FLASH_ADDR_SALINITY, (u16)gs_u8_Salinity);  //更新FLASH数据
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
								
							case 0x0B:		//修改零氧值的温度补偿系数								
								if (MAX_TcFactor < temp)
								{ 
									gs_u16_TcFactor 					 = MAX_TcFactor;
								  MODBUS_DATA_FRAME[ADDR_R5] = MAX_TcFactor;
								}	
								else
								{	
									gs_u16_TcFactor = temp;
									MODBUS_DATA_FRAME[ADDR_R5] = temp;
								}	
								Test_Write(FLASH_ADDR_TCFACTOR, gs_u16_TcFactor);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x0F:		//修改实时温度校准趋势标志	
    						if (0 == temp)		
								{
									gs_u16_CaliT_BCFLAG = temp;
								  MODBUS_DATA_FRAME[ADDR_R10] = temp;
								}
                else
                {
									gs_u16_CaliT_BCFLAG = 1;
								  MODBUS_DATA_FRAME[ADDR_R10] = 1;
								}	
								Test_Write(FLASH_ADDR_CaliT_BCFLAG, gs_u16_CaliT_BCFLAG);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x10:		//修改实时温度校准值						
								gs_u16_CaliT_BC = temp;
								MODBUS_DATA_FRAME[ADDR_R11] = temp;
								Test_Write(FLASH_ADDR_CaliT_BC, gs_u16_CaliT_BC);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x1C:		//修改延时相位基准值	
                if (MAX_DOFIXPHASE_Base < temp)	
								{
									gs_u16_DOFIXPHASE_Base 		 = MAX_DOFIXPHASE_Base;
									MODBUS_DATA_FRAME[ADDR_R6] = MAX_DOFIXPHASE_Base;
								}
								else
								{
									gs_u16_DOFIXPHASE_Base 		 = temp;
									MODBUS_DATA_FRAME[ADDR_R6] = temp;
								}	
								Test_Write(FLASH_ADDR_DOFIXPHASE_Base, gs_u16_DOFIXPHASE_Base);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x1D:		//修改延时相位温度系数
								if (MAX_DOFIXPHASE_Factor_T < temp)	
								{
									gs_u16_DOFIXPHASE_Factor_T = MAX_DOFIXPHASE_Factor_T;
								  MODBUS_DATA_FRAME[ADDR_R7] = MAX_DOFIXPHASE_Factor_T;
								}	
								else
								{
									gs_u16_DOFIXPHASE_Factor_T = temp;
								  MODBUS_DATA_FRAME[ADDR_R7] = temp;
								}		
								Test_Write(FLASH_ADDR_DOFIXPHASE_Factor_T, gs_u16_DOFIXPHASE_Factor_T);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x1E:		//修改延时相位趋势标志	
                if(	0	== temp	)					
								{
									gs_u16_DOFIXPHASE_Flag     = temp;
								  MODBUS_DATA_FRAME[ADDR_R8] = temp;
								}
                else
								{
									gs_u16_DOFIXPHASE_Flag 		 = 1;
								  MODBUS_DATA_FRAME[ADDR_R8] = 1;
								}	
								Test_Write(FLASH_ADDR_DOFIXPHASE_Flag, gs_u16_DOFIXPHASE_Flag);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x1F:		//Stern-Volmer系数随温度变化的系数	
                if(MAX_KsvFactor_T < temp)								
								{
									gs_u16_KsvFactor_T         = MAX_KsvFactor_T;
								  MODBUS_DATA_FRAME[ADDR_R9] = MAX_KsvFactor_T;
								}	
								else
								{	
									gs_u16_KsvFactor_T         = temp;
								  MODBUS_DATA_FRAME[ADDR_R9] = temp;
								}	
								Test_Write(FLASH_ADDR_KsvFactor_T, gs_u16_KsvFactor_T);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
								
						/*	
							case 0x02:		//更新零氧校准时的相位差值							
								MODBUS_DATA_FRAME[ADDR_R1] = temp;   //TEST
								gs_u16_CaliPhase					 = temp;   //更新零氧校准时的相位差值
								Test_Write(FLASH_ADDR_CALIPH0, (u16)gs_u16_CaliPhase);
							  USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
							
			        case 0x03:
								MODBUS_DATA_FRAME[ADDR_R2] = temp;   //TEST
								gs_u16_CaliT0  					   = temp;   //更新零氧校准时的温度值
								Test_Write(FLASH_ADDR_CALIT0, (u16)gs_u16_CaliT0);							
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
             */
								
							case 0x0D:		//修改大气压力值
							  if (1013 < temp)
								{
										temp = 1013;
								}
								else if (540 > temp)
								{
										temp = 540;
								}
								else
								{
										;
								}
								gs_u16_Pressure = temp;                           				//更新全局变量
								MODBUS_DATA_FRAME[ADDR_PRE] = temp;
								Test_Write(FLASH_ADDR_PRESSURE, (u16)temp);    						//更新FLASH数据
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;

							case 0x11:		//17: 版本号
									          //复制接收到的版本号，只接收硬件版本号（前3位）						
									MODBUS_DATA_VER[0] = comend_Buff[4]*256 + comend_Buff[5];
									temp = (u16)(comend_Buff[6]*256);           
									MODBUS_DATA_VER[1] &= 0x00ff;
									MODBUS_DATA_VER[1] |= temp;
							
									for (i=0;i<DATA_LEN_VER;i++)
									{
											MODBUS_DATA_FRAME[ADDR_VER+i] = MODBUS_DATA_VER[i];  //更新MODBUS缓存
									}
									
									STMFLASH_Write(FLASH_ADDR_SNSR_VER, MODBUS_DATA_VER, (u16)DATA_LEN_VER);														
									USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;

							case 0x14:		//20: PN								
									          //复制接收到的型号
									for (i=0;i<DATA_LEN_PN;i++)
									{
											temp = comend_Buff[4+2*i]*256 + comend_Buff[5+2*i];
											MODBUS_DATA_PN[i] = temp;             //更新PN
											MODBUS_DATA_FRAME[ADDR_PN+i] = temp;  //更新MODBUS缓存
									}
									
									STMFLASH_Write(FLASH_ADDR_SNSR_PN, MODBUS_DATA_PN, (u16)DATA_LEN_PN);
									USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;

							case 0x18:		//24: SN								
									//复制接收到的序列号
									for (i=0;i<DATA_LEN_SN;i++)
									{
											temp = comend_Buff[4+2*i]*256 + comend_Buff[5+2*i];
											MODBUS_DATA_SN[i] = temp;             //更新PN
											MODBUS_DATA_FRAME[ADDR_SN+i] = temp;  //更新MODBUS缓存
									}
									
									STMFLASH_Write(FLASH_ADDR_SNSR_SN, MODBUS_DATA_SN, (u16)DATA_LEN_SN);								
									USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;																	 

							default: 
							break;	
					}  //switch(comend_Buff[3])  //数据首地址解析

					break;
					default: 
						
				break;
		}  //switch(comend_Buff[1])  //命令字解析
}

/** 帧错误返回，内部函数 ********************************************************/
void Comeback_err(u8 comend_ack)
{
	u8 i = 0;
	u16 temp;
	
	uart1_buff.tx_len = 0x05;
	uart1_buff.tx_data_buff[i] = MODBUS_DATA_FRAME[ADDR_485];			 //帧头，传感器地址
	i++;
	uart1_buff.tx_data_buff[i] = uart1_buff.rx_data_buff[1]+0x80;  //命令字
	i++;
	uart1_buff.tx_data_buff[i] = comend_ack;					        		 //错误代码
	i++;
	temp = CRC16((u8 *)uart1_buff.tx_data_buff, (uart1_buff.tx_len-2));
	uart1_buff.tx_data_buff[i++] = (u8)((temp>>8)&0x00ff);	  		 //CRC校验高位
	uart1_buff.tx_data_buff[i] = (u8)(temp&0x00ff);		     	  		 //CRC校验低位

	USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);					
}

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
 0x0100, 0x0001, 0x0000,          	//Ӳ���汾:V1.0.0; ����汾:V1.0.0
 0x444f, 0x2d59, 0x3130, 0x3000,  	//�ͺ�: DO-Y100
 0x0c1c, 0x0107, 0x0900, 0x0100,  	//���к�: �ͺ�:3100 ID:01 ����:1801 ��ˮ��:0001
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

u16 MODBUS_DATA_VER[DATA_LEN_VER] = {0x0100, 0x0001, 0x0000};        		//Ӳ���汾:V1.0.0; ����汾:V1.0.0
u16 MODBUS_DATA_PN[DATA_LEN_PN] 	= {0x444f, 0x2d59, 0x3130, 0x3000};  	//�ͺ�: DO-Y100
u16 MODBUS_DATA_SN[DATA_LEN_SN] 	= {0x0c1c, 0x0107, 0x0900, 0x0100};  	//���к�: �ͺ�:3100 ID:01 ����:1801 ��ˮ��:0001						  

//��������

u8 gs_bEmpty = 1;


u8  gs_u8_Addr485 	 = 1;                 //��������ַ��[1-127], Ĭ��:1
u8  gs_u8_Salinity 	 = 0;                 //�ζ�ֵ����λ:1/1000, ��Χ[0-40], Ĭ��:0
u16 gs_u16_Pressure  = 1013;             	//����ѹ��ֵ����λ:0.1kPa, Ĭ��:1013, ���ֽ���ǰ

u16 gs_u16_CaliPhase = 9000;           		//����У׼ʱ����λ��ֵ����λ:0.0001, ��Χ[0-13000], Ĭ��:9000
u16 gs_u16_CaliT0 	 = 250;               //����У׼ʱ���¶�ֵ����λ:1/10��, ��Χ[0-600], Ĭ��:250

u16 gs_u16_CaliPhaseB  = 2300;            //��׼ʱ����λֵ����λ:0.0001, ��Χ[0-13000], Ĭ��:2300
u16 gs_u16_CaliTemp  = 250;             	//��׼ʱ���¶�ֵ����λ:1/10��, ��Χ[0-600], Ĭ��:250

u16 gs_u16_CaliT_BC  = 2000;              //�¶Ȳ���ֵ����λ:0.0001�棬Ĭ��:0
u16 gs_u16_CaliT_BCFLAG   = 0;					  //�¶Ȳ������Ʊ�־
u16 gs_u16_CaliT_BCFactor = 10000;				//�¶Ȳ���ϵ����δʹ�ã�

u16 gs_u16_CaliDO    = 825;               //У׼�¶ȶ�Ӧ�ı���������λ:0.01
u16 gs_u16_CaliDO_BC = 0;                 //��������ģ�ͳ������λ:0.01��Ĭ��:0
u16 gs_u16_TcFactor  = 41;              	//������λ�¶�ϵ������λ:0.0001, Ĭ��:41

u16 gs_u16_KsvFactor = 3000;          		//Stern-Volmerϵ��, ��λ:0.0001, Ĭ��:0
u16 gs_u16_KsvFactor_T 		 		 = 168;     //Stern-Volmerϵ�����¶ȱ仯��ϵ������λ:0.0001, Ĭ��:168

u16 gs_u16_DOFIXPHASE_Base 		 = 1400;    //��λ��ʱ��׼ֵ
u16 gs_u16_DOFIXPHASE_Factor_T = 0;       //��λ��ʱ�¶�ϵ��
u16 gs_u16_DOFIXPHASE_Flag     = 0;       //��λ��ʱ���¶����Ӷ����ӣ�Ϊ1������Ϊ0��

//������ԭʼ���ݲɼ�
u16 gs_u16_PhaseDiff = 0;              		//ӫ����λ��ֵ

//����������
u16 gs_u16_Percent 	 = 1000;             		//�ܽ������Ͷ�,��λ:0.1
u16 gs_u16_TCDO    	 = 0;                   //�ܽ����¶Ȳ������ֵ
u16 gs_u16_SCDO      = 0;                   //�ܽ����ζȲ������ֵ
u16 gs_u16_PCDO      = 0;                   //�ܽ�������ѹ���������ֵ

//�¶Ȳɼ�
u16 gs_u16_TempValue = 250;               //�������¶�ֵ����λ:1/10��, ��Χ[0-600], Ĭ��:250

//����������״̬
_Bool gs_b1_SnrErr   = 0;                  	//�������쳣
_Bool gs_b1_CalErr   = 0;                  	//У׼�쳣

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
/** �����ַ������飬�ڲ����� ****************************************************/
void mem_copy_byte(u8 *source, u8 *direct, u8 length)
{
	while(length--)
	{
		*direct++ = *source++;
	}
}

/** ����FLASH���ݣ��ⲿ���� *****************************************************/
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
	
	//�����ζ�ֵ
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
	
	//���ش���ѹ��ֵ
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
	
	//����485��ַ
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
	
	//���ش������汾��
  STMFLASH_Read(FLASH_ADDR_SNSR_VER, Data_Version, DATA_LEN_VER);
	MODBUS_DATA_VER[0] = Data_Version[0];                   //���¸�3λ
	Data_Version[1] &= 0xff00;
	MODBUS_DATA_VER[1] |= Data_Version[1];

	for (i=0;i<DATA_LEN_VER;i++)
	{
		MODBUS_DATA_FRAME[ADDR_VER+i] = MODBUS_DATA_VER[i];  //����MODBUS����
	}	
	
	//���ش������ͺ�
	STMFLASH_Read(FLASH_ADDR_SNSR_PN, MODBUS_DATA_PN, DATA_LEN_PN);		
	for (i=0;i<DATA_LEN_PN;i++)
	{
		MODBUS_DATA_FRAME[ADDR_PN+i] = MODBUS_DATA_PN[i];    //����MODBUS����
	}
		
	//���ش��������к�
	STMFLASH_Read(FLASH_ADDR_SNSR_SN, MODBUS_DATA_SN, DATA_LEN_SN);		
	for (i=0;i<DATA_LEN_SN;i++)
	{
		MODBUS_DATA_FRAME[ADDR_SN+i] = MODBUS_DATA_SN[i];     //����MODBUS����
	}
	
	//���ر궨����ʱ����λ��ֵ���¶�ֵ
	gs_u16_CaliPhase = STMFLASH_ReadHalfWord(FLASH_ADDR_CALIPH0);
	MODBUS_DATA_FRAME[ADDR_R1] = gs_u16_CaliPhase;   				//TEST
	if (0 < gs_u16_PhaseDiff)  															//��Χ�ж�
	{
	    ;
	}
	else
	{
	    ;
	}
	
	
	//����ʵʱ�¶Ȳ������Ʊ�־
	gs_u16_CaliT_BCFLAG = STMFLASH_ReadHalfWord(FLASH_ADDR_CaliT_BCFLAG);      //�¶Ȳ���
	MODBUS_DATA_FRAME[ADDR_R10] = gs_u16_CaliT_BCFLAG;   					             //TEST
	
	//����ʵʱ�¶�ϵ��
	//gs_u16_CaliT0_BCFactor = STMFLASH_ReadHalfWord(FLASH_ADDR_CaliT0_BCFactor);   //�¶Ȳ���
	//MODBUS_DATA_FRAME[ADDR_R10] = gs_u16_CaliT0_BCFactor;   					            //TEST
	
	//����ʵʱ�¶�У׼ʱ��ֵ
	gs_u16_CaliT_BC = STMFLASH_ReadHalfWord(FLASH_ADDR_CaliT_BC);                   //�¶Ȳ���
	MODBUS_DATA_FRAME[ADDR_R11] = gs_u16_CaliT_BC;   					 //TEST
	
	//��������У׼ʱ���¶�ֵ
	gs_u16_CaliT0 = STMFLASH_ReadHalfWord(FLASH_ADDR_CALIT0);
	MODBUS_DATA_FRAME[ADDR_R2] = gs_u16_CaliT0;   					 //TEST
	
	//���ر�����У׼ʱ���¶�ֵ
	gs_u16_CaliTemp = STMFLASH_ReadHalfWord(FLASH_ADDR_CALITEMP);	
	MODBUS_DATA_FRAME[ADDR_R4] = gs_u16_CaliTemp;
	
	//���ر�����У׼ʱ����λֵ
	gs_u16_CaliPhaseB = STMFLASH_ReadHalfWord(FLASH_ADDR_CALITPHASE);	
	MODBUS_DATA_FRAME[ADDR_NCAIL] = gs_u16_CaliPhaseB;
	
	//������ʱ��λ��׼ֵ
	gs_u16_DOFIXPHASE_Base = STMFLASH_ReadHalfWord(FLASH_ADDR_DOFIXPHASE_Base);	
	MODBUS_DATA_FRAME[ADDR_R6] = gs_u16_DOFIXPHASE_Base;
	
  //������ʱ��λ�¶�ϵ�� 
	gs_u16_DOFIXPHASE_Factor_T = STMFLASH_ReadHalfWord(FLASH_ADDR_DOFIXPHASE_Factor_T);	
	MODBUS_DATA_FRAME[ADDR_R7] = gs_u16_DOFIXPHASE_Factor_T;
	
	//������ʱ��λ���Ʊ�־  
	gs_u16_DOFIXPHASE_Flag = STMFLASH_ReadHalfWord(FLASH_ADDR_DOFIXPHASE_Flag);	
	MODBUS_DATA_FRAME[ADDR_R8] = gs_u16_DOFIXPHASE_Flag;
	
	//����Stern-Volmerϵ�����¶ȱ仯��ϵ��
	gs_u16_KsvFactor_T = STMFLASH_ReadHalfWord(FLASH_ADDR_KsvFactor_T);	
	MODBUS_DATA_FRAME[ADDR_R9] = gs_u16_KsvFactor_T;
	
	//��ͬ�¶��¶�Ӧ�ı�����:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
	//�õ������¶��µı�����������2λС��
	//Cal_Value 	  = (-0.0075*(gs_u16_CaliTemp*gs_u16_CaliTemp*gs_u16_CaliTemp))/1000 + (0.7945*(gs_u16_CaliTemp*gs_u16_CaliTemp))/100 - (40.506*gs_u16_CaliTemp)/10 + 1460.1 + 0.5; 
	//gs_u16_CaliDO = (u16)Cal_Value;	
}

/** ӫ����λ���ȡ���ڲ����� ****************************************************/
void FDO_Phase_Acquisition(void)
{
		u8 i;			
		//��ȡӫ����λ��ֵPhase����4λС��
		float Red_fft1,Blue_fft1,sub,Red_fft2,Blue_fft2;
		u32 Sub_fft,Sub_fft_end;
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);//����
		Blue_fft1=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1); 
		GPIO_SetBits(GPIOA,GPIO_Pin_3);  //���
		Red_fft1=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1); 
		Red_fft2=Get_Adc_Average(ADC_Channel_10,11);
		delay_ms(1);
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);//����
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

/** �¶ȸ��£��ⲿ���� **********************************************************/
void Temp_Value_Update(void)
{
	u16 	TempValue = 0;
	float RseValue;

	//��ȡPT1000�ĵ���ֵRseValue
	RseValue = RES;
 
	if (PT1000_MIN_VALUE > RseValue) 
	{
			RseValue = PT1000_MIN_VALUE;    	//�¶�����0���Ӧ�ĵ���ֵ
	}
	else if (PT1000_MAX_VALUE < RseValue)
	{
			RseValue = PT1000_MAX_VALUE;    	//�¶�����100���Ӧ�ĵ���ֵ
	}
	else
	{
			;
	}	
	
	//�¶�ֵ����
	TempValue = (u16)((RseValue - 1000.0)/0.3851);

	if ((u16)DO_MAX_TEMP < TempValue)    //�¶ȷ�Χ��һ������[0-60��],��ֹ�ƶ�ƽ���˲�ʱ���
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
 
/** DOУ׼�������ڲ����� ********************************************************/
u8 DO_Calibration(u8 DataRev_485)
{
  u8  Ret = 0;
	u16 DO_FIX_PHASE;
	float Cal_Value;
	
	if (gs_u16_PhaseDiff > MAX_PhaseDiff)
	{
		//��λ����ޣ�У׼�쳣
		gs_b1_CalErr = 1;
		return (Ret);
	}	
	else
	{
				;
	}
	
	switch(DataRev_485)
	{
	  case 0x01:          											         		//����У׼
			Ret = 1;
			MODBUS_DATA_FRAME[ADDR_R1] = gs_u16_PhaseDiff;   		//TEST
			gs_u16_CaliPhase					 = gs_u16_PhaseDiff;   		//��������У׼ʱ����λ��ֵ
			Test_Write(FLASH_ADDR_CALIPH0, (u16)gs_u16_PhaseDiff);	
			
			MODBUS_DATA_FRAME[ADDR_R2] = gs_u16_TempValue;   		//TEST
			gs_u16_CaliT0  					   = gs_u16_TempValue;   		//��������У׼ʱ���¶�ֵ
			Test_Write(FLASH_ADDR_CALIT0, (u16)gs_u16_TempValue);
		
		break;
			
		case 0x02:          											            //������У׼
			MODBUS_DATA_FRAME[ADDR_NCAIL] = gs_u16_PhaseDiff;   //TEST
			gs_u16_CaliPhaseB					 		= gs_u16_PhaseDiff;   //��������У׼ʱ����λ��ֵ
			Test_Write(FLASH_ADDR_CALITPHASE, (u16)gs_u16_PhaseDiff);	
		
			gs_u16_CaliTemp            =  gs_u16_TempValue;
			MODBUS_DATA_FRAME[ADDR_R4] =  gs_u16_TempValue;		  //TEST
			Test_Write(FLASH_ADDR_CALITEMP, (u16)gs_u16_TempValue);
			
			//��ͬ�¶��¶�Ӧ�ı�����:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
			//�õ������¶��µı�����������2λС��			
			Cal_Value 		= (-0.0075*(gs_u16_CaliTemp*gs_u16_CaliTemp*gs_u16_CaliTemp))/1000 + (0.7945*(gs_u16_CaliTemp*gs_u16_CaliTemp))/100 - (40.506*gs_u16_CaliTemp)/10 + 1460.1 + 0.5; 				
			gs_u16_CaliDO = (u16)Cal_Value;
						
			//�ܽ�������ģ��
			//����Stern-Volmer����,����Stern-Volmer����Ksv
			//F0/F = 1+Ksv*[Q]
			//Ksv = (F0/F-1)/[Q]
			//�¶Ȳ��������㱥����У׼�¶��µ�tao_0ֵ��gs_u16_CaliPhase
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
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base - (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;  //��λ��ʱ�¶�ϵ��
			}		
			else
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base + (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;			
			}
			
			if (gs_u16_PhaseDiff < (u16)Cal_Value)
			{
				  Cal_Value = (Cal_Value - gs_u16_CaliPhaseB )/(gs_u16_CaliPhaseB - DO_FIX_PHASE);
					Cal_Value = ((Cal_Value*10000)/(gs_u16_CaliDO - gs_u16_CaliDO_BC))*100;          //����б�ʣ�����4λС��		
			}
			else
			{
			    return (Ret);	
			}
					
			//FLASH�洢			
			gs_u16_KsvFactor             = (u16)Cal_Value;
			MODBUS_DATA_FRAME[ADDR_FACT] = (u16)Cal_Value;
			Test_Write(FLASH_ADDR_KSVFACTOR, (u16)Cal_Value);
								
			//����У׼�жϼ�״̬����
			/*
			Temp1 = (1.0 - (float)DO_CALI_RANGE) * Cal_Value;   //У׼����
			Temp2 = (1.0 + (float)DO_CALI_RANGE) * Cal_Value;   //У׼����
			if ((gs_u16_TCDO < Temp1) || (gs_u16_TCDO > Temp2))
			{
			    gs_b1_CalErr = 1;     //������У׼��Χ��У׼�쳣
					return (Ret);					
			}
			else
			{
			    Ret = 1;
					gs_b1_CalErr = 0;     //У׼����				  
			}
			*/
			Ret = 1;
			
		break;
			
		default:		
		break;
	}  //switch(DataRev_485)
	
	return (Ret);
}

/** ����ֵ���㣬�ڲ����� ********************************************************/
void DO_Value_Calculation(void)
{
	float Cal_Value, Ratio, Sat_DO;
	u16 DO_FIX_PHASE;
	u32 tmp;
	FDO_Phase_Acquisition();    	//��ȡԭʼ��λ��ֵ����2λС��
	
	//��ͬ�¶��¶�Ӧ�ı�������ֵ:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
	//�õ������¶��µı�����������2λС��	
	Sat_DO = ((gs_u16_TempValue*gs_u16_TempValue*gs_u16_TempValue)*(-0.0075))/1000+((gs_u16_TempValue*gs_u16_TempValue)*0.7945)/100-(40.506*gs_u16_TempValue)/10+1460.1+0.5; 

	//�¶Ȳ��������㵱ǰ�¶��µ�tao_0,��ʱ��λֵ��gs_u16_CaliPhase
	if (gs_u16_TempValue > gs_u16_CaliT0)
		{	
		 Cal_Value = gs_u16_CaliPhase - ((gs_u16_TempValue - gs_u16_CaliT0)*gs_u16_TcFactor)/10;		   
		}
	else
		{	
		 Cal_Value = gs_u16_CaliPhase + ((gs_u16_CaliT0 - gs_u16_TempValue)*gs_u16_TcFactor)/10;		
		}	
	//�¶Ȳ��������㵱ǰ�¶��µ���ʱ��λֵ��DO_FIX_PHASE
	if(gs_u16_DOFIXPHASE_Flag == 0)
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base - (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;  //��λ��ʱ�¶�ϵ��
			}		
			else
			{
					DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base + (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;			
			}
	
	if (gs_u16_PhaseDiff > (u16)Cal_Value)
			{
				//��λ���������У׼ʱ����λ��
				gs_u16_TCDO                  = 0;
				gs_u16_Percent					     = 0; 	
				MODBUS_DATA_FRAME[ADDR_PERC] = 0;
				return;		
			}
	else
			{
				;
			}
	
	//����Stern-Volmer����,�����ܽ���Ũ��
	//F0/F = 1+Ksv*[Q]
	//[Q] = (F0/F-1)/Ksv	
	Cal_Value = (Cal_Value - gs_u16_PhaseDiff)/(gs_u16_PhaseDiff - DO_FIX_PHASE);
	//gs_u16_PhaseDiff��ӫ����λ��ֵ��DO_FIX_PHASE����ǰ�¶���λ
	
	if (gs_u16_TempValue > gs_u16_CaliTemp)
		{
			tmp   = gs_u16_KsvFactor_T * (gs_u16_TempValue - gs_u16_CaliTemp);//gs_u16_KsvFactor_T��KSV���¶�ϵ����gs_u16_TempValue���������¶�ֵ��gs_u16_CaliTemp����׼ʱ���¶�ֵ
			Ratio = gs_u16_KsvFactor * exp(((float)tmp)/100000);//KSVϵ��
		}
	else
		{
			tmp   = gs_u16_KsvFactor_T * (gs_u16_CaliTemp - gs_u16_TempValue);
			Ratio = gs_u16_KsvFactor*exp(-((float)tmp)/100000);
		}	
	
	MODBUS_DATA_FRAME[ADDR_FACT]=	Ratio;  //ksvϵ��
	Cal_Value = ((Cal_Value*10000)/Ratio)*100;   		//��2λС��
	Cal_Value += gs_u16_CaliDO_BC;      						//��2λС��	
	
	//���㵱ǰ���¶Ȳ�������ܽ���ֵ
	if ((u16)DO_MAX_VALUE < gs_u16_TCDO)    				//���������趨20mg/L
		{
	    gs_u16_TCDO = (u16)DO_MAX_VALUE;
		}
	else
		{
			gs_u16_TCDO = (u16)Cal_Value;
		}
	
	gs_u16_Percent 							 = (u16)((gs_u16_TCDO / Sat_DO)*1000); 	
	MODBUS_DATA_FRAME[ADDR_PERC] = (u16)((gs_u16_TCDO / Sat_DO)*1000); //���Ͷ�
}

/** �ζȲ������ڲ����� **********************************************************/
u16 Salinity_Compensation(void)
{
	u16 Do_Value = 0;              //�ζȲ������DOֵ
	float TempValue = 0;
	
	//DO_salt(T) = DO(theory) - Solid(T)*n                 (��ʽ1)
	//Solid(T) = 10-3(0.037T2 - 2.912T +91.231), T[0, 40]  (��ʽ2)
	//�����ζ����� Solid_T :��ʽ2 * 10000��, ���ݷ�Χ:[340-925]
	//TempValue = (Solid_T * Salinity * 100) / 10000 + 0.5;  //����100����2λС��
	
	TempValue = (((0.37*(gs_u16_TempValue*gs_u16_TempValue))/100 - (29.12*gs_u16_TempValue)/10 + 912.31 + 0.5)*gs_u8_Salinity)/100;
	
	if (gs_u16_TCDO > (u16)TempValue)
	{
	    Do_Value = gs_u16_TCDO - (u16)(TempValue);
	}
	else
	{
	    Do_Value = 0;
	}	
	
	//����ѹ������
	//lnPw = 12.062 - 4039.558/(T+235.379)                 (��ʽ3) ���㱥��ˮ����ѹ����T��λ�棬Pw��λbar = 100kPa
	//Cs' = Cs*(P-Pw)/(101.3-Pw)                           (��ʽ4) ����ѹ�������������ֵ������ѹ����λ:kPa

	TempValue  = 1000/exp(4039.558/(0.1*gs_u16_TempValue+235.379) - 12.062);
	TempValue  = ((gs_u16_Pressure-TempValue)/(1013-TempValue))*Do_Value;
	Do_Value   = (u16)TempValue;
	
	return (Do_Value);
}

/** �������£��ⲿ���� **********************************************************/
void Do_Value_Update(void)
{
	u16 Do_Value = 0;
	DO_Value_Calculation();//����ֵ����
	Do_Value = Salinity_Compensation();//����ζȲ����������ֵ
	
  if ((u16)DO_MAX_VALUE < Do_Value)                  //���������趨21mg/L
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

/** UART1һ֡���ݽ�����ɣ����������ֽ������ⲿ����(main����) *******************/
void UART1_com(void)
{
	u8  Ret=0;
	u8  i=0,j=0;
	u8  crc_datah, crc_datal;          //CRCУ����
	u8  comend_Len, comend_Buff[128];  //MODBUS֡����BUFF
	u16 temp=0;
	float Cal_Value;
	u16 DO_FIX_PHASE;
	
	  comend_Len = uart1_buff.rx_len;
		mem_copy_byte((u8 *)uart1_buff.rx_data_buff, (u8 *)comend_Buff, comend_Len);//��������
			
		//֡�豸ID���ݽ���
		if (comend_Buff[0] != gs_u8_Addr485)	//������Ǳ����豸��ַ�룬������֡����
		{ 
			return ;
		}
		
		//CRCУ��֡����		
		temp = CRC16((u8 *)comend_Buff, (comend_Len-2));
		crc_datah = (u8)((temp>>8)&0x00ff);
		crc_datal = (u8)(temp&0x00ff);
		
		if ((comend_Buff[comend_Len-2] != crc_datah)||(comend_Buff[comend_Len-1] != crc_datal))
		{	
			//return ;	//CRCУ�����������֡����
			Comeback_err(ERROR_CRC);
		}
		 
		//������ȼ�飬������������ȣ��򷵻ش�����Ϣ
		if (comend_Buff[3] > 32)
		{
			//return ;
			Comeback_err(ERROR_LEN);
		}
		
		switch(comend_Buff[1])
		{
			case 0x03:																				//��������
				 uart1_buff.tx_len = comend_Buff[5]*2+5;  			//�����ܳ��ȣ���ַ1; ָ��1; ���ݳ���1; ����temp*2; CRC��2
				 delay_ms(2);
				 uart1_buff.tx_data_buff[0] = gs_u8_Addr485;		//��������֡ͷ����ַ
				 uart1_buff.tx_data_buff[1] = comend_Buff[1];		//��������֡ͷ�������루ָ�
				 uart1_buff.tx_data_buff[2] = comend_Buff[5]*2;	//��Ҫ���ص����ݳ��ȣ�comend_Buff[5]Ϊ���ݳ��ȣ���λΪWORD�����Գ���2
				 i = 3;       																	//�����ű�Ϊ3
				 
				 for (j=0;j<comend_Buff[5];j++)
				 {
						uart1_buff.tx_data_buff[i++] = (u8)(MODBUS_DATA_FRAME[comend_Buff[3]+j]/256);   //���ֽ���ǰ��comend_Buff[3]�����׵�ַ
						uart1_buff.tx_data_buff[i++] = (u8)(MODBUS_DATA_FRAME[comend_Buff[3]+j]%256);   //���ֽ��ں�
				 }
				 
				 delay_ms(2);
				 temp = CRC16((u8 *)uart1_buff.tx_data_buff, (uart1_buff.tx_len-2));
				 uart1_buff.tx_data_buff[i++] = (u8)((temp>>8)&0x00ff);		//CRCУ���λ
				 uart1_buff.tx_data_buff[i] 	= (u8)(temp&0x00ff); 		    //CRCУ���λ
				 USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
						 
			break;
						
			case 0x06:															//д����������
					uart1_buff.tx_len = comend_Len;     //��������֡����
					for (i=0;i<comend_Len;i++)
					{
							uart1_buff.tx_data_buff[i] = comend_Buff[i];
					}
					//USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);

					temp = comend_Buff[4]*256 + comend_Buff[5];   //��ȡ��������
					
					switch(comend_Buff[3])                        //�����׵�ַ����
					{
							case 0x07:		//У׼ָ��
								switch(comend_Buff[5])
								{
										case 0x01:		//����У׼
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

										case 0x02:		//������У׼
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

										case 0x0A:		//�ָ�����Ĭ������
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
											////�궨����ʱ����ز���
											gs_u16_CaliPhase = 8800;
											MODBUS_DATA_FRAME[ADDR_R1] = 8800;//TEST
											Test_Write(FLASH_ADDR_CALIPH0, (u16)8800);
											
											gs_u16_CaliT0 = 250;
											MODBUS_DATA_FRAME[ADDR_R2] = 250;//TEST
											Test_Write(FLASH_ADDR_CALIT0, (u16)250);
											///////////////////////////////////////////
											
											///////////////////////////////////////////
											////�궨������ʱ����ز���
											gs_u16_PhaseDiff = 3069;
											MODBUS_DATA_FRAME[ADDR_R3] = 3069;//TEST
											Test_Write(FLASH_ADDR_CALITPHASE, (u16)3069);
											
											gs_u16_CaliTemp = 250;
											MODBUS_DATA_FRAME[ADDR_R4] = 250;//TEST
											Test_Write(FLASH_ADDR_CALITEMP, (u16)250);
											///////////////////////////////////////////	
											
											///////////////////////////////////////////
											////ϵͳ�̶�����
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
											
											//��ͬ�¶��¶�Ӧ�ı�����:y = (-0.0075*x*x*x+0.7945*x*x-40.506*x+1460.1)
											//�õ������¶��µı�����������2λС��			
											Cal_Value 		= (-0.0075*(gs_u16_CaliTemp*gs_u16_CaliTemp*gs_u16_CaliTemp))/1000 + (0.7945*(gs_u16_CaliTemp*gs_u16_CaliTemp))/100 - (40.506*gs_u16_CaliTemp)/10 + 1460.1 + 0.5; 				
											gs_u16_CaliDO = (u16)Cal_Value;
						
											//�ܽ�������ģ��
											//����Stern-Volmer����,����Stern-Volmer����Ksv
											//F0/F = 1+Ksv*[Q]
											//Ksv = (F0/F-1)/[Q]
											//�¶Ȳ��������㱥����У׼�¶��µ�tao_0ֵ��gs_u16_CaliPhase
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
													DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base - (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;  //��λ��ʱ�¶�ϵ��
												}		
											else
												{
													DO_FIX_PHASE = gs_u16_DOFIXPHASE_Base + (gs_u16_TempValue*gs_u16_DOFIXPHASE_Factor_T)/100;			
												}
			
											if ((gs_u16_PhaseDiff < (u16)Cal_Value)&&(gs_u16_PhaseDiff > DO_FIX_PHASE))
											  {
													Cal_Value = (Cal_Value - gs_u16_PhaseDiff )/(gs_u16_PhaseDiff - DO_FIX_PHASE);
													Cal_Value = ((Cal_Value*10000)/(gs_u16_CaliDO - gs_u16_CaliDO_BC))*100;          //����б�ʣ�����4λС��		
												}
											else
												{
													//return (Ret);	
												}
												
					          //Ret=1;
										//FLASH�洢			
										gs_u16_KsvFactor             = (u16)Cal_Value;
										MODBUS_DATA_FRAME[ADDR_FACT] = (u16)Cal_Value;
										Test_Write(FLASH_ADDR_KSVFACTOR, (u16)Cal_Value);
	
										USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
										break;
										
										case 0x80:		//��λָ��
											USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
											delay_ms(10);
											__set_FAULTMASK(1);  //�ر������ж�
											NVIC_SystemReset();  //��λ����
										break;
										
										case 0x68:		//����ָ��
											USART1_SendTxData("\n\rUpdate file!", 15);
											iap_load_app(FLASH_Bootloader_ADDR);
										default: 
										break;
							}
							break;
								
							case 0x09:		//�޸�485��ַ
								if ((1 > temp)||(127 < temp))
								{
										gs_u8_Addr485 = 1;
								}
								else
								{
										gs_u8_Addr485 = comend_Buff[5];
								}
								                     
								MODBUS_DATA_FRAME[ADDR_485] = (u16)gs_u8_Addr485;       //����ȫ�ֱ���
								Test_Write(FLASH_ADDR_SNSR_ADDR, (u16)gs_u8_Addr485);   //����FLASH����
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
							
							case 0x0A:		//�޸��ζ�ֵ, ��λ1/1000,��Χ[0-400],Ĭ��0
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
								Test_Write(FLASH_ADDR_SALINITY, (u16)gs_u8_Salinity);  //����FLASH����
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
								
							case 0x0B:		//�޸�����ֵ���¶Ȳ���ϵ��								
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
							
							case 0x0F:		//�޸�ʵʱ�¶�У׼���Ʊ�־	
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
							
							case 0x10:		//�޸�ʵʱ�¶�У׼ֵ						
								gs_u16_CaliT_BC = temp;
								MODBUS_DATA_FRAME[ADDR_R11] = temp;
								Test_Write(FLASH_ADDR_CaliT_BC, gs_u16_CaliT_BC);								
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;	
							
							case 0x1C:		//�޸���ʱ��λ��׼ֵ	
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
							
							case 0x1D:		//�޸���ʱ��λ�¶�ϵ��
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
							
							case 0x1E:		//�޸���ʱ��λ���Ʊ�־	
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
							
							case 0x1F:		//Stern-Volmerϵ�����¶ȱ仯��ϵ��	
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
							case 0x02:		//��������У׼ʱ����λ��ֵ							
								MODBUS_DATA_FRAME[ADDR_R1] = temp;   //TEST
								gs_u16_CaliPhase					 = temp;   //��������У׼ʱ����λ��ֵ
								Test_Write(FLASH_ADDR_CALIPH0, (u16)gs_u16_CaliPhase);
							  USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
							
			        case 0x03:
								MODBUS_DATA_FRAME[ADDR_R2] = temp;   //TEST
								gs_u16_CaliT0  					   = temp;   //��������У׼ʱ���¶�ֵ
								Test_Write(FLASH_ADDR_CALIT0, (u16)gs_u16_CaliT0);							
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;
             */
								
							case 0x0D:		//�޸Ĵ���ѹ��ֵ
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
								gs_u16_Pressure = temp;                           				//����ȫ�ֱ���
								MODBUS_DATA_FRAME[ADDR_PRE] = temp;
								Test_Write(FLASH_ADDR_PRESSURE, (u16)temp);    						//����FLASH����
								USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;

							case 0x11:		//17: �汾��
									          //���ƽ��յ��İ汾�ţ�ֻ����Ӳ���汾�ţ�ǰ3λ��						
									MODBUS_DATA_VER[0] = comend_Buff[4]*256 + comend_Buff[5];
									temp = (u16)(comend_Buff[6]*256);           
									MODBUS_DATA_VER[1] &= 0x00ff;
									MODBUS_DATA_VER[1] |= temp;
							
									for (i=0;i<DATA_LEN_VER;i++)
									{
											MODBUS_DATA_FRAME[ADDR_VER+i] = MODBUS_DATA_VER[i];  //����MODBUS����
									}
									
									STMFLASH_Write(FLASH_ADDR_SNSR_VER, MODBUS_DATA_VER, (u16)DATA_LEN_VER);														
									USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;

							case 0x14:		//20: PN								
									          //���ƽ��յ����ͺ�
									for (i=0;i<DATA_LEN_PN;i++)
									{
											temp = comend_Buff[4+2*i]*256 + comend_Buff[5+2*i];
											MODBUS_DATA_PN[i] = temp;             //����PN
											MODBUS_DATA_FRAME[ADDR_PN+i] = temp;  //����MODBUS����
									}
									
									STMFLASH_Write(FLASH_ADDR_SNSR_PN, MODBUS_DATA_PN, (u16)DATA_LEN_PN);
									USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;

							case 0x18:		//24: SN								
									//���ƽ��յ������к�
									for (i=0;i<DATA_LEN_SN;i++)
									{
											temp = comend_Buff[4+2*i]*256 + comend_Buff[5+2*i];
											MODBUS_DATA_SN[i] = temp;             //����PN
											MODBUS_DATA_FRAME[ADDR_SN+i] = temp;  //����MODBUS����
									}
									
									STMFLASH_Write(FLASH_ADDR_SNSR_SN, MODBUS_DATA_SN, (u16)DATA_LEN_SN);								
									USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);
							break;																	 

							default: 
							break;	
					}  //switch(comend_Buff[3])  //�����׵�ַ����

					break;
					default: 
						
				break;
		}  //switch(comend_Buff[1])  //�����ֽ���
}

/** ֡���󷵻أ��ڲ����� ********************************************************/
void Comeback_err(u8 comend_ack)
{
	u8 i = 0;
	u16 temp;
	
	uart1_buff.tx_len = 0x05;
	uart1_buff.tx_data_buff[i] = MODBUS_DATA_FRAME[ADDR_485];			 //֡ͷ����������ַ
	i++;
	uart1_buff.tx_data_buff[i] = uart1_buff.rx_data_buff[1]+0x80;  //������
	i++;
	uart1_buff.tx_data_buff[i] = comend_ack;					        		 //�������
	i++;
	temp = CRC16((u8 *)uart1_buff.tx_data_buff, (uart1_buff.tx_len-2));
	uart1_buff.tx_data_buff[i++] = (u8)((temp>>8)&0x00ff);	  		 //CRCУ���λ
	uart1_buff.tx_data_buff[i] = (u8)(temp&0x00ff);		     	  		 //CRCУ���λ

	USART1_SendTxData((u8 *)uart1_buff.tx_data_buff, uart1_buff.tx_len);					
}

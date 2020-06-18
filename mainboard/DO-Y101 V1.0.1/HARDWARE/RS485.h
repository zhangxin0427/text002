#ifndef __RS485_H
#define __RS485_H			  	 
#include "sys.h"
#include "delay.h"
#include "stdlib.h"	
#include "iap.h"

//////////////////////////////////////////////////////////////////////////////////	
/* Private macro -------------------------------------------------------------*/
//������ԭʼ���ݲɼ�
#define PT1000_MAX_VALUE 	1385        		 //PT1000���ֵ����Ӧ100��
#define PT1000_MIN_VALUE 	1000        		 //PT1000��Сֵ����Ӧ0��
                                  
#define DO_MAX_VALUE 		 	2100            //�ܽ������ֵ������100��: 21mg/L
#define DO_MAX_TEMP  		 	510             //�¶����ֵ������10��:51��

#define MAX_PhaseDiff    	13000

#define MAX_TcFactor            	1000     //�����²�ϵ�����ֵ
#define MAX_DOFIXPHASE_Base     	2000     //�ӳ���λ��׼���ֵ
#define MAX_DOFIXPHASE_Factor_T 	200      //�ӳ���λ��׼�²�ϵ�����ֵ
#define MAX_KsvFactor_T         	1000     //Stern-Volmerϵ�����¶ȱ仯��ϵ�����ֵ

//MODBUS485ͨ�����ݸ�ʽ:MODBUS_DATA_FRAME[]
//                ��ַ    ����                ����             ����
#define ADDR_TEMP  0     //�¶�               0.1              ֻ��
#define ADDR_PERC  1     //���Ͷ�DO%          0.1              ֻ��
#define ADDR_R1    2     //Ԥ��1              1.0              
#define ADDR_R2    3     // 
#define ADDR_DO    4     //�ܽ���(mg/L)       0.01             ֻ��
#define ADDR_NCAIL 5     //
#define ADDR_R4    6     //
#define ADDR_ORDER 7     //DO��������         [1:����У׼; 2:������У׼; 10:�ָ�����Ĭ������]  ֻд
#define ADDR_TW    8     //�¶ȹ��ϱ�־λ     [0:��; 1:δ����] ֻ��
#define ADDR_485   9     //RS485��ַ          [0, 127]         [��д]
#define ADDR_SAL   10    //�ζ�ֵ             [0-400],0.1      [��д]
#define ADDR_R5    11    //
#define ADDR_FACT  12    //Stern-Volmerϵ��   0.0001            ֻ��
#define ADDR_PRE   13    //����ѹ��ֵ         1.0              [��д]
#define ADDR_R3    14    //
#define ADDR_R10   15    //ʵʱ�¶�У׼���Ʊ�־     0/1     [��д] 
#define ADDR_R11   16    //ʵʱ�¶�У׼ֵ           0-50     [��д] 
#define ADDR_VER   17    //�������汾��                        [��д]
#define ADDR_PN    20    //�������ͺ�                          [��д]
#define ADDR_SN    24    //������ϵ�к�                        [��д]

#define ADDR_R6    28    //��ʱ��λ��׼ֵ     0-2000  [��д]           
#define ADDR_R7    29    //��ʱ��λ�¶�ϵ��   0-100   [��д]
#define ADDR_R8    30    //��ʱ��λ���Ʊ�־   0/1     [��д]   
#define ADDR_R9    31    //Stern-Volmerϵ�����¶ȱ仯��ϵ���� 0-300     [��д]   

//#define ADDR_TEMP_tmp  14           //�¶�  
//#define ADDR_CaliT_BCFactor  14     //�¶�  

#define DATA_LEN_VER  3    //
#define DATA_LEN_PN   4    //
#define DATA_LEN_SN   4    //

#define ERROR_CRC  1
#define ERROR_LEN  2
#define ERROR_CLI  3


////////////////////////////////////////////////////////////////////////////////// 	
//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
//FLASH��ַ����
#define FLASH_ADDR_SNSR_ADDR   0X0800E000    //��������ַ,[1-127],1���ֽ�
#define FLASH_ADDR_SALINITY    0X0800E002    //�ζ�ֵ,[0-40],1���ֽ�
#define FLASH_ADDR_PRESSURE    0X0800E004    //����ѹ��ֵ,2���ֽ�

#define FLASH_ADDR_TCFACTOR    0X0800E006    //�¶Ȳ���ϵ��,2���ֽ�
#define FLASH_ADDR_KSVFACTOR   0X0800E008    //Stern-Volmerϵ��,2���ֽ�

#define FLASH_ADDR_CALIPH0     0X0800E00A    //�궨DO���ʱ����λ��ֵ
#define FLASH_ADDR_CALIT0      0X0800E00C    //�궨DO���ʱ���¶�ֵ����λ: 1/10�棬2���ֽ�
#define FLASH_ADDR_CALITEMP    0X0800E00E    //�궨������ʱ���¶�ֵ����λ: 1/10�棬2���ֽ�
#define FLASH_ADDR_CALITPHASE  0X0800E04E    //�궨������ʱ����λ��ֵ����λ: 1/10�棬2���ֽ�

#define FLASH_ADDR_SNSR_VER    0X0800E010    //�������汾��,6���ֽ�
#define FLASH_ADDR_SNSR_PN     0X0800E020    //�������ͺ�,8���ֽ�
#define FLASH_ADDR_SNSR_SN     0X0800E030    //���������к�,8���ֽ�

#define FLASH_ADDR_DOFIXPHASE_Base  		0X0800E040    //��ʱ��λ��׼ֵ,2���ֽ�
#define FLASH_ADDR_DOFIXPHASE_Factor_T 	0X0800E042 		//��ʱ��λ�¶�ϵ��,2���ֽ�
#define FLASH_ADDR_DOFIXPHASE_Flag  		0X0800E044    //��ʱ��λ���Ʊ�־,2���ֽ�
#define FLASH_ADDR_KsvFactor_T 					0X0800E046    //Stern-Volmerϵ�����¶ȱ仯��ϵ��,2���ֽ�

#define FLASH_ADDR_CaliT_BCFLAG         0X0800E048    //ʵʱ�¶Ȳ���ֵ,2���ֽ�
#define FLASH_ADDR_CaliT_BC             0X0800E04A    //ʵʱ�¶Ȳ������Ʊ�־,2���ֽ�
//#define FLASH_Running_APP_Flag  0X0800E024


#define DO_CALI_RANGE 0.5            //У׼����Χ[50%~150%],float��

/* Exported functions declared ---------------------------------------------- */
/** ����FLASH���ݣ��ⲿ���� ***************************************************/
void FLASH_Load_Data(void);

/** �¶ȸ��£��ⲿ���� ********************************************************/
void Temp_Value_Update(void);

/** �������£��ⲿ���� ********************************************************/
void Do_Value_Update(void);

/** UART1һ֡���ݽ�����ɣ����������ֽ������ⲿ����(main����) ********************/
void UART1_com(void);

/** ֡���󷵻أ��ڲ����� ********************************************************/
void Comeback_err(u8 comend_ack);

#endif 



#ifndef __RS485_H
#define __RS485_H			  	 
#include "sys.h"
#include "delay.h"
#include "stdlib.h"	
#include "iap.h"

//////////////////////////////////////////////////////////////////////////////////	
/* Private macro -------------------------------------------------------------*/
//传感器原始数据采集
#define PT1000_MAX_VALUE 	1385        		 //PT1000最大值，对应100℃
#define PT1000_MIN_VALUE 	1000        		 //PT1000最小值，对应0℃
                                  
#define DO_MAX_VALUE 		 	2100            //溶解氧最大值（倍率100）: 21mg/L
#define DO_MAX_TEMP  		 	510             //温度最大值（倍率10）:51℃

#define MAX_PhaseDiff    	13000

#define MAX_TcFactor            	1000     //零氧温补系数最大值
#define MAX_DOFIXPHASE_Base     	2000     //延迟相位基准最大值
#define MAX_DOFIXPHASE_Factor_T 	200      //延迟相位基准温补系数最大值
#define MAX_KsvFactor_T         	1000     //Stern-Volmer系数随温度变化的系数最大值

//MODBUS485通信数据格式:MODBUS_DATA_FRAME[]
//                地址    内容                精度             属性
#define ADDR_TEMP  0     //温度               0.1              只读
#define ADDR_PERC  1     //饱和度DO%          0.1              只读
#define ADDR_R1    2     //预留1              1.0              
#define ADDR_R2    3     // 
#define ADDR_DO    4     //溶解氧(mg/L)       0.01             只读
#define ADDR_NCAIL 5     //
#define ADDR_R4    6     //
#define ADDR_ORDER 7     //DO命令输入         [1:零氧校准; 2:饱和氧校准; 10:恢复出厂默认设置]  只写
#define ADDR_TW    8     //温度故障标志位     [0:无; 1:未连接] 只读
#define ADDR_485   9     //RS485地址          [0, 127]         [读写]
#define ADDR_SAL   10    //盐度值             [0-400],0.1      [读写]
#define ADDR_R5    11    //
#define ADDR_FACT  12    //Stern-Volmer系数   0.0001            只读
#define ADDR_PRE   13    //大气压力值         1.0              [读写]
#define ADDR_R3    14    //
#define ADDR_R10   15    //实时温度校准趋势标志     0/1     [读写] 
#define ADDR_R11   16    //实时温度校准值           0-50     [读写] 
#define ADDR_VER   17    //传感器版本号                        [读写]
#define ADDR_PN    20    //传感器型号                          [读写]
#define ADDR_SN    24    //传感器系列号                        [读写]

#define ADDR_R6    28    //延时相位基准值     0-2000  [读写]           
#define ADDR_R7    29    //延时相位温度系数   0-100   [读写]
#define ADDR_R8    30    //延时相位趋势标志   0/1     [读写]   
#define ADDR_R9    31    //Stern-Volmer系数随温度变化的系数。 0-300     [读写]   

//#define ADDR_TEMP_tmp  14           //温度  
//#define ADDR_CaliT_BCFactor  14     //温度  

#define DATA_LEN_VER  3    //
#define DATA_LEN_PN   4    //
#define DATA_LEN_SN   4    //

#define ERROR_CRC  1
#define ERROR_LEN  2
#define ERROR_CLI  3


////////////////////////////////////////////////////////////////////////////////// 	
//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
//FLASH地址定义
#define FLASH_ADDR_SNSR_ADDR   0X0800E000    //传感器地址,[1-127],1个字节
#define FLASH_ADDR_SALINITY    0X0800E002    //盐度值,[0-40],1个字节
#define FLASH_ADDR_PRESSURE    0X0800E004    //大气压力值,2个字节

#define FLASH_ADDR_TCFACTOR    0X0800E006    //温度补偿系数,2个字节
#define FLASH_ADDR_KSVFACTOR   0X0800E008    //Stern-Volmer系数,2个字节

#define FLASH_ADDR_CALIPH0     0X0800E00A    //标定DO零点时的相位差值
#define FLASH_ADDR_CALIT0      0X0800E00C    //标定DO零点时的温度值，单位: 1/10℃，2个字节
#define FLASH_ADDR_CALITEMP    0X0800E00E    //标定饱和氧时的温度值，单位: 1/10℃，2个字节
#define FLASH_ADDR_CALITPHASE  0X0800E04E    //标定饱和氧时的相位差值，单位: 1/10℃，2个字节

#define FLASH_ADDR_SNSR_VER    0X0800E010    //传感器版本号,6个字节
#define FLASH_ADDR_SNSR_PN     0X0800E020    //传感器型号,8个字节
#define FLASH_ADDR_SNSR_SN     0X0800E030    //传感器序列号,8个字节

#define FLASH_ADDR_DOFIXPHASE_Base  		0X0800E040    //延时相位基准值,2个字节
#define FLASH_ADDR_DOFIXPHASE_Factor_T 	0X0800E042 		//延时相位温度系数,2个字节
#define FLASH_ADDR_DOFIXPHASE_Flag  		0X0800E044    //延时相位趋势标志,2个字节
#define FLASH_ADDR_KsvFactor_T 					0X0800E046    //Stern-Volmer系数随温度变化的系数,2个字节

#define FLASH_ADDR_CaliT_BCFLAG         0X0800E048    //实时温度补偿值,2个字节
#define FLASH_ADDR_CaliT_BC             0X0800E04A    //实时温度补偿趋势标志,2个字节
//#define FLASH_Running_APP_Flag  0X0800E024


#define DO_CALI_RANGE 0.5            //校准允许范围[50%~150%],float型

/* Exported functions declared ---------------------------------------------- */
/** 更新FLASH数据，外部函数 ***************************************************/
void FLASH_Load_Data(void);

/** 温度更新，外部函数 ********************************************************/
void Temp_Value_Update(void);

/** 溶氧更新，外部函数 ********************************************************/
void Do_Value_Update(void);

/** UART1一帧数据接收完成，进行命令字解析，外部函数(main调用) ********************/
void UART1_com(void);

/** 帧错误返回，内部函数 ********************************************************/
void Comeback_err(u8 comend_ack);

#endif 



/**
  ******************************************************************************
  * @file    Project/FDO/main.c 
  * @author  XiongZhiming
  * @version V1.0.0
  * @date    12-Dec-2018
  * @brief   Main program body  
  * @attention FDO Signal Processing
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "RS485.h"
#include "adc.h"
#include "string.h"
#include "timer.h"
#include "dspcode.h"
#include "dac1.h"
#include "stm32_dsp.h"

extern void  load_dspcode(u16 start_adr,const char *dspcode);
extern void  DSP_PHREAD(u16 flag_adr,u16 updata_adr,u16 dndata_adr,u16 resdata_adr);
extern void  SPI1_Init(void);
extern void  Codec_Init(void);
extern void  LTC2450_Config(void);
extern float LTC2450_Read(void);
extern void  power_sup(void);
extern u16   MODBUS_DATA_FRAME[];
extern signed short cc[4];
u16 phase_buffer[10];
void sortA1(float a[], int length);
void cleararray(float a[], int length);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 

//变量定义: RS485通信协议
u8 USART1RxFlag = 0;                              //485接收中断标志位
struct uart_data_buff uart1_buff;                 //485数据收发结构体
GPIO_InitTypeDef GPIO_InitStructure;

#define num  10                                   //窗口长度
#define dnum 2                                    //首尾去除长度
#define dist 10                                   //滑动长度


//获取相位差
float 	ccf_buf[num];
float 	ccf_pr[num];
float 	ccf_sum;
float 	ccf;

//获取PT1000电阻值
float 	Temp_buf[num];
float 	Temp_pr[num];
float 	Temp_sum; 
float 	RES;

//u16 ADC_Value[NPT];

/* Private function prototypes -----------------------------------------------*/
	/*
	 * 初始化独立看门狗
	 * prer:分频数:0~7(低3位有效)
	 * 分频因子=4*2^prer.最大值为256!
	 * rlr:重装载寄存器值:低11位有效.
	 * 时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
	 */
void IWDG_Init(u8 prer,u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对寄存器IWDG_PR和IWDG_RLR的写操作
    IWDG_SetPrescaler(prer);    //设置IWDG预分频值
    IWDG_SetReload(rlr);        //设置IWDG重装载值
    IWDG_ReloadCounter();       //按照IWDG重装载寄存器的值重装计数器
    IWDG_Enable();              //使能IWDG
}

  /*
   * 喂独立看门狗
   */
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();    /*reload*/
}
/* Private functions ---------------------------------------------------------*/

int main(void)
 {		
    u16 i=0;	
	  u16 distcount=0;
	  u16 window=0;
	  u16 init_count=0;
		u8 Empty=0;
	  Adc_Init();
//	  Dac1_Test();
    USART1RxFlag = 0;	 
    //SCB->VTOR = SRAM_BASE;
//	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  	   
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	     
	  delay_init();	 //延时函数初始化	
	  power_sup();   //上电初始化I/O,添加LED驱动直流电压分量
	  delay_ms(100);
	  uart1_init(9600);	//串口1初始化为9600	 	

	  LTC2450_Config();  								 //配置LTC2450
		
	  FLASH_Load_Data(); 								 //加载FLASH数据       
		//IWDG_Init(5,625);                //(4*2^6)*625/40=4000ms=4s

		for(i=0;i<4;i++)
	  {
			cc[i]=0;
		}  	
			 
	  cleararray(Temp_buf, num);//清零
		cleararray(ccf_buf, num); //清零
	  ccf=0;
		ccf_sum=0;
		Temp_sum=0;
		i=0;
		window=0; 
		init_count=0;
		distcount=0;
 
		while(1)
		{		
			//Temp_buf[window] = LTC2450_Read();  //显示温度      
	//   		DSP_PHREAD(0x0a4,0x0be,0xbe,0x0a6);
			ccf_buf[window]=((float)(cc[0]*cc[3]-cc[2]*cc[1]))/((float)(cc[0]*cc[2]+cc[1]*cc[3]));
			
			if(init_count < num)
      {
				init_count++;
			}	
			else
			{
				init_count=num;
			}	
			
			if(window < num)
      {
				window++;
			}	
			else
			{
				window = 0;
			}	
			
			if(distcount < dist)
			{
				distcount++;
			}	
			else
			{	
				for(i=0;i<num;i++)
				{
			//		Temp_pr[i] =Temp_buf[i];
					ccf_pr[i] =ccf_buf[i];
				}
			
				Temp_sum=0;
				ccf_sum=0;
		
			//	sortA1(Temp_pr, num); //排序
				sortA1(ccf_pr, num);  //排序
			 
				for(i=dnum;i<num-dnum;i++)   
				{
				//	Temp_sum+=Temp_pr[i];
					ccf_sum +=ccf_pr[i];
				}	//去掉最高最低值
			
		//		RES=Temp_sum/(num-dnum*2);//取平均
				 RES=LTC2450_Read();
										
				ccf=abs(((ccf_sum)/(num-dnum*2))*10000);//取平均
  //			GPIO_SetBits(GPIOA,GPIO_Pin_3);//调试用	
				Temp_Value_Update();              //更新温度值,单位:0.1℃,范围[0-600]
				Do_Value_Update(); 
			
				distcount=0;
				Temp_sum=0;
				ccf_sum=0;
				cleararray(Temp_pr, num);
				cleararray(ccf_pr, num);
			}
		
		
			RS485_Re1();
			
			if ((USART1RxFlag == 1) && (init_count == num))                            //数据接收完毕
	    {
				//USART1_SendTxData("\n\rOpertaing APP!", 18);
				USART1RxFlag = 0;               //清接收标志位
				memset(uart1_buff.rx_data_buff,'\0',sizeof(uart1_buff.rx_data_buff));   //清接收BUFFER 
				USART1_ReadRxData(uart1_buff.rx_data_buff);                             //获取接收数据        
				UART1_com();                                                            //指令解析
	    }	
			
		}
}


/*
int main(void)
{
		float a;
		Adc_Init();
		delay_init();	 //延时函数初始化	
	  power_sup();   //上电初始化I/O,添加LED驱动直流电压分量
		GPIO_SetBits(GPIOA,GPIO_Pin_3);
		while(1)
		{
			Dac1_Test();
			a=Get_Adc_fft(ADC_Channel_10);
		
		}

}
*/

void sortA1(float a[], int length)
{
    int i, j;
	  float temp;

    for(i = 0; i < length; ++i)
	  {
        for(j = i + 1; j < length; ++j)
			  {
            if(a[j] < a[i])
						{
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
}

void cleararray(float a[], int length)
{	 
	 int i;	
	 for(i=0;i<length;i++)
	 {
				a[i]=0;
	 }	
 }

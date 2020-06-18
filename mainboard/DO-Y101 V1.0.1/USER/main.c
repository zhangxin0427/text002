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

//��������: RS485ͨ��Э��
u8 USART1RxFlag = 0;                              //485�����жϱ�־λ
struct uart_data_buff uart1_buff;                 //485�����շ��ṹ��
GPIO_InitTypeDef GPIO_InitStructure;

#define num  10                                   //���ڳ���
#define dnum 2                                    //��βȥ������
#define dist 10                                   //��������


//��ȡ��λ��
float 	ccf_buf[num];
float 	ccf_pr[num];
float 	ccf_sum;
float 	ccf;

//��ȡPT1000����ֵ
float 	Temp_buf[num];
float 	Temp_pr[num];
float 	Temp_sum; 
float 	RES;

//u16 ADC_Value[NPT];

/* Private function prototypes -----------------------------------------------*/
	/*
	 * ��ʼ���������Ź�
	 * prer:��Ƶ��:0~7(��3λ��Ч)
	 * ��Ƶ����=4*2^prer.���ֵΪ256!
	 * rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
	 * ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
	 */
void IWDG_Init(u8 prer,u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
    IWDG_SetPrescaler(prer);    //����IWDGԤ��Ƶֵ
    IWDG_SetReload(rlr);        //����IWDG��װ��ֵ
    IWDG_ReloadCounter();       //����IWDG��װ�ؼĴ�����ֵ��װ������
    IWDG_Enable();              //ʹ��IWDG
}

  /*
   * ι�������Ź�
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
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	     
	  delay_init();	 //��ʱ������ʼ��	
	  power_sup();   //�ϵ��ʼ��I/O,���LED����ֱ����ѹ����
	  delay_ms(100);
	  uart1_init(9600);	//����1��ʼ��Ϊ9600	 	

	  LTC2450_Config();  								 //����LTC2450
		
	  FLASH_Load_Data(); 								 //����FLASH����       
		//IWDG_Init(5,625);                //(4*2^6)*625/40=4000ms=4s

		for(i=0;i<4;i++)
	  {
			cc[i]=0;
		}  	
			 
	  cleararray(Temp_buf, num);//����
		cleararray(ccf_buf, num); //����
	  ccf=0;
		ccf_sum=0;
		Temp_sum=0;
		i=0;
		window=0; 
		init_count=0;
		distcount=0;
 
		while(1)
		{		
			//Temp_buf[window] = LTC2450_Read();  //��ʾ�¶�      
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
		
			//	sortA1(Temp_pr, num); //����
				sortA1(ccf_pr, num);  //����
			 
				for(i=dnum;i<num-dnum;i++)   
				{
				//	Temp_sum+=Temp_pr[i];
					ccf_sum +=ccf_pr[i];
				}	//ȥ��������ֵ
			
		//		RES=Temp_sum/(num-dnum*2);//ȡƽ��
				 RES=LTC2450_Read();
										
				ccf=abs(((ccf_sum)/(num-dnum*2))*10000);//ȡƽ��
  //			GPIO_SetBits(GPIOA,GPIO_Pin_3);//������	
				Temp_Value_Update();              //�����¶�ֵ,��λ:0.1��,��Χ[0-600]
				Do_Value_Update(); 
			
				distcount=0;
				Temp_sum=0;
				ccf_sum=0;
				cleararray(Temp_pr, num);
				cleararray(ccf_pr, num);
			}
		
		
			RS485_Re1();
			
			if ((USART1RxFlag == 1) && (init_count == num))                            //���ݽ������
	    {
				//USART1_SendTxData("\n\rOpertaing APP!", 18);
				USART1RxFlag = 0;               //����ձ�־λ
				memset(uart1_buff.rx_data_buff,'\0',sizeof(uart1_buff.rx_data_buff));   //�����BUFFER 
				USART1_ReadRxData(uart1_buff.rx_data_buff);                             //��ȡ��������        
				UART1_com();                                                            //ָ�����
	    }	
			
		}
}


/*
int main(void)
{
		float a;
		Adc_Init();
		delay_init();	 //��ʱ������ʼ��	
	  power_sup();   //�ϵ��ʼ��I/O,���LED����ֱ����ѹ����
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

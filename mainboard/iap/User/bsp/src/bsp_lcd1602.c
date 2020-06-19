#include "bsp.h"

/********************************************
*��������LCD_GPIO_Init
*����������LCD�˿ڳ�ʼ��
*�����������
*�����������
*����ֵ����
*����˵����
	 LCD_RW--------------PD10		
	 LCD_RS--------------PD9
	 LCD_EN--------------PE15
	 LCD_DB0-------------PD8
	 LCD_DB1-------------PE13
	 LCD_DB2-------------PE14
	 LCD_DB3-------------PE11
	 LCD_DB4-------------PE12
	 LCD_DB5-------------PG0
	 LCD_DB6-------------PG1
	 LCD_DB7-------------PF14
	 LCD_ON--------------PF15
********************************************/
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG|RCC_APB2Periph_GPIOF,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;			
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;		
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
}

/********************************************
*��������LCD_Write_Cmd
*����������д��LCD����
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Write_Cmd(unsigned char Command)
{
	LCD_RW_L;
	LCD_RS_L;
	LCD_EN_H;
	LCD_DB0 = (Command & 0x01)>0?1:0;
	LCD_DB1 = (Command & 0x02)>0?1:0;
	LCD_DB2 = (Command & 0x04)>0?1:0;
	LCD_DB3 = (Command & 0x08)>0?1:0;
	LCD_DB4 = (Command & 0x10)>0?1:0;
	LCD_DB5 = (Command & 0x20)>0?1:0;
	LCD_DB6 = (Command & 0x40)>0?1:0;
	LCD_DB7 = (Command & 0x80)>0?1:0;
	bsp_DelayUS(100);
	LCD_EN_L;
	LCD_RW_H;
}

/********************************************
*��������LCD_Write_Data
*����������д��LCD����
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Write_Data(unsigned char Data)
{
	LCD_RW_L;
	LCD_RS_H;
	LCD_EN_H;
	LCD_DB0 = (Data & 0x01)>0?1:0;
	LCD_DB1 = (Data & 0x02)>0?1:0;
	LCD_DB2 = (Data & 0x04)>0?1:0;
	LCD_DB3 = (Data & 0x08)>0?1:0;
	LCD_DB4 = (Data & 0x10)>0?1:0;
	LCD_DB5 = (Data & 0x20)>0?1:0;
	LCD_DB6 = (Data & 0x40)>0?1:0;
	LCD_DB7 = (Data & 0x80)>0?1:0;
	bsp_DelayUS(100);
	LCD_EN_L;
	LCD_RW_H;
}

/********************************************
*��������LCD_Init
*������������ʼ��LCD
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Init(void)
{
	LCD_GPIO_Init();
	LCD_ON;
	LCD_RS_L;
	LCD_RW_L;
	LCD_EN_L;
	LCD_Write_Cmd(0x38);		//��λ���ߣ�������ʾ�������С5*8
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x0C);		//����ʾ����겻��ʾ����������˸Ч��
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x01);		//����
	bsp_DelayMS(2);
	LCD_Write_Cmd(0x06);		//��ַ�Զ���һ����ʾ���Զ���λ
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x14);		//������ƣ���ַ�Զ���һ
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x02);		//��λLCD1602
	bsp_DelayMS(2);
}

/********************************************
*��������LCD_Clear
*��������������Ļ
*�����������
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Clear(void)
{
	LCD_Write_Cmd(0x38);
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x0C);
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x01);
	bsp_DelayMS(2);
	LCD_Write_Cmd(0x06);
	bsp_DelayUS(50);
	LCD_Write_Cmd(0x14);
	bsp_DelayUS(50);	
	LCD_Write_Cmd(0x02);
	bsp_DelayUS(2);
}

/********************************************
*��������LCD_Write_Str
*����������д���ַ���
*���������*p������ʾ���ַ���ָ��
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Write_Str( char *p)
{
	while(*p && (*p != '\0'))
	{
		LCD_Write_Data(*p);
		p++;
	}
}

/********************************************
*��������LCD_Set_Addr
*���������������ַ�����ʾ��ַ
*���������n:0x00-0x0F��ʾ��һ�У�0x40-0x4F��ʾ�ڶ���
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Set_Addr(unsigned char n)
{
	LCD_Write_Cmd(0x80|n);
	bsp_DelayUS(100);
}

/********************************************
*��������LCD_Disp
*����������LCD��ʾ
*���������(DSPL*)pDS: ��ʾ�����ṹ��	(EPRM*)pEP���洢�����ṹ��	(DS3231_RTC*)DS_RTC��ʱ������ṹ��	(DS3231_RTC*)SDS_RTC:ʱ������ṹ��
*�����������
*����ֵ����
*����˵����
********************************************/
void LCD_Disp(struct DSPL *pDS,struct EPRM *pEP,RTC_Param *DS_RTC,RTC_Param *SDS_RTC)
{
//	LCD_Clear();							//LCD����
	LCD_Write_Cmd(0x0C);						//����ʾ����겻��ʾ����������˸Ч��
	bsp_DelayUS(50);
	LCD_Set_Addr(0x00);						//LCD���õ�һ����ʾ
  char str[15];						//�洢ת���������
	switch(pDS->DsplMenu)
	{
		case 1: 
			if(pDS->DsplSwitchTime <= 40)
			{
				LCD_Write_Str("Light_Controller");
				LCD_Set_Addr(0x40);				//LCD���õڶ�����ʾ
				LCD_Write_Str("ID: ");
				str[0] = pEP->ID % 1000 /100 + '0';		//ת��ΪASCII��
				str[1] = pEP->ID % 100 / 10 + '0';
				str[2] = pEP->ID % 10 + '0';
				str[3] = '\0';
				LCD_Write_Str(str);
				LCD_Write_Str("  V0.3   ");
			}
			else if(pDS->DsplSwitchTime > 40 && pDS->DsplSwitchTime <= 80)
			{	
				LCD_Write_Str("ID: ");
				str[0] = pEP->ID % 1000 /100 + '0';		//ת��ΪASCII��
				str[1] = pEP->ID % 100 / 10 + '0';
				str[2] = pEP->ID % 10 + '0';
				str[3] = '\0';
				LCD_Write_Str(str);
				LCD_Write_Str("  V0.3   ");
				LCD_Set_Addr(0x40);
				LCD_Write_Str("Date:");
				str[0] = DS_RTC->Years / 10 + '0';
				str[1] = DS_RTC->Years % 10 + '0';
				str[2] = '-';
				str[3] = DS_RTC->Months / 10 + '0';
				str[4] = DS_RTC->Months % 10 + '0';
				str[5] = '-';
				str[6] = DS_RTC->Days /10 + '0';
				str[7] = DS_RTC->Days % 10 + '0';
				str[8] = '\0';
				LCD_Write_Str(str);
				LCD_Write_Str("   ");
			}
			else if(pDS->DsplSwitchTime > 80 && pDS->DsplSwitchTime <= 120)
			{
				LCD_Write_Str("Date:");
				str[0] = DS_RTC->Years / 10 + '0';
				str[1] = DS_RTC->Years % 10 + '0';
				str[2] = '-';
				str[3] = DS_RTC->Months / 10 + '0';
				str[4] = DS_RTC->Months % 10 + '0';
				str[5] = '-';
				str[6] = DS_RTC->Days /10 + '0';
				str[7] = DS_RTC->Days % 10 + '0';
				str[8] = '\0';
				LCD_Write_Str(str);
				LCD_Write_Str("   ");
				
				LCD_Set_Addr(0x40);
				LCD_Write_Str("Time:");
				str[0] = DS_RTC->Hours / 10 + '0';
				str[1] = DS_RTC->Hours % 10 + '0';
				str[2] = ':';
				str[3] = DS_RTC->Minutes / 10 + '0';
				str[4] = DS_RTC->Minutes % 10 + '0';
				str[5] = ':';
				str[6] = DS_RTC->Seconds / 10 + '0';
				str[7] = DS_RTC->Seconds % 10 + '0';
				str[8] = '\0';
				LCD_Write_Str(str);
				LCD_Write_Str("   ");
			}
			else if(pDS->DsplSwitchTime > 120 && pDS->DsplSwitchTime <= 160)
			{
				LCD_Write_Str("Time:");
				str[0] = DS_RTC->Hours / 10 + '0';
				str[1] = DS_RTC->Hours % 10 + '0';
				str[2] = ':';
				str[3] = DS_RTC->Minutes / 10 + '0';
				str[4] = DS_RTC->Minutes % 10 + '0';
				str[5] = ':';
				str[6] = DS_RTC->Seconds / 10 + '0';
				str[7] = DS_RTC->Seconds % 10 + '0';
				str[8] = '\0';
				LCD_Write_Str(str);
				LCD_Write_Str("   ");
				LCD_Set_Addr(0x40);
				LCD_Write_Str("Light_Controller");
			}

		break;
		case 2:
			switch(pDS->DsplSelect_2)
			{
				case System_Set    :LCD_Write_Str("System          ");break;
				case SDCard_Check  :LCD_Write_Str("SDCard Play     ");break;
				case UDisk_Check   :LCD_Write_Str("UDisk Play      ");break;
				case Default_Play  :LCD_Write_Str("Default Play    ");break;
				case Other_Function:LCD_Write_Str("Other Function  ");break;
				default:break;
			}
			LCD_Set_Addr(0x40);
			LCD_Write_Str("                ");
		break;
		case 3:
			switch(pDS->DsplSelect_2)
			{
				case System_Set	 :
					switch(pDS->DsplSelect_3)
					{
						case ID_Set       :LCD_Write_Str("ID Set          ");break;
						case Chip_Set     :LCD_Write_Str("Chip Set        ");break;
						case Frame_Speed  :LCD_Write_Str("Frame Speed     ");break;
						case Data_Sequence:LCD_Write_Str("Data Sequence   ");break;
						case Time_Set	  :LCD_Write_Str("Time Set        ");break;
						case WiFi_Set	  :LCD_Write_Str("WiFi Set        ");break;
						case Config_Read  :LCD_Write_Str("Config Read     ");break;
						case Pixel_Set	  :LCD_Write_Str("Pixel Set       ");break;
						case NetSeg_Set	  :LCD_Write_Str("NetSeg Set      ");break;
						case System_Init  :LCD_Write_Str("System Init     ");break;
						default:break;
					}
				break;
				case SDCard_Check:
					switch(pDS->DsplSelect_3)
					{
						case SDCard_Init     :LCD_Write_Str("SDCard Init     ");break;
						case SDCard_Complete :LCD_Write_Str("SDCard Complete ");break;
						case SDCard_Error    :LCD_Write_Str("SDCard Error    ");break;
						case SDCard_File_Null:LCD_Write_Str("SDCard File Null");break;
						default:break;
					}
				break;
				case UDisk_Check:
					switch(pDS->DsplSelect_3)
					{
						case SDCard_Init     :LCD_Write_Str("UDisk Init     ");break;
						case SDCard_Complete :LCD_Write_Str("UDisk Complete ");break;
						case SDCard_Error    :LCD_Write_Str("UDisk Error    ");break;
						case SDCard_File_Null:LCD_Write_Str("UDisk File Null");break;
						default:break;
					}
				break;
				case SDCard_Play :
					switch(pDS->DsplSelect_3)
					{
						  case All_Loop   :LCD_Write_Str("All Loop        ");break;
						  case Single_Loop:LCD_Write_Str("Single Loop     ");break;
						  case Single_Play:LCD_Write_Str("Single Play     ");break;
						  default:break;
					}
				break;
				case UDisk_Play  :
					switch(pDS->DsplSelect_3)
					{
						  case All_Loop   :LCD_Write_Str("All Loop        ");break;
						  case Single_Loop:LCD_Write_Str("Single Loop     ");break;
						  case Single_Play:LCD_Write_Str("Single Play     ");break;
						  default:break;
					}	
				break;
				case Default_Play:
					switch(pDS->Chip)
					{
						  case DMX1200:LCD_Write_Str("DMX512");break;
						  case MBI6020:LCD_Write_Str("MBI6020");break;
						  case MBI6021:LCD_Write_Str("MBI6021");break;
						  case MBI6023:LCD_Write_Str("MBI6023");break;
						  case MBI6024:LCD_Write_Str("MBI6024");break;
						  case MBI6030:LCD_Write_Str("MBI6030");break;
						  case MBI6033:LCD_Write_Str("MBI6033");break;
						  default:break;
					}
					switch(pDS->DsplSelect_3)
					{
						  case _R       :LCD_Write_Str("_R       ");break;
						  case _G       :LCD_Write_Str("_G       ");break;
						  case _B       :LCD_Write_Str("_B       ");break;
						  case _RGB     :LCD_Write_Str("_RGB     ");break;
						  case _Flow    :LCD_Write_Str("_Flow    ");break;
						  case _Black   :LCD_Write_Str("_Black   ");break;
						  case _Gradient:LCD_Write_Str("_Gradient");break;
						  default:break;
					}	
				break;
				case Other_Function:
					switch(pDS->DsplSelect_3)
					{
						case Write_Address:LCD_Write_Str("Write Address    ");break;
						case Test_Address :LCD_Write_Str("Test Address     ");break;
						default:break;
					}
				break;
				default:break;
			}
			LCD_Set_Addr(0x40);
			LCD_Write_Str("                ");
		break;
		case 4:
			switch(pDS->DsplSelect_2)
			{
				case System_Set:
					switch(pDS->DsplSelect_3)
					{
						case ID_Set	  :
							LCD_Write_Str("ID:");
							str[0] = pDS->ID % 1000 /100 + '0';		//����ת��ΪASCII��
							str[1] = pDS->ID % 100 / 10 + '0';
							str[2] = pDS->ID % 10 + '0';
							str[3] = '\0';
							LCD_Write_Str(str);
							LCD_Write_Str("          ");			
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							LCD_Write_Cmd(0x0F);				//����ʾ�������ʾ����˸
							switch(pDS->BitCnt)
							{	
								case 0:LCD_Set_Addr(0x05);break;	//��λ��˸
								case 1:LCD_Set_Addr(0x04);break;	//ʮλ��˸
								case 2:LCD_Set_Addr(0x03);break;	//��λ��˸
								case 3:LCD_Write_Cmd(0x0C);break;	//����ʾ����겻��ʾ��ֹͣ��˸
								default:break;
							}
							
						break;
						case Chip_Set	  :
							switch(pDS->Chip)
							{
								case DMX1200:LCD_Write_Str("DMX512");break;
								case MBI6020:LCD_Write_Str("MBI6020");break;
								case MBI6021:LCD_Write_Str("MBI6021");break;
								case MBI6023:LCD_Write_Str("MBI6023");break;
								case MBI6024:LCD_Write_Str("MBI6024");break;
								case MBI6030:LCD_Write_Str("MBI6030");break;
								case MBI6033:LCD_Write_Str("MBI6033");break;
								default:break;
							}
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
						break;
						case Frame_Speed  :
							LCD_Write_Str("Frame Speed:");
							str[0] = pDS->FrameSpeed / 10 + '0';		//����ת��ΪASCII��
							str[1] = pDS->FrameSpeed % 10 + '0';
							str[2] = '\0';
							LCD_Write_Str(str);
							LCD_Write_Str("  ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							LCD_Write_Cmd(0x0F);				//����ʾ�������ʾ����˸
							switch(pDS->BitCnt)
							{
								case 0:LCD_Set_Addr(0x0D);break;	//��λ��˸
								case 1:LCD_Set_Addr(0x0C);break;	//ʮλ��˸
								case 2:LCD_Write_Cmd(0x0C);break;	//����ʾ����겻��ʾ��ֹͣ��˸
								default:break;
							}
							
						break;
						case Data_Sequence:
							switch(pDS->DataSequence)
							{
								case LCD_RGB:LCD_Write_Str("RGB");break;
								case LCD_RBG:LCD_Write_Str("RBG");break;
								case LCD_GRB:LCD_Write_Str("GRB");break;
								case LCD_GBR:LCD_Write_Str("GBR");break;
								case LCD_BRG:LCD_Write_Str("BRG");break;
								case LCD_BGR:LCD_Write_Str("BGR");break;
								case LCD_RGBN:LCD_Write_Str("RGBN");break;
								case LCD_RBGN:LCD_Write_Str("RBGN");break;
								case LCD_GRBN:LCD_Write_Str("GRBN");break;
								case LCD_GBRN:LCD_Write_Str("GBRN");break;
								case LCD_BRGN:LCD_Write_Str("BRGN");break;
								case LCD_BGRN:LCD_Write_Str("BGRN");break;
							}
							LCD_Write_Str("             ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
						break;
						case Time_Set:
							LCD_Write_Str("Date:");
							str[0] = SDS_RTC->Years / 10 + '0';		//����ת��ΪASCII��
							str[1] = SDS_RTC->Years % 10 + '0';
							str[2] = '-';
							str[3] = SDS_RTC->Months / 10 + '0';
							str[4] = SDS_RTC->Months % 10 + '0';
							str[5] = '-';
							str[6] = SDS_RTC->Days /10 + '0';
							str[7] = SDS_RTC->Days % 10 + '0';
							str[8] = '\0';
							LCD_Write_Str(str);
							LCD_Write_Str("   ");
						
							LCD_Set_Addr(0x40);
							LCD_Write_Str("Time:");
							str[0] = SDS_RTC->Hours / 10 + '0';
							str[1] = SDS_RTC->Hours % 10 + '0';
							str[2] = ':';
							str[3] = SDS_RTC->Minutes / 10 + '0';
							str[4] = SDS_RTC->Minutes % 10 + '0';
							str[5] = ':';
							str[6] = SDS_RTC->Seconds / 10 + '0';
							str[7] = SDS_RTC->Seconds % 10 + '0';
							str[8] = '\0';
							LCD_Write_Str(str);
							LCD_Write_Str("   ");
							LCD_Write_Cmd(0x0F);				//����ʾ�������ʾ����˸
							switch(pDS->BitCnt)
							{	
								case 0:LCD_Set_Addr(0x0C);break;	//�ո�λ��˸	
								case 1:LCD_Set_Addr(0x0B);break;	//��ʮλ��˸
								case 2:LCD_Set_Addr(0x09);break;	//�¸�λ��˸
								case 3:LCD_Set_Addr(0x08);break;	//��ʮλ��˸
								case 4:LCD_Set_Addr(0x06);break;	//���λ��˸
								case 5:LCD_Set_Addr(0x05);break;	//��ʮλ��˸
								case 6:LCD_Set_Addr(0x4C);break;	//���λ��˸
								case 7:LCD_Set_Addr(0x4B);break;	//��ʮλ��˸
								case 8:LCD_Set_Addr(0x49);break;	//�ָ�λ��˸
								case 9:LCD_Set_Addr(0x48);break;	//��ʮλ��˸
								case 10:LCD_Set_Addr(0x46);break;	//ʱ��λ��˸
								case 11:LCD_Set_Addr(0x45);break;	//ʱʮλ��˸
								case 12:LCD_Write_Cmd(0x0C);break;	//����ʾ����겻��ʾ��ֹͣ��˸
								default:break;
							}
						break;
						case WiFi_Set:
							switch(pDS->WiFiMode)
							{
								case WiFi_AP :LCD_Write_Str("WiFi AP");break;
								case WiFi_STA:LCD_Write_Str("WiFi STA");break;
								default:break;
							}
							LCD_Write_Str("         ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
						break;
						case Pixel_Set:
							LCD_Write_Str("Pixel:");
							str[0] = pDS->PixelNum % 1000 /100 + '0';		//����ת��ΪASCII��
							str[1] = pDS->PixelNum % 100 / 10 + '0';
							str[2] = pDS->PixelNum % 10 + '0';
							str[3] = '\0';
							LCD_Write_Str(str);
							LCD_Write_Str("          ");			
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							LCD_Write_Cmd(0x0F);				//����ʾ�������ʾ����˸
							switch(pDS->BitCnt)
							{	
								case 0:LCD_Set_Addr(0x08);break;	//��λ��˸
								case 1:LCD_Set_Addr(0x07);break;	//ʮλ��˸
								case 2:LCD_Set_Addr(0x06);break;	//��λ��˸
								case 3:LCD_Write_Cmd(0x0C);break;	//����ʾ����겻��ʾ��ֹͣ��˸
								default:break;
							}
							
						break;
						case NetSeg_Set:
							LCD_Write_Str("Network Seg:");
							LCD_Write_Str("       ");
							str[0] = pDS->NetSeg[0] % 1000 / 100 + '0';
							str[1] = pDS->NetSeg[0] % 100 / 10 + '0';
							str[2] = pDS->NetSeg[0] % 10 + '0';
							str[3] = '.';
							str[4] = pDS->NetSeg[1] % 1000 / 100 + '0';
							str[5] = pDS->NetSeg[1] % 100 / 10 + '0';
							str[6] = pDS->NetSeg[1] % 10 + '0';
							str[7] = '.';
							str[8] = pDS->NetSeg[2] % 1000 / 100 + '0';
							str[9] = pDS->NetSeg[2] % 100 / 10 + '0';
							str[10] = pDS->NetSeg[2] % 10 + '0';
							str[11] = '\0';
							LCD_Set_Addr(0x40);
							LCD_Write_Str(str);
							LCD_Write_Str("       ");
							LCD_Write_Cmd(0x0F);
							switch(pDS->BitCnt)
							{
								case 0:LCD_Set_Addr(0x4A);break;
								case 1:LCD_Set_Addr(0x49);break;
								case 2:LCD_Set_Addr(0x48);break;
								case 3:LCD_Set_Addr(0x46);break;
								case 4:LCD_Set_Addr(0x45);break;
								case 5:LCD_Set_Addr(0x44);break;
								case 6:LCD_Set_Addr(0x42);break;
								case 7:LCD_Set_Addr(0x41);break;
								case 8:LCD_Set_Addr(0x40);break;
								case 9:LCD_Write_Cmd(0x0C);break;
								default:break;
							}
						default:break;
					}
				break;
				case SDCard_Play:
					switch(pDS->DsplSelect_3)
					{
						case All_Loop	:
							LCD_Write_Str((char*)pDS->FileName[pDS->DsplSelect_4]);
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							break;
						case Single_Loop:
							LCD_Write_Str((char*)pDS->FileName[pDS->DsplSelect_4]);
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							break;
						case Single_Play:
							LCD_Write_Str((char*)pDS->FileName[pDS->DsplSelect_4]);
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							break;
						default:break;
					}
				break;
				case UDisk_Play:
					switch(pDS->DsplSelect_3)
					{
						case All_Loop	:
							LCD_Write_Str((char*)pDS->FileName[pDS->DsplSelect_4]);
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							break;
						case Single_Loop:
							LCD_Write_Str((char*)pDS->FileName[pDS->DsplSelect_4]);
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							break;
						case Single_Play:
							LCD_Write_Str((char*)pDS->FileName[pDS->DsplSelect_4]);
							LCD_Write_Str("                ");
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                ");
							break;
						default:break;
					}
				break;
				case Other_Function:
					switch(pDS->DsplSelect_3)
					{
						case Write_Address:
							switch(pDS->DsplSelect_4)
							{
								case LampInitAddress:LCD_Write_Str("LampInitAddress   ");break;
								case LampNum	    :LCD_Write_Str("Lamp Num          ");break;
								case ChannelNum	    :LCD_Write_Str("Channel Num       ");break;
								case StartAddress   :LCD_Write_Str("Start Address     ");break;
								default:break;
							}
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                  ");
							break;
						case Test_Address:
							switch(pDS->DsplSelect_4)
							{
								case LampInitAddress:LCD_Write_Str("LampInitAddress   ");break;
								case LampNum	    :LCD_Write_Str("Lamp Num          ");break;
								case ChannelNum	    :LCD_Write_Str("Channel Num       ");break;
								case StartAddress   :LCD_Write_Str("Start Test     ");break;
								default:break;
							}
							LCD_Set_Addr(0x40);
							LCD_Write_Str("                  ");
							break;
						default:break;
					}
				break;			
				default:break;
			}
		break;
		case 5:
			switch(pDS->DsplSelect_2)
			{
				case Other_Function:
					switch(pDS->DsplSelect_3)
					{
						case Write_Address:
							switch(pDS->DsplSelect_4)
							{
								case LampInitAddress:
								LCD_Write_Str("LampInitAddress   ");
								LCD_Set_Addr(0x40);
								str[0] = pDS->_LampInitAddress / 100 + '0';
								str[1] = pDS->_LampInitAddress % 100 / 10 + '0';
								str[2] = pDS->_LampInitAddress % 10 + '0';
								str[3] = '\0';
								LCD_Write_Str(str);
								LCD_Write_Str("             ");
								LCD_Write_Cmd(0x0F);
								switch(pDS->BitCnt)
								{
									case 0:LCD_Set_Addr(0x42);break;
									case 1:LCD_Set_Addr(0x41);break;
									case 2:LCD_Set_Addr(0x40);break;
									case 3:LCD_Write_Cmd(0x0C);break;
									default:break;
								}
								break;
							case LampNum	    :
								LCD_Write_Str("Lamp Num          ");
								LCD_Set_Addr(0x40);
								str[0] = pDS->_AddressLampNum / 100 + '0';
								str[1] = pDS->_AddressLampNum % 100 / 10 + '0';
								str[2] = pDS->_AddressLampNum % 10 + '0';
								str[3] = '\0';
								LCD_Write_Str(str);
								LCD_Write_Str("             ");
								LCD_Write_Cmd(0x0F);
								switch(pDS->BitCnt)
								{
									case 0:LCD_Set_Addr(0x42);break;
									case 1:LCD_Set_Addr(0x41);break;
									case 2:LCD_Set_Addr(0x40);break;
									case 3:LCD_Write_Cmd(0x0C);break;
									default:break;
								}
								break;
							case ChannelNum     :
								LCD_Write_Str("Channel Num       ");
								LCD_Set_Addr(0x40);
								str[0] = pDS->_ChannelNum / 100 + '0';
								str[1] = pDS->_ChannelNum  % 100 / 10 + '0';
								str[2] = pDS->_ChannelNum  % 10 + '0';
								str[3] = '\0';
								LCD_Write_Str(str);
								LCD_Write_Str("             ");
								LCD_Write_Cmd(0x0F);
								switch(pDS->BitCnt)
								{
									case 0:LCD_Set_Addr(0x42);break;
									case 1:LCD_Set_Addr(0x41);break;
									case 2:LCD_Set_Addr(0x40);break;
									case 3:LCD_Write_Cmd(0x0C);break;
									default:break;
								}
								break;
							case StartAddress   :
								switch(pDS->DsplSelect_5)
								{
									case Address_do:
										LCD_Write_Str("Addressing..... ");
										LCD_Set_Addr(0x40);
										LCD_Write_Str("                ");
										break;
									case Address_Error:
										LCD_Write_Str("Address Error   ");
										LCD_Set_Addr(0x40);
										LCD_Write_Str("                ");
										break;
									case Address_Success:
										LCD_Write_Str("Address Success ");
										LCD_Set_Addr(0x40);
										LCD_Write_Str("                ");
										break;
									default:break;
								}
								break;
							default:break;
						}
							break;
						case Test_Address:	
							switch(pDS->DsplSelect_4)
							{
								case LampInitAddress:
								LCD_Write_Str("LampInitAddress   ");
								LCD_Set_Addr(0x40);
								str[0] = pDS->_LampInitAddress / 100 + '0';
								str[1] = pDS->_LampInitAddress % 100 / 10 + '0';
								str[2] = pDS->_LampInitAddress % 10 + '0';
								str[3] = '\0';
								LCD_Write_Str(str);
								LCD_Write_Str("             ");
								LCD_Write_Cmd(0x0F);
								switch(pDS->BitCnt)
								{
									case 0:LCD_Set_Addr(0x42);break;
									case 1:LCD_Set_Addr(0x41);break;
									case 2:LCD_Set_Addr(0x40);break;
									case 3:LCD_Write_Cmd(0x0C);break;
									default:break;
								}
								break;
							case LampNum	    :
								LCD_Write_Str("Lamp Num          ");
								LCD_Set_Addr(0x40);
								str[0] = pDS->_AddressLampNum / 100 + '0';
								str[1] = pDS->_AddressLampNum % 100 / 10 + '0';
								str[2] = pDS->_AddressLampNum % 10 + '0';
								str[3] = '\0';
								LCD_Write_Str(str);
								LCD_Write_Str("             ");
								LCD_Write_Cmd(0x0F);
								switch(pDS->BitCnt)
								{
									case 0:LCD_Set_Addr(0x42);break;
									case 1:LCD_Set_Addr(0x41);break;
									case 2:LCD_Set_Addr(0x40);break;
									case 3:LCD_Write_Cmd(0x0C);break;
									default:break;
								}
								break;
							case ChannelNum     :
								LCD_Write_Str("Channel Num       ");
								LCD_Set_Addr(0x40);
								str[0] = pDS->_ChannelNum / 100 + '0';
								str[1] = pDS->_ChannelNum  % 100 / 10 + '0';
								str[2] = pDS->_ChannelNum  % 10 + '0';
								str[3] = '\0';
								LCD_Write_Str(str);
								LCD_Write_Str("             ");
								LCD_Write_Cmd(0x0F);
								switch(pDS->BitCnt)
								{
									case 0:LCD_Set_Addr(0x42);break;
									case 1:LCD_Set_Addr(0x41);break;
									case 2:LCD_Set_Addr(0x40);break;
									case 3:LCD_Write_Cmd(0x0C);break;
									default:break;
								}
								break;
							case StartAddress   :
								switch(pDS->DsplSelect_5)
								{
									case Test_do:
										LCD_Write_Str("Testing..... ");
										LCD_Set_Addr(0x40);
										LCD_Write_Str("                ");
										break;
									case Test_Error:
										LCD_Write_Str("Test Error   ");
										LCD_Set_Addr(0x40);
										LCD_Write_Str("                ");
										break;
									case Test_Success:
										LCD_Write_Str("Test Success ");
										LCD_Set_Addr(0x40);
										LCD_Write_Str("                ");
										break;
									default:break;
								}
								break;
							default:break;
						}
							break;
						default:break;
					}
					break;
				default:break;
			}
		break;							
		default:break;
	}
}


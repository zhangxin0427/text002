#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//��˫��485�ӿ�IAP BootLoader����
//wangwang@ZYWL
//�޸�����:2018.10.12
//�汾��V1.0									  
////////////////////////////////////////////////////////////////////////////////// 	

typedef void (*iapfun)(void);					//����һ���������͵Ĳ���.   
#define FLASH_Bootloader_ADDR		0x08060000  //Ӧ�ó�����ʼ��ַ(�����FLASH)
											                //����0X08060000~0X0800FFFF�Ŀռ�ΪBootloaderʹ�ã�64KB��	   
void iap_load_app(u32 appxaddr);			//��ת��bootloader����ִ��
#endif







































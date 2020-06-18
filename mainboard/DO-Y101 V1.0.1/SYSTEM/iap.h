#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//半双工485接口IAP BootLoader程序
//wangwang@ZYWL
//修改日期:2018.10.12
//版本：V1.0									  
////////////////////////////////////////////////////////////////////////////////// 	

typedef void (*iapfun)(void);					//定义一个函数类型的参数.   
#define FLASH_Bootloader_ADDR		0x08060000  //应用程序起始地址(存放在FLASH)
											                //保留0X08060000~0X0800FFFF的空间为Bootloader使用（64KB）	   
void iap_load_app(u32 appxaddr);			//跳转到bootloader程序执行
#endif







































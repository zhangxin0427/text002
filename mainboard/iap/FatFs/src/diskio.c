/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "bsp_sdio_sd.h"
//#include "malloc.h"		


#define SD_CARD	 0  	//SD卡,卷标为0
#define U_DISK 1	//U盘,卷标为1

/********************************************
*函数名：disk_initialize
*功能描述：初始化磁盘
*输入参数：(BYTE)pdrv：磁盘号 
*输出参数：无
*返回值：0：初始化成功	1：初始化失败
*其他说明：
********************************************/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD:					//SD卡
			res = SD_Init(); 	 
 			if(res==SD_OK)res=0;
			else res=1;		
  			break;
		case U_DISK:					//U盘
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 						//初始化成功
}  

/********************************************
*函数名：disk_status 
*功能描述：获得磁盘状态
*输入参数：(BYTE)pdrv：磁盘号 
*输出参数：无
*返回值：无
*其他说明：
********************************************/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

/********************************************
*函数名：disk_read
*功能描述：读扇区
*输入参数：(BYTE)pdrv：磁盘号	(BYTE*)buff：数据接收缓冲首地址	(DWORD)sector：扇区地址	(UINT)count：需要读取的扇区数 
*输出参数：无
*返回值：0：读取成功	1：读取失败
*其他说明：
********************************************/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;				//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD:					//SD卡
			res=SD_ReadDisk(buff,sector,count);	 
			break;
		case U_DISK:					//U盘
			break;
		default:
			res=1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

/********************************************
*函数名：disk_write 
*功能描述：写扇区
*输入参数：(BYTE)pdrv：磁盘号	(const BYTE*)buff:发送数据首地址		(DWORD)sector:扇区地址	(uint)count：需要写入的扇区数 
*输出参数：无
*返回值：0：写入成功	1：写入失败
*其他说明：
********************************************/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,		/* Data to be written */
	DWORD sector,			/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	u8 res=0;
	u8 retry=0x1F;			//写入失败的时候，重试次数
	if (!count)return RES_PARERR;	//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD:		//SD卡
			while(retry)
			{
				res=SD_WriteDisk((u8*)buff,sector,count);
 				if(res==0)break;
				retry--;
			}
			break;
		case U_DISK:		//U盘
			break;
		default:
			res=1; 
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif

/********************************************
*函数名：disk_ioctl 
*功能描述：其他表参数的获得
*输入参数：(BYTE)pdrv：磁盘号	(BYTE)cmd：控制代码	(void*)buff：发送/接收缓冲区指针 
*输出参数：无
*返回值：无
*其他说明：
********************************************/
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	BYTE cmd,			/* Control code */
	void *buff			/* Buffer to send/receive control data */
)
{
	DRESULT res=RES_OK;						  			     
	if(pdrv==SD_CARD)		//SD卡
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:	    
 		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
 		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
	else if(pdrv==U_DISK)		//U盘  
	{

	}
	else res=RES_ERROR;//其他的不支持
    return res;
}
#endif

//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
////动态分配内存，这里没使用，应为没有使用内存分配，减小RAM的使用
//void *ff_memalloc (UINT size)			
//{
////	return (void*)mymalloc(size);
//}
////释放内存
//void ff_memfree (void* mf)		 
//{
////	myfree(mf);
//}

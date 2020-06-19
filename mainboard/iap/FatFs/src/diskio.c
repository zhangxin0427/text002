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


#define SD_CARD	 0  	//SD��,���Ϊ0
#define U_DISK 1	//U��,���Ϊ1

/********************************************
*��������disk_initialize
*������������ʼ������
*���������(BYTE)pdrv�����̺� 
*�����������
*����ֵ��0����ʼ���ɹ�	1����ʼ��ʧ��
*����˵����
********************************************/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD:					//SD��
			res = SD_Init(); 	 
 			if(res==SD_OK)res=0;
			else res=1;		
  			break;
		case U_DISK:					//U��
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 						//��ʼ���ɹ�
}  

/********************************************
*��������disk_status 
*������������ô���״̬
*���������(BYTE)pdrv�����̺� 
*�����������
*����ֵ����
*����˵����
********************************************/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

/********************************************
*��������disk_read
*����������������
*���������(BYTE)pdrv�����̺�	(BYTE*)buff�����ݽ��ջ����׵�ַ	(DWORD)sector��������ַ	(UINT)count����Ҫ��ȡ�������� 
*�����������
*����ֵ��0����ȡ�ɹ�	1����ȡʧ��
*����˵����
********************************************/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;				//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD:					//SD��
			res=SD_ReadDisk(buff,sector,count);	 
			break;
		case U_DISK:					//U��
			break;
		default:
			res=1; 
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

/********************************************
*��������disk_write 
*����������д����
*���������(BYTE)pdrv�����̺�	(const BYTE*)buff:���������׵�ַ		(DWORD)sector:������ַ	(uint)count����Ҫд��������� 
*�����������
*����ֵ��0��д��ɹ�	1��д��ʧ��
*����˵����
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
	u8 retry=0x1F;			//д��ʧ�ܵ�ʱ�����Դ���
	if (!count)return RES_PARERR;	//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD:		//SD��
			while(retry)
			{
				res=SD_WriteDisk((u8*)buff,sector,count);
 				if(res==0)break;
				retry--;
			}
			break;
		case U_DISK:		//U��
			break;
		default:
			res=1; 
	}
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif

/********************************************
*��������disk_ioctl 
*��������������������Ļ��
*���������(BYTE)pdrv�����̺�	(BYTE)cmd�����ƴ���	(void*)buff������/���ջ�����ָ�� 
*�����������
*����ֵ����
*����˵����
********************************************/
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	BYTE cmd,			/* Control code */
	void *buff			/* Buffer to send/receive control data */
)
{
	DRESULT res=RES_OK;						  			     
	if(pdrv==SD_CARD)		//SD��
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
	else if(pdrv==U_DISK)		//U��  
	{

	}
	else res=RES_ERROR;//�����Ĳ�֧��
    return res;
}
#endif

//���ʱ��
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
////��̬�����ڴ棬����ûʹ�ã�ӦΪû��ʹ���ڴ���䣬��СRAM��ʹ��
//void *ff_memalloc (UINT size)			
//{
////	return (void*)mymalloc(size);
//}
////�ͷ��ڴ�
//void ff_memfree (void* mf)		 
//{
////	myfree(mf);
//}

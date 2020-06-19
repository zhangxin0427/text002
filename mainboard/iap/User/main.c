#include "bsp.h"


void Fs_to_Flash(u32 appxaddr,u8 *appbuf,u32 Sd_Size);

#define		Ctrl_Param_Flash_Address			0x0803C000
#define		Update_Flash_FLAGs_Address 		0x0803E000

const u32		Sd_Buf_Lenth=2048;
extern FATFS  FS;
u8 Contro_ID;
u8 Update_Flags;
u8 Sd_Buf[Sd_Buf_Lenth];
u32 br;
int  main ()
{
	FIL Updatefile;
	u8 Updatefile_path[20];
	u8 res;
	
	
	bsp_Init();
	
	FlashDataRead(Ctrl_Param_Flash_Address+8,(u16 *)&Contro_ID,1);
	
	printf("Contro_ID : %d \r\n",Contro_ID);
	
	if(Contro_ID>0&&Contro_ID<0xff)	
	{
		printf("Contro_ID true. \r\n");
		sprintf((char*)Updatefile_path,"0:/%d/3451.bin",Contro_ID);
		
	}
	else  
		printf("Contro_ID error. \r\n");
	
	while(1)
	{		
			u32	Flash_Adrees=FLASH_APP1_ADDR;
			u32 applenth;
			u16 i=0;
		
		FlashDataRead(Update_Flash_FLAGs_Address,(u16 *)&Update_Flags,1);
		printf("Update_Flags : %d \r\n",Update_Flags);
		
		if(Update_Flags)
		{
			res=f_mount(&FS,"0:",1);				//挂载SD卡	
			
			Update_Flags=0;
			FlashDataWrite(Update_Flash_FLAGs_Address,(u16 *)&Update_Flags,1);
			res = f_open(&Updatefile,(char*)Updatefile_path,FA_READ);	
			
			printf("f_open : %d \r\n",res);
			applenth = f_size(&Updatefile);
			printf("applenth = %d \r\n",applenth);
			while(1)
			{
				memset(Sd_Buf, 0, sizeof(Sd_Buf));
				res = f_read(&Updatefile,Sd_Buf,Sd_Buf_Lenth,(UINT*)&br);	//读取SD卡上的数据到Sd_Buf数组里面，一次长度为2048，单位是B
				

				if (br==0)		//判断读取的SD卡文件是否到了末尾，到了之后跳出while循环
				break;
				
				if(res==0)
				{

						iap_write_appbin(Flash_Adrees,Sd_Buf,2048);	//把SD卡读取到放在Sd_Buf上的数据代码写入到Flash_Adrees这个地址开始的Flash里
						i++;
						Flash_Adrees = FLASH_APP1_ADDR+i*2048;		//Flash地址需要增加2048,因为程序代码需要一直接着往下写

				}
			}

			f_close(&Updatefile);
			f_mount(&FS,"0:",0);				//注销SD卡
			
		}
		
		else 	//不更新FLASH代码
		{
			
			printf("开始执行FLASH用户代码!!\r\n");	
			iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码	
			
		}
		
	}
}




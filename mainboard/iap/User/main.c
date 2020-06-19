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
			res=f_mount(&FS,"0:",1);				//����SD��	
			
			Update_Flags=0;
			FlashDataWrite(Update_Flash_FLAGs_Address,(u16 *)&Update_Flags,1);
			res = f_open(&Updatefile,(char*)Updatefile_path,FA_READ);	
			
			printf("f_open : %d \r\n",res);
			applenth = f_size(&Updatefile);
			printf("applenth = %d \r\n",applenth);
			while(1)
			{
				memset(Sd_Buf, 0, sizeof(Sd_Buf));
				res = f_read(&Updatefile,Sd_Buf,Sd_Buf_Lenth,(UINT*)&br);	//��ȡSD���ϵ����ݵ�Sd_Buf�������棬һ�γ���Ϊ2048����λ��B
				

				if (br==0)		//�ж϶�ȡ��SD���ļ��Ƿ���ĩβ������֮������whileѭ��
				break;
				
				if(res==0)
				{

						iap_write_appbin(Flash_Adrees,Sd_Buf,2048);	//��SD����ȡ������Sd_Buf�ϵ����ݴ���д�뵽Flash_Adrees�����ַ��ʼ��Flash��
						i++;
						Flash_Adrees = FLASH_APP1_ADDR+i*2048;		//Flash��ַ��Ҫ����2048,��Ϊ���������Ҫһֱ��������д

				}
			}

			f_close(&Updatefile);
			f_mount(&FS,"0:",0);				//ע��SD��
			
		}
		
		else 	//������FLASH����
		{
			
			printf("��ʼִ��FLASH�û�����!!\r\n");	
			iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����	
			
		}
		
	}
}




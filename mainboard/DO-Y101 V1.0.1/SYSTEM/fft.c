#include "fft.h"
#include "math.h"


u32 lBufInArray[NPT];
u32 lBufOutArray[NPT];
u32 lBufMagArray[NPT];
float PI2=6.28318530717959;
u32 Fs=192000;
/******************************************************************
��������:InitBufInArray()
��������:ģ��������ݣ����������а���3��Ƶ�����Ҳ�
����˵��:
��    ע:��lBufInArray�����У�ÿ�����ݵĸ�16λ�洢�������ݵ�ʵ����
          ��16λ�洢�������ݵ��鲿(����Ϊ0)
*******************************************************************/
void InitBufInArray(void)
{
    unsigned short i;
    float fx;
    for(i=0; i<NPT; i++)
    {
        fx = 1000 * sin(PI2 * i * 1000.0 / Fs) +
             3000 * sin(PI2 * i * 3000.0 / Fs) +
             4000 * sin(PI2 * i * 5000.0 / Fs);
        lBufInArray[i] = ((signed short)fx) << 16;
    }
} 
/******************************************************************
��������:GetPowerMag()
��������:�������г����ֵ
����˵��:
������ע:�Ƚ�lBufOutArray�ֽ��ʵ��(X)���鲿(Y)��Ȼ������ֵ(sqrt(X*X+Y*Y)
*******************************************************************/
void GetPowerMag(void)
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
    for(i=0; i<NPT/2; i++)
    {
        lX  = (lBufOutArray[i] << 16) >> 16;
        lY  = (lBufOutArray[i] >> 16);
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y) / NPT;
        if(i == 0)
            lBufMagArray[i] = (unsigned long)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned long)(Mag * 65536);
    }
}

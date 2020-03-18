

/*
 * m62429.c
 *
 *	Created on: Aug 8, 2019
 *		Author: wt
 */
#include <zm62429_handle.h>
//m62429 has two input/output channel.
//Data Input Format
//D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10
//D0:	0:1ch,1:2ch
//D1:	0:Both channels at a time,1:A channel at a time.
//D2-D8: volume.
//D9: 1,fixed value.
//D10:1,fixed value.
//Volume Code from Datasheet.
//要达到1dB的精度，需要组合使用D2~D6和D7~D8两个位域
//我们一次调整2个通道故D0D1=10,同时D9D10=11固定值
//0dB: ATT1=0dB,ATT2=0dB,10101,11 ->10,10101,11,11=0x55F
//-1dB: ATT1=0dB,ATT2=-1dB,10101,01 ->10,10101,01,11=0x557
//-2dB: ATT1=0dB,ATT2=-2dB,10101,10 ->10,10101,10,11=0x55B
//-3dB: ATT1=0dB,ATT2=-3dB,10101,00 ->10,10101,00,11=0x553
//-4dB: ATT1=-4dB,ATT2=0dB,00101,11 ->10,00101,11,11=0x45F
//-5dB: ATT1=-4dB,ATT2=-1dB,00101,01 ->10,00101,01,11=0x457
//-6dB: ATT1=-4dB,ATT2=-2dB,00101,10 ->10,00101,10,11=0x45B
//-7dB: ATT1=-4dB,ATT2=-3dB,00101,00 ->10,00101,00,11=0x453
//-8dB: ATT1=-8dB,ATT2=0dB,11001,11 ->10,11001,11,11=0x59F
//......................
//-10dB: ATT1=-8dB,ATT2=-2dB,11001,10 ->10,11001,10,11=0x59B
//......................
//-20dB: ATT1=-20dB,ATT2=0dB,00001,11 ->10,00001,11,11=0x41F
//......................
//-30dB: ATT1=-28dB,ATT2=-2dB,01110,10 ->10,01110,10,11=0x4EB
//......................
//-40dB: ATT1=-40dB,ATT2=0dB,11010,11 ->10,11010,11,11=0x5AF
//......................
//-50dB: ATT1=-48dB,ATT2=-2dB,10010,10 ->10,10010,11,11=0x52F
//......................
//-83dB: ATT1=-80dB,ATT2=-4dB,10000,00 ->10,10000,00,11=0x503

uint8_t 		g_M62429Volume = 100;

void zsy_M62429Init(void)
{
	//maximum audio volume	at startup.
	zsy_M62429Ctrl(&g_M62429Volume);
}


void zsy_Delay()
{
	uint8_t 		i;

	for (i = 0; i < 20; i++)
		{
		;
		}
}


void zsy_M62429Ctrl(uint8_t volume)
{
	uint8_t 		i	= 0;
	uint16_t		uVolumedB = 0;

	if (volume > 80 && volume <= 100)
		{
		uVolumedB			= 0x55F;				/*0dB*/
		g_M62429Volume		= 100;
		}
	else if (volume > 60 && volume <= 80)
		{
		uVolumedB			= 0x59B;				/*-10dB*/
		g_M62429Volume		= 80;
		}
	else if (volume > 40 && volume <= 60)
		{
		uVolumedB			= 0x41F;				/*-20dB*/
		g_M62429Volume		= 60;
		}
	else if (volume > 20 && volume <= 40)
		{
		uVolumedB			= 0x4EB;				/*-30dB*/
		g_M62429Volume		= 40;
		}
	else if (volume > 10 && volume <= 20)
		{
		uVolumedB			= 0x5AF;				/*-40dB*/
		g_M62429Volume		= 20;
		}
	else if (volume > 0 && volume <= 10)
		{
		uVolumedB			= 0x52F;				/*-50dB*/
		g_M62429Volume		= 10;
		}
	else 
		{
		uVolumedB			= 0x503;				/*-83dB*/
		g_M62429Volume		= 0;
		}

	for (i = 0; i < 11; i++)
		{
		DAT_L;
		zsy_Delay();
		CLK_L;
		zsy_Delay();

		if ((uVolumedB & 0x400))
			{
			DAT_H;
			}
		else 
			{
			DAT_L;
			}

		zsy_Delay();
		CLK_H;
		zsy_Delay();
		uVolumedB <<= 1;
		}

	DAT_H;
	zsy_Delay();
	CLK_L;
	zsy_Delay();
	CLK_H;
}


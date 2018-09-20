#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>
#include "AVE_AUDIO_RESAMPLE.h"

//round函数实现
static short resample_Round(long in)
{
	long temp_neg = 0x80000000;
	long temp_pos = 0x7fffffff;
	short temp1;

	if(in>temp_pos)   
	{
		temp1 = 0x7fff;
	}
	else if(in<temp_neg)
	{
		temp1 = 0x8000;
	}
	//   else   temp1=(short)((long)(in>>16))+(((long)(in>>15)) & 0x01);
	else
	{
		temp1 = (short)((long)(in>>16));
	}

	return temp1;
}

//音频上采样函数
static void AVE_Audio_Upsample(short *inputbuf,short *inputbuf_temp,int stereo,int num_data, int up_ratio, int ratio_ord)
{
	int j,k;

	if(stereo)
	{
		for(j=0; j<num_data/2; j++)
		{
			inputbuf_temp[ratio_ord*2+up_ratio*2*j]   = inputbuf[2*j];
			inputbuf_temp[ratio_ord*2+up_ratio*2*j+1] = inputbuf[2*j+1];

			for(k=2; k<up_ratio*2; k++)
			{
				inputbuf_temp[ratio_ord*2+up_ratio*2*j+k] = 0;
			}
		}
	}
	else
	{
		for(j=0; j<num_data; j++)
		{
			inputbuf_temp[ratio_ord+up_ratio*j] = inputbuf[j];

			for(k=1; k<up_ratio; k++)
			{
				inputbuf_temp[ratio_ord+up_ratio*j+k] = 0;
			}
		}
	}
}

//音频上采样后滤波函数
static void AVE_Audio_filter(short *p_coef,short *inputbuf,short *outputbuf,int stereo,int num_data, int ratio_ord)
{
	int i,j;
	long temp_left, temp_right, temp;

	if(stereo)
	{
		for(i=0; i<num_data/2; i++)
		{
			temp_left =0;
			temp_right=0;

			for(j=0; j<(ratio_ord+1); j++)
			{
				temp_left += ((p_coef[j]*inputbuf[ratio_ord*2+2*i-2*j])<<1);
			}

			for(j=0; j<(ratio_ord+1); j++)	
			{
				temp_right += ((p_coef[j]*inputbuf[ratio_ord*2+2*i+1-2*j])<<1);
			}

			outputbuf[2*i]   = resample_Round(temp_left);
			outputbuf[2*i+1] = resample_Round(temp_right);
		}
		for(i=0; i<ratio_ord*2; i++)
		{
			inputbuf[i] = inputbuf[num_data+i];
		}
	}
	else
	{
		for(i=0; i<num_data; i++)
		{
			temp_left = 0;
			for(j=0; j<(ratio_ord+1); j++)
			{
				temp_left += (((p_coef[j])*(inputbuf[ratio_ord+i-j]))<<1);
			}
			outputbuf[i] = resample_Round(temp_left);
		}
		for(i=0; i<ratio_ord; i++)
		{
			inputbuf[i] = inputbuf[num_data+i];
		}
	}
}

//音频下采样函数
static void AVE_Audio_Dnsample(short *inputbuf, short *outbuf, int stereo, int num_data, int dn_ratio)
{
	int i;
	if(stereo)
	{
		for(i=0; i<(num_data/(2*dn_ratio)); i++)
		{
			outbuf[2*i]   = inputbuf[2*dn_ratio*i];
			outbuf[2*i+1] = inputbuf[2*dn_ratio*i+1];
		}
	}
	else
	{
		for(i=0; i<num_data/dn_ratio; i++)
		{
			outbuf[i] = inputbuf[dn_ratio*i];
		}
	}
}

//音频下采样函数
// very sample method but not accuraty enough
void AVE_Audio_Dnsample(short *inputbuf, short *outbuf, int stereo, int num_data, int orig_samplerate, int target_samplerate)
{
	int i;
	int dn_ratio = (orig_samplerate/target_samplerate) == 0?1:(orig_samplerate/target_samplerate); // 2倍数

	if(stereo)
	{
		for(i=0; i<(num_data/(2*dn_ratio)); i++)
		{
			outbuf[2*i]   = inputbuf[2*dn_ratio*i];
			outbuf[2*i+1] = inputbuf[2*dn_ratio*i+1];
		}
	}
	else
	{
		for(i=0; i<num_data/dn_ratio; i++)
		{
			outbuf[i] = inputbuf[dn_ratio*i];
		}
	}
}

void AVE_Audio_Dnsample2(char *inputbuf, char *outbuf, int stereo, int num_data, int orig_samplerate, int target_samplerate)
{
	int i;
	int dn_ratio = (orig_samplerate/target_samplerate) == 0?1:(orig_samplerate/target_samplerate); // 2倍数

	// audio is short type
	if(stereo)
	{
		for(i=0; i<(num_data/(4*dn_ratio)); i++)
		{
			// left channel
			outbuf[4*i]   = inputbuf[4*dn_ratio*i];
			outbuf[4*i+1] = inputbuf[4*dn_ratio*i+1];
		    // right channel
			outbuf[4*i+2] = inputbuf[4*dn_ratio*i+2];
			outbuf[4*i+3] = inputbuf[4*dn_ratio*i+3];
		}
	}
	else
	{
		for(i=0; i<num_data/(2*dn_ratio); i++) // char to short
		{
			outbuf[2*i] = inputbuf[2*dn_ratio*i];
			outbuf[2*i+1] = inputbuf[2*dn_ratio*i+1];
		}
	}
}

//音频采样率转换滤波器系数 added by LILY
short coef_up_2[41] =
{ 
	0x0000,
	0xffc6,
	0x0000,
	0x0088,
	0x0000,
	0xfef4,
	0x0000,
	0x01d3,
	0x0000,
	0xfd04,
	0x0000,
	0x04b2,
	0x0000,
	0xf8ad,
	0x0000,
	0x0bc9,
	0x0000,
	0xea7a,
	0x0000,
	0x4387,
	0x6aaa,
	0x4387,
	0x0000,
	0xea7a,
	0x0000,
	0x0bc9,
	0x0000,
	0xf8ad,
	0x0000,
	0x04b2,
	0x0000,
	0xfd04,
	0x0000,
	0x01d3,
	0x0000,
	0xfef4,
	0x0000,
	0x0088,
	0x0000,
	0xffc6,
	0x0000

	/*
	0x0000,
	0x00b7,
	0x0000,
	0xfd36,
	0x0000,
	0x0782,
	0x0000,
	0xed8f,
	0x0000,
	0x4267,
	0x6aaa,
	0x4267,
	0x0000,
	0xed8f,
	0x0000,
	0x0782,
	0x0000,
	0xfd36,
	0x0000,
	0x00b7,
	0x0000
	*/                 
};  
    
    
short coef_up_3[49]=
{    
/*    0x0000,
    0xffd6,
    0xffc5,
    0x0000,
    0x0068,
    0x0085,
    0x0000,
    0xff2f,
    0xff00,
    0x0000,
    0x0172,
    0x01b8,
    0x0000,
    0xfd9c,
    0xfd35,
    0x0000,
    0x03c6,
    0x0460,
    0x0000,
    0xfa1d,
    0xf929,
    0x0000,
    0x0962,
    0x0b24,
    0x0000,
    0xef6c,
    0xeacf,
    0x0000,
    0x2bab,
    0x57fe,
    0x6aaa,
    0x57fe,
    0x2bab,
    0x0000,
    0xeacf,
    0xef6c,
    0x0000,
    0x0b24,
    0x0962,
    0x0000,
    0xf929,
    0xfa1d,
    0x0000,
    0x0460,
    0x03c6,
    0x0000,
    0xfd35,
    0xfd9c,
    0x0000,
    0x01b8,
    0x0172,
    0x0000,
    0xff00,
    0xff2f,
    0x0000,
    0x0085,
    0x0068,
    0x0000,
    0xffc5,
    0xffd6,
    0x0000
*/
/*	0x0000,
    0x0075,
    0x00d0,
    0x0000,
    0xfe00,
    0xfd1a,
    0x0000,
    0x0595,
    0x078d,
    0x0000,
    0xf252,
    0xed3b,
    0x0000,
    0x2a61,
    0x5757,
    0x6aaa,
    0x5757,
    0x2a61,
    0x0000,
    0xed3b,
    0xf252,
    0x0000,
    0x078d,
    0x0595,
    0x0000,
    0xfd1a,
    0xfe00,
    0x0000,
    0x00d0,
    0x0075,
    0x0000
*/
    0x0000,
    0xffc7,
    0xffaa,
    0x0000,
    0x00a7,
    0x00de,
    0x0000,
    0xfe8f,
    0xfe30,
    0x0000,
    0x02c1,
    0x035b,
    0x0000,
    0xfb19,
    0xfa1b,
    0x0000,
    0x088c,
    0x0a62,
    0x0000,
    0xf000,
    0xeb48,
    0x0000,
    0x2b6d,
    0x57df,
    0x6aaa,
    0x57df,
    0x2b6d,
    0x0000,
    0xeb48,
    0xf000,
    0x0000,
    0x0a62,
    0x088c,
    0x0000,
    0xfa1b,
    0xfb19,
    0x0000,
    0x035b,
    0x02c1,
    0x0000,
    0xfe30,
    0xfe8f,
    0x0000,
    0x00de,
    0x00a7,
    0x0000,
    0xffaa,
    0xffc7,
    0x0000
};
 
 
short coef_up_4[81]=
{
/*    0x0000,
    0xffe1,
    0xffc6,
    0xffcc,
    0x0000,
    0x004f,
    0x0088,
    0x0074,
    0x0000,
    0xff5e,
    0xfef4,
    0xff24,
    0x0000,
    0x0121,
    0x01d3,
    0x0177,
    0x0000,
    0xfe20,
    0xfd04,
    0xfda1,
    0x0000,
    0x02f8,
    0x04b2,
    0x03b5,
    0x0000,
    0xfb5f,
    0xf8ad,
    0xfa32,
    0x0000,
    0x075a,
    0x0bc9,
    0x0984,
    0x0000,
    0xf331,
    0xea7a,
    0xed74,
    0x0000,
    0x1f9c,
    0x4387,
    0x5fe6,
    0x6aaa,
    0x5fe6,
    0x4387,
    0x1f9c,
    0x0000,
    0xed74,
    0xea7a,
    0xf331,
    0x0000,
    0x0984,
    0x0bc9,
    0x075a,
    0x0000,
    0xfa32,
    0xf8ad,
    0xfb5f,
    0x0000,
    0x03b5,
    0x04b2,
    0x02f8,
    0x0000,
    0xfda1,
    0xfd04,
    0xfe20,
    0x0000,
    0x0177,
    0x01d3,
    0x0121,
    0x0000,
    0xff24,
    0xfef4,
    0xff5e,
    0x0000,
    0x0074,
    0x0088,
    0x004f,
    0x0000,
    0xffcc,
    0xffc6,
    0xffe1,
    0x0000
*/
    0x0000,
    0x0051,
    0x00b7,
    0x00c1,
    0x0000,
    0xfe86,
    0xfd36,
    0xfd6b,
    0x0000,
    0x0437,
    0x0782,
    0x06a4,
    0x0000,
    0xf5a5,
    0xed8f,
    0xef54,
    0x0000,
    0x1e6f,
    0x4267,
    0x5f7f,
    0x6aaa,
    0x5f7f,
    0x4267,
    0x1e6f,
    0x0000,
    0xef54,
    0xed8f,
    0xf5a5,
    0x0000,
    0x06a4,
    0x0782,
    0x0437,
    0x0000,
    0xfd6b,
    0xfd36,
    0xfe86,
    0x0000,
    0x00c1,
    0x00b7,
    0x0051,
    0x0000
};
 
short coef_up_6[121]=
{
/*   0x0000,
   0xffec,
   0xffd6,
   0xffc6,
   0xffc5,
   0xffd8,
   0x0000,
   0x0034,
   0x0068,
   0x0088,
   0x0085,
   0x0056,
   0x0000,
   0xff93,
   0xff2f,
   0xfef4,
   0xff00,
   0xff5d,
   0x0000,
   0x00c3,
   0x0172,
   0x01d3,
   0x01b8,
   0x0114,
   0x0000,
   0xfeba,
   0xfd9c,
   0xfd04,
   0xfd35,
   0xfe42,
   0x0000,
   0x0206,
   0x03c6,
   0x04b2,
   0x0460,
   0x02b8,
   0x0000,
   0xfcd8,
   0xfa1d,
   0xf8ad,
   0xf929,
   0xfbbc,
   0x0000,
   0x04fd,
   0x0962,
   0x0bc9,
   0x0b24,
   0x070c,
   0x0000,
   0xf76a,
   0xef6c,
   0xea7a,
   0xeacf,
   0xf1e2,
   0x0000,
   0x140e,
   0x2bab,
   0x4387,
   0x57fe,
   0x65cb,
   0x6aaa,
   0x65cb,
   0x57fe,
   0x4387,
   0x2bab,
   0x140e,
   0x0000,
   0xf1e2,
   0xeacf,
   0xea7a,
   0xef6c,
   0xf76a,
   0x0000,
   0x070c,
   0x0b24,
   0x0bc9,
   0x0962,
   0x04fd,
   0x0000,
   0xfbbc,
   0xf929,
   0xf8ad,
   0xfa1d,
   0xfcd8,
   0x0000,
   0x02b8,
   0x0460,
   0x04b2,
   0x03c6,
   0x0206,
   0x0000,
   0xfe42,
   0xfd35,
   0xfd04,
   0xfd9c,
   0xfeba,
   0x0000,
   0x0114,
   0x01b8,
   0x01d3,
   0x0172,
   0x00c3,
   0x0000,
   0xff5d,
   0xff00,
   0xfef4,
   0xff2f,
   0xff93,
   0x0000,
   0x0056,
   0x0085,
   0x0088,
   0x0068,
   0x0034,
   0x0000,
   0xffd8,
   0xffc5,
   0xffc6,
   0xffd6,
   0xffec,
   0x0000
*/
    0x0000,
    0x002f,
    0x0075,
    0x00b7,
    0x00d0,
    0x009a,
    0x0000,
    0xff0e,
    0xfe00,
    0xfd36,
    0xfd1a,
    0xfe03,
    0x0000,
    0x02c1,
    0x0595,
    0x0782,
    0x078d,
    0x050e,
    0x0000,
    0xf934,
    0xf252,
    0xed8f,
    0xed3b,
    0xf322,
    0x0000,
    0x1323,
    0x2a61,
    0x4267,
    0x5757,
    0x659b,
    0x6aaa,
    0x659b,
    0x5757,
    0x4267,
    0x2a61,
    0x1323,
    0x0000,
    0xf322,
    0xed3b,
    0xed8f,
    0xf252,
    0xf934,
    0x0000,
    0x050e,
    0x078d,
    0x0782,
    0x0595,
    0x02c1,
    0x0000,
    0xfe03,
    0xfd1a,
    0xfd36,
    0xfe00,
    0xff0e,
    0x0000,
    0x009a,
    0x00d0,
    0x00b7,
    0x0075,
    0x002f,
    0x0000
  
}; 
   
/*short coef_8kto12k[61]=   
{
   0x0000,	
   0xffd6,
   0xffc5,
   0x0000,
   0x0068,
   0x0085,
   0x0000,
   0xff2f,
   0xff00,
   0x0000,
   0x0172,
   0x01b8,
   0x0000,
   0xfd9c,
   0xfd35,
   0x0000,
   0x03c6,
   0x0460,
   0x0000,
   0xfa1d,
   0xf929,
   0x0000,
   0x0962,
   0x0b24,
   0x0000,
   0xef6c,
   0xeacf,
   0x0000,
   0x2bab,
   0x57fe,
   0x6aaa,
   0x57fe,
   0x2bab,
   0x0000,
   0xeacf,
   0xef6c,
   0x0000,
   0x0b24,
   0x0962,
   0x0000,
   0xf929,
   0xfa1d,
   0x0000,
   0x0460,
   0x03c6,
   0x0000,
   0xfd35,
   0xfd9c,
   0x0000,
   0x01b8,
   0x0172,
   0x0000,
   0xff00,
   0xff2f,
   0x0000,
   0x0085,
   0x0068,
   0x0000,
   0xffc5,
   0xffd6,
   0x0000
}; 
*/
short coef_8kto11k[221]=
{
/*   0x0000,	
   0xfff5,
   0xffea,
   0xffde,
   0xffd2,
   0xffc9,
   0xffc4,
   0xffc4,
   0xffc9,
   0xffd5,
   0xffe8,
   0x0000,
   0x001b,
   0x0039,
   0x0056,
   0x0070,
   0x0083,
   0x008c,
   0x0089,
   0x007a,
   0x005d,
   0x0033,
   0x0000,
   0xffc5,
   0xff89,
   0xff50,
   0xff20,
   0xfefe,
   0xfef0,
   0xfef8,
   0xff18,
   0xff51,
   0xffa0,
   0x0000,
   0x0069,
   0x00d5,
   0x0139,
   0x018b,
   0x01c3,
   0x01d9,
   0x01c7,
   0x018c,
   0x0128,
   0x00a1,
   0x0000,
   0xff4e,
   0xfe9d,
   0xfdf9,
   0xfd74,
   0xfd1c,
   0xfcfb,
   0xfd1b,
   0xfd7e,
   0xfe21,
   0xfefc,
   0x0000,
   0x011a,
   0x0233,
   0x0335,
   0x0405,
   0x048e,
   0x04be,
   0x0489,
   0x03ec,
   0x02ec,
   0x0195,
   0x0000,
   0xfe48,
   0xfc91,
   0xfb00,
   0xf9bc,
   0xf8e6,
   0xf89a,
   0xf8ea,
   0xf9dd,
   0xfb6c,
   0xfd82,
   0x0000,
   0x02b6,
   0x0570,
   0x07f2,
   0x0a01,
   0x0b66,
   0x0bf2,
   0x0b84,
   0x0a0b,
   0x078e,
   0x0425,
   0x0000,
   0xfb62,
   0xf6a0,
   0xf21b,
   0xee3a,
   0xeb63,
   0xe9f7,
   0xea46,
   0xec8e,
   0xf0f2,
   0xf776,
   0x0000,
   0x0a54,
   0x161a,
   0x22de,
   0x3018,
   0x3d34,
   0x4998,
   0x54ae,
   0x5dee,
   0x64e2,
   0x6933,
   0x6aaa,
   0x6933,
   0x64e2,
   0x5dee,
   0x54ae,
   0x4998,
   0x3d34,
   0x3018,
   0x22de,
   0x161a,
   0x0a54,
   0x0000,
   0xf776,
   0xf0f2,
   0xec8e,
   0xea46,
   0xe9f7,
   0xeb63,
   0xee3a,
   0xf21b,
   0xf6a0,
   0xfb62,
   0x0000,
   0x0425,
   0x078e,
   0x0a0b,
   0x0b84,
   0x0bf2,
   0x0b66,
   0x0a01,
   0x07f2,
   0x0570,
   0x02b6,
   0x0000,
   0xfd82,
   0xfb6c,
   0xf9dd,
   0xf8ea,
   0xf89a,
   0xf8e6,
   0xf9bc,
   0xfb00,
   0xfc91,
   0xfe48,
   0x0000,
   0x0195,
   0x02ec,
   0x03ec,
   0x0489,
   0x04be,
   0x048e,
   0x0405,
   0x0335,
   0x0233,
   0x011a,
   0x0000,
   0xfefc,
   0xfe21,
   0xfd7e,
   0xfd1b,
   0xfcfb,
   0xfd1c,
   0xfd74,
   0xfdf9,
   0xfe9d,
   0xff4e,
   0x0000,
   0x00a1,
   0x0128,
   0x018c,
   0x01c7,
   0x01d9,
   0x01c3,
   0x018b,
   0x0139,
   0x00d5,
   0x0069,
   0x0000,
   0xffa0,
   0xff51,
   0xff18,
   0xfef8,
   0xfef0,
   0xfefe,
   0xff20,
   0xff50,
   0xff89,
   0xffc5,
   0x0000,
   0x0033,
   0x005d,
   0x007a,
   0x0089,
   0x008c,
   0x0083,
   0x0070,
   0x0056,
   0x0039,
   0x001b,
   0x0000,
   0xffe8,
   0xffd5,
   0xffc9,
   0xffc4,
   0xffc4,
   0xffc9,
   0xffd2,
   0xffde,
   0xffea,
   0xfff5,
   0x0000
   */

    0x0000,
    0x0016,
    0x0035,
    0x005a,
    0x0082,
    0x00a7,
    0x00c4,
    0x00d1,
    0x00c8,
    0x00a3,
    0x0060,
    0x0000,
    0xff84,
    0xfef6,
    0xfe60,
    0xfdd3,
    0xfd60,
    0xfd18,
    0xfd0e,
    0xfd4e,
    0xfde2,
    0xfeca,
    0x0000,
    0x0171,
    0x0306,
    0x049b,
    0x0607,
    0x0721,
    0x07be,
    0x07b8,
    0x06f4,
    0x0564,
    0x030b,
    0x0000,
    0xfc6b,
    0xf88d,
    0xf4b3,
    0xf13a,
    0xee83,
    0xecf1,
    0xecde,
    0xee97,
    0xf24f,
    0xf81f,
    0x0000,
    0x09c4,
    0x1520,
    0x21a5,
    0x2ecd,
    0x3bfe,
    0x4894,
    0x53ef,
    0x5d76,
    0x64a9,
    0x6925,
    0x6aaa,
    0x6925,
    0x64a9,
    0x5d76,
    0x53ef,
    0x4894,
    0x3bfe,
    0x2ecd,
    0x21a5,
    0x1520,
    0x09c4,
    0x0000,
    0xf81f,
    0xf24f,
    0xee97,
    0xecde,
    0xecf1,
    0xee83,
    0xf13a,
    0xf4b3,
    0xf88d,
    0xfc6b,
    0x0000,
    0x030b,
    0x0564,
    0x06f4,
    0x07b8,
    0x07be,
    0x0721,
    0x0607,
    0x049b,
    0x0306,
    0x0171,
    0x0000,
    0xfeca,
    0xfde2,
    0xfd4e,
    0xfd0e,
    0xfd18,
    0xfd60,
    0xfdd3,
    0xfe60,
    0xfef6,
    0xff84,
    0x0000,
    0x0060,
    0x00a3,
    0x00c8,
    0x00d1,
    0x00c4,
    0x00a7,
    0x0082,
    0x005a,
    0x0035,
    0x0016,
    0x0000


};
   
   
short *coef_up[]=
{
   0,
   0,
   coef_up_2,
   coef_up_3,
   coef_up_4,
   0,
   coef_up_6
};

int filter_ord[]=
{
   FILTER_ORD_11,
   0,
   FILTER_ORD_2,
   FILTER_ORD_3,
   FILTER_ORD_4,
   0,
   FILTER_ORD_6
};

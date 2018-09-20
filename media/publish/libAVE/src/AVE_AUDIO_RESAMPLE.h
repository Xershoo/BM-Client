#ifndef _AVE_AUDI_RESAMPLE_H
#define _AVE_AUDI_RESAMPLE_H

//#include "ave_global.h"

#ifdef __cplusplus
extern "C" {
#endif

//音频转换时需要滤波的参数  added by LILY 20140515
#define FILTER_ORD_2  40       //20           //40
#define FILTER_ORD_3  48
#define FILTER_ORD_4  40
#define FILTER_ORD_6  60        //120
#define FILTER_ORD_11  110

extern short    coef_up_2[];
extern short    coef_up_6[];
extern short    coef_up_4[];
extern short    coef_up_3[];
extern short	*coef_up[];
extern int		filter_ord[];
extern short    coef_8kto11k[];

//音频采样率转换的函数
//void AVE_Audio_Sample_Conversion(short *inputbuf,short *inputbuf_temp,short *out_temp,short *outbuf,int stereo,int num_data, int up_ratio,int dn_ratio);
//int AVE_Resample_Audio_Frame(void * h, short *resamplebuffer, int dst_sample_rate, AVE_Audio_Frame *res_frame);
//void * AVE_Resample_Audio_Handle_Create(int nInMAudioLen);
//int AVE_Resample_Audio_Handle_Close(void *h);
//int AVE_Resample_Audio_Handle_Delete(void *h);
void AVE_Audio_Dnsample(short *inputbuf, short *outbuf, int stereo, int num_data, int orig_samplerate, int target_samplerate);
void AVE_Audio_Dnsample2(char *inputbuf, char *outbuf, int stereo, int num_data, int orig_samplerate, int target_samplerate);
int AVE_Audio_Sample_Conversion(char *inputbuf, char *outbuf, int stereo, int num_data, int orig_ratio, int target_ratio);
#ifdef __cplusplus
}
#endif

#endif

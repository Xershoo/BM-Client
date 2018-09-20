#ifndef _AVE_GLOBAL_H
#define _AVE_GLOBAL_H

#include "stdint.h"
#if !defined(_MSC_VER) && !defined(ANDROID)
#include <stdbool.h> 
#ifndef NULL
#define NULL 0
#endif
#endif
#define AVE_TEST_MEIPAI 0 // 大片
#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif

//by Lily

#define TEST_TIME 0	

typedef int16_t sample_t;
//typedef float sample_t;
//typedef int sample_t; // by ZL
#define FLOAT_TO_SAMPLE(a) ((sizeof(sample_t) == sizeof(float)) ? (sample_t)(a) : (sample_t)((a)*32768))

#if TEST_TIME

#define TICK_START() {start_clock = clock();}
#define TICK_END(n, name)																				\
	{																									\
		end_clock = clock();																			\
		intern_clock[n] += (end_clock - start_clock);													\
		*moduleName = #name;																			\
		printf("-%-30s, intern_clock[%d] = %ld\n", *moduleName, n, end_clock - start_clock);			\
	}																									\

//__android_log_print(ANDROID_LOG_INFO, "AVE_LOG_zl", "-%-30s, intern_clock[%d] = %ld\n", *moduleName, n, end_clock - start_clock);					\

#else

#define TICK_START()
#define TICK_END(n, name)

#endif																				

//点信息
typedef struct _AVE_POINT
{
	int nX;
	int nY;
}AVE_POINT;

typedef struct ave_video_frame
{
	//int struct_size;			// 本结构体的字节数，= sizeof(xx_image_t)
	uint8_t * data[4];			// 各图像平面
	int stride[4];				// 各图像平面的行间距(相邻两行的字节数)
	int nWidth;					// 图像宽
	int nHeight;					// 图像高
	int nChannel;					// 色彩空间
	int nRotate;
} AVE_Video_Frame;

typedef struct ave_audio_frame
{
	//int struct_size;			// 本结构体的字节数，= sizeof(xx_aframe_t)
	void * buffer;				// PCM数据地址
	int nSize;				// PCM数据有效长度
	int nFormat;					// PCM类型 big/little endian, 16/32bit, signed/unsigned, int/float
	                      // S16=0, FLT32, S32, U8
	int nChannel;
	int nSampleRate;			// 采样率
	int nSamples;               //采样点个数
} AVE_Audio_Frame;

//  文件类型定义 
typedef enum _AVE_FILE_TYPE{ 
	R_FILET_NONE = 0, // video+audio
	R_FILET_MP4 = 1,
	R_FILET_YUV420 = 10, // raw video 
	R_FILET_NV21 = 11,
	R_FILET_NV12 = 12,
	R_FILET_RGBA = 13,
	R_FILET_AYUV = 14, 
	R_FILET_RGB565 = 15,
	R_FILET_RGB888 = 16,
	R_FILET_PNG = 30, // pic 
	R_FILET_PCMS16 = 50, // audio
	R_FILET_PCMFLT32 = 51,
	R_FILET_PCMS32 = 52,
	R_FILET_WAV = 61, 
	R_FILET_AAC = 62
}AVE_FILE_TYPE; 

//  特效主类型 
typedef enum _AVE_FILTER_TYPE{ 
	R_FILTER_NONE         = 0,
	R_FILTER_WATERMARK    = 10, // vfilter 
	R_FILTER_PHOTOFRAME   = 11,
	R_FILTER_MOSAIC       = 12,
	R_FILTER_STAGGERED    = 13,
    //R_FILTER_DYNMP4       = 14, // Dynamic video
	R_FILTER_AR           = 15,
	R_FILTER_FILTERGLASS  = 16,
	R_FILTER_FILTERBEAUTY = 17,
	R_FILTER_BLOCKING     = 18,
	R_FILTER_SKETCH		  = 19,
    R_FILTER_CHANGEVIDEOSPEED = 20,
	R_FILTER_INVERSE      = 21,
	R_FILTER_MIXAUD       = 60, // afilter
	R_FILTER_CHANGEAUDIOSPEED = 61,
	R_FILTER_CHANGEAUDIOTONES = 62,
	R_FILTER_AVTHEME      = 80,
	R_FILTER_CHANGESPEED  = 81
}AVE_FILTER_TYPE;

#ifdef ANDROID
#include "android/log.h"
#define MYLOG __android_log_print(ANDROID_LOG_INFO, "cxx", "%s line=%d\n", __FUNCTION__, __LINE__);
#define MYLOG0(str) __android_log_print(ANDROID_LOG_INFO, "cxx", "%s line=%d %s\n", __FUNCTION__, __LINE__, str);
#elif defined(_MSC_VER)
#define MYLOG printf("%s line=%d\n", __FUNCTION__, __LINE__);
#define MYLOG0(str) printf("%s line=%d %s\n", __FUNCTION__, __LINE__, str);
#endif


#endif //_AVE_GLOBAL_H
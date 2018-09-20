#ifndef _AVE_FILTER_LOADDLL_H
#define _AVE_FILTER_LOADDLL_H

#include <iostream>
#include <windows.h>


//
        /// <remarks> 1: 只处理8位灰度和24位图像。</remarks>
        /// <remarks> 2: Src和Dest可以相同，在相同时速度更快。</remarks>    
typedef void (__stdcall* AdjustBrightnessAndContrast)(byte* Src, byte* Dest, int Width, int Height, int Stride,  int Brightness, int Contrast);
			
typedef struct _effectapi
{
	HINSTANCE ImageProcessingDLL;
	AdjustBrightnessAndContrast _AdjustBrightnessAndContrast;
} VideoEffectAPI;
#ifdef __cplusplus
extern "C" {
#endif
	//进行美镜效果
	//
	VideoEffectAPI* AVE_FILTER_IMGPRODLL_INITI();
	int AVE_FILTER_DLL_RELEASE(VideoEffectAPI * api);
#ifdef __cplusplus
}
#endif

#endif //_AVE_FILTER_FILTERGLASS_H
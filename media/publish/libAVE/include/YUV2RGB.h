#ifndef _YUV2RGB_H
#define _YUV2RGB_H
#include "ave_global.h"
#ifdef __cplusplus
extern "C" {
#endif

int rgb2yuv420(unsigned char* RgbBuf, int nWidth, int nHeight,	unsigned char* yuvBuf, bool bSwitch);
void rgb565_2_rgb24(unsigned char *rgb24, short rgb565);
int RGB2YUV420(unsigned char* RgbBuf, int nRGBChannels, int nRGBWidth, int nRGBHeight , unsigned char* yuvBuf, int nYUVChannel);
int RGBA2YUVA420(unsigned char* RgbBuf, int nWidth, int nHeight , unsigned char* yuvaBuf);
int yuv2rgb(unsigned char* rgb, int width, int height, unsigned char* yuv, bool bSwitch);

#ifdef __cplusplus
}
#endif

#endif //_YUV2RGB_H
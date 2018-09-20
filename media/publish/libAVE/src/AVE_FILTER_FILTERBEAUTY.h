#ifndef _AVE_FILTER_FILTERBEAUTY_H
#define _AVE_FILTER_FILTERBEAUTY_H

#include "AVE_FILTER_LOADDLL.h""
#include "ave_global.h"

#ifdef __cplusplus
extern "C" {
#endif
	//进行美效
	//
	void SmoothingFilteringYUV420(unsigned char *pInImage , const int nInWidth , const int nInHeight , const int nBoxRadius , unsigned char *pOutImage);
	//void AVE_superFastBlur_gray(unsigned char *pInImageBuff, int nWidth, int nHeight, int radius);
	void AVE_superFastBlur_gray2(unsigned char *pInImageBuff, int nWidth, int nHeight, int radius, unsigned char *pOutImageBuff);
	void AVE_FILTER_BEAUTY_YUV(AVE_Video_Frame* pInImgBuff, AVE_Video_Frame *pOutImgBuff, int nInSubID, VideoEffectAPI* apiImgProcessing);
	void AVE_FILTER_BEAUTY_RGB(unsigned char* pInImgBuff, unsigned char *pOutImgBuff, int nWidth, int nHeight, int nStrites, int nInSubID, VideoEffectAPI* apiImgProcessing, int nParams[2]);
#ifdef __cplusplus
}
#endif

#endif //_AVE_FILTER_FILTERGLASS_H
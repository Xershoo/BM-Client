#include "AVE_GLOBAL.h"
#include "AVE_VIDEO_FILTER.h"
#include "AVE_FILTER_FILTERBEAUTY.h"

//#include "YUV2RGB.h"
#include <math.h>

#include <stdlib.h>
#include <memory.h>

#include "AVE_FILTER_LOADDLL.h"

VideoEffectAPI *ImgProAPI;
/************************************************************************/
/*                                                                      */
/************************************************************************/
#define MAXFACENUM	10

int AVE_SpecialEffect_Filter_Video_Init()
{
	ImgProAPI = AVE_FILTER_IMGPRODLL_INITI();

	if(!ImgProAPI)
		return -1;

	return 0;
}

int AVE_SpecialEffect_Filter_Video_Frame(AVE_Video_Frame *iFrame, int nMainID, int nSubID, char *pVMarkPath , AVE_Video_Frame *mFrame , int nInResFlag , AVE_Video_Frame* oFrame)
{
	unsigned char *dst[4] = {NULL};
	unsigned char *src[4] = {NULL};
	int dst_stride[4];
	int src_stride[4];
	int nStartFlag = 0;

	// 做初始化,
	{
 		oFrame->nWidth = iFrame->nWidth;
		oFrame->nHeight = iFrame->nHeight;
		oFrame->nChannel = iFrame->nChannel;
		memcpy(oFrame->data[0] , iFrame->data[0] , iFrame->nWidth * iFrame->nHeight * 3 / 2);
	}

    // 
	if(R_FILTER_FILTERBEAUTY == nMainID)
	{
		//如果输入的SubID为空
		if (R_FILTER_NONE == nSubID)
		{
			oFrame->nWidth = iFrame->nWidth;
			oFrame->nHeight = iFrame->nHeight;
			oFrame->nChannel = iFrame->nChannel;
			memcpy(oFrame->data[0] , iFrame->data[0] , iFrame->nWidth * iFrame->nHeight * 3 / 2);
		}
		else 
		{
			AVE_FILTER_BEAUTY_YUV(iFrame, oFrame, nSubID, ImgProAPI);
		}
	} 
	//

	return 0;
}

int AVE_SpecialEffect_Filter_Video_FrameRGB(unsigned char *iFrame, int nWidth, int nHeight, int nMainID, int nSubID, char *pVMarkPath , unsigned char *mFrame , int nInResFlag , unsigned char* oFrame, int nParams[2])
{
    int nImgSize = nWidth*nHeight*3;

	// 做初始化,
	{
		memcpy(oFrame, iFrame, nImgSize*sizeof(unsigned char));
	}

    // 当输入的特效主ID是PictureFrame时
	if (R_FILTER_FILTERBEAUTY == nMainID)
	{
		//如果输入的SubID为空
		if (R_FILTER_NONE == nSubID || ImgProAPI == NULL)
		{
			return 0;
		}
		else 
		{
			AVE_FILTER_BEAUTY_RGB(iFrame, oFrame, nWidth, nHeight, nWidth*3, nSubID, ImgProAPI, nParams);
			//
		}
	}

	return 0;
}

int AVE_SpecialEffect_Filter_Video_Release()
{
	int status = AVE_FILTER_DLL_RELEASE(ImgProAPI);

	return status;
}
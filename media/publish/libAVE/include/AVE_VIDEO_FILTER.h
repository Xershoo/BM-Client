#ifndef _AVE_VIDEO_FILTER_H
#define _AVE_VIDEO_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

	//处理每一帧图像
	int AVE_SpecialEffect_Filter_Video_Frame(AVE_Video_Frame *iFrame, int nMainID, int nSubID , char *pVMarkPath , AVE_Video_Frame *mFrame , int nInResFlag , AVE_Video_Frame* oFrame);
	int AVE_SpecialEffect_Filter_Video_FrameRGB(unsigned char *iFrame, int nWidth, int nHeight, int nMainID, int nSubID, char *pVMarkPath , unsigned char *mFrame , int nInResFlag , unsigned char* oFrame, int nParams[2]);
	int AVE_SpecialEffect_Filter_Video_Init();
	int AVE_SpecialEffect_Filter_Video_Release();

#ifdef __cplusplus
}
#endif
#endif //_AVE_VIDEO_FILTER_H
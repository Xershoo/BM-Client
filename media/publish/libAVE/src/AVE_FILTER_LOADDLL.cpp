#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "AVE_FILTER_LOADDLL.h"

//#include <opencv/cv.h>
//#include <opencv/cxcore.h>
//#include <opencv/highgui.h>

// just for Imgpro dll
VideoEffectAPI* AVE_FILTER_IMGPRODLL_INITI()
{
	 // ʹ��ImageProccesing�ӿ�   
	 VideoEffectAPI*  ImageProcessingApi;  
	 ImageProcessingApi = (VideoEffectAPI*)malloc(sizeof(VideoEffectAPI));

	 if(!ImageProcessingApi)
		 return NULL;

	 // ���нӿ�����dll�е�   
	 ImageProcessingApi->ImageProcessingDLL = ::LoadLibrary(TEXT("AVE_PROCESSING.dll"));

	 if(ImageProcessingApi->ImageProcessingDLL == NULL)
		 return NULL;

	 // �õ��ӿ�ָ��
	 ImageProcessingApi->_AdjustBrightnessAndContrast = (AdjustBrightnessAndContrast)::GetProcAddress(ImageProcessingApi->ImageProcessingDLL, "AdjustBrightnessAndContrast");
	 {  
	 	// ȡ�ýӿ�ָ��ʧ��   
		 ::FreeLibrary(ImageProcessingApi->ImageProcessingDLL);  
		return NULL;
	 }

	 return ImageProcessingApi;
}

//
int AVE_FILTER_DLL_RELEASE(VideoEffectAPI *api)
{
	if(api)
	{
		if(api->ImageProcessingDLL)
			::FreeLibrary(api->ImageProcessingDLL);
		delete(api);

		api = NULL;
	}

	return 0;
}
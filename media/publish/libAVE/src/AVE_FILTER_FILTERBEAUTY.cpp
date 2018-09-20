#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <time.h>
#include "AVE_FILTER_FILTERBEAUTY.h"
#include "YUV2RGB.h"

//#include <opencv/cv.h>
//#include <opencv/cxcore.h>
//#include <opencv/highgui.h>

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b)) 
#endif

#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

#define  RADIUS1 5   // 高斯模糊半径
#define  RADIUS2 10   // 高斯模糊半径
#define  RADIUS3 20   // 高斯模糊半径

void SmoothingFilteringYUV420(unsigned char *pInImage , const int nInWidth , const int nInHeight , const int nBoxRadius, unsigned char *pOutImage)
{
	int nHIndex = 0 , nWIndex = 0;
	int tmp = 0;
	int nImageSize = 0;
	unsigned char *pTempSrcImageY = NULL;
	unsigned char *pTempSrcImageU = NULL;
	unsigned char *pTempSrcImageV = NULL;
	unsigned char *pTempOutImageY = NULL;
	unsigned char *pTempOutImageU = NULL;
	unsigned char *pTempOutImageV = NULL;
	unsigned char nTempGrayVal = 0;

	pTempSrcImageY = pInImage;
	pTempSrcImageU = pTempSrcImageY + nInHeight * nInWidth;
	pTempSrcImageV = pTempSrcImageU + (nInHeight * nInWidth >> 2);

	pTempOutImageY = pOutImage;
	pTempOutImageU = pTempOutImageY + nInHeight * nInWidth;
	pTempOutImageV = pTempOutImageU + (nInHeight * nInWidth >> 2);

	nImageSize = nInWidth * nInHeight;

	//YUV数据的积分图数据
	//pInteralImage = (unsigned int *)malloc(nImageSize * 3 / 2 * sizeof(unsigned int));
	//memset(pInteralImage , 0 , nImageSize * 3 / 2 * sizeof(unsigned int));

	AVE_superFastBlur_gray2(pTempSrcImageY, nInWidth, nInHeight, 8 , pTempOutImageY);

	//CalIntegralY(pTempSrcImageY, width, height, pInteralImage);
	//计算Y通道的boxfilter
	//BoxFilterY(pInteralImage, 3, width>>1, height>>1, pTempGrayImage, pTempSrcImageY);

	
	memcpy(pTempSrcImageY , pTempOutImageY , nImageSize);
	memcpy(pTempSrcImageU , pTempOutImageU , nImageSize >> 2);
	memcpy(pTempSrcImageV , pTempOutImageV , nImageSize >> 2);

	AVE_superFastBlur_gray2(pTempSrcImageY, nInWidth, nInHeight, nBoxRadius, pTempOutImageY);
	AVE_superFastBlur_gray2(pTempSrcImageU, nInWidth>>1, nInHeight>>1, nBoxRadius>>1, pTempOutImageU);
	AVE_superFastBlur_gray2(pTempSrcImageV, nInWidth>>1, nInHeight>>1, nBoxRadius>>1, pTempOutImageV);
}

//
// brightness and contrast enhancments
// by Ray Zhang
// 201507
int CheckValue(int value)  
{  
    return (value & ~0xff) == 0? value : value > 255? 255 : 0;
}

void GetValue(unsigned char nValue[256], int diffBright, int diffContrast, int nThread)
{
	float cv = diffContrast <= -100? -1.0f : diffContrast / 100.0f;  
    if (diffContrast > 0 && diffContrast < 100)  
        cv = 1.0f / (1.0f - cv) - 1.0f;

	for(int j = 0; j < 256; j ++)  
    {  
        int v = diffContrast > 0? CheckValue(j + diffBright) : j;  
        if (diffContrast >= 255)  
            v = v >= nThread? 255 : 0;  
        else  
            v = CheckValue(v + (int)((v - nThread) * cv + 0.5f));  
        nValue[j] = diffContrast <= 0? CheckValue(v + diffBright) : v;
	}
}
	
void BrightnessAndContrast(unsigned char *pInImgBuff, unsigned char * pOutImgBuff, int nWidth, int nHeight, int diffBright, int diffContrast, int nThread)
{
	int nSize = nWidth*nHeight*3;
	int j;
	static int nFrame = 0;
	static int preBright = 0, preContrast = 0;
	static unsigned char nValues[256];

	memcpy(pOutImgBuff, pInImgBuff, nSize*sizeof(unsigned char));

	if(diffBright == 0 && diffContrast == 0)  
        return;  
    
	if(diffBright != preBright || diffContrast != preContrast)
		nFrame = 0;
	if(nFrame == 0)
	{
		preBright = diffBright;
		preContrast = diffContrast;
		GetValue(nValues, diffBright, diffContrast, nThread);

		nFrame = 1;
	}

	for(j = 0; j < nSize; j++)  
    {  
       pOutImgBuff[j] = nValues[pOutImgBuff[j]];
	}
}

void AVE_superFastBlur_gray2(unsigned char *pInImageBuff, int nWidth, int nHeight, int radius, unsigned char *pOutImageBuff)
{
	unsigned char *pTempImage = NULL;

	if (radius<1)
	{
		return;
	}

	pTempImage = pOutImageBuff;

	int wm  = nWidth-1;
	int hm  = nHeight-1;
	int wh  = nWidth*nHeight;
	int div = (radius<<1)+1;
	int divPixel =  (div<<8);

	unsigned char *Y=new unsigned char[wh];

	int sum = 0;
	int nWIndex = 0 , nHIndex = 0 , nIndex = 0;
	int p,p1,p2,yp,yi,yw;

	int *vMIN = new int[MAX(nWidth,nHeight)];
	int *vMAX = new int[MAX(nWidth,nHeight)];

	unsigned char *dv = new unsigned char[divPixel];

	memcpy(pTempImage, pInImageBuff, wh*sizeof(unsigned char));

	for (nIndex = 0; nIndex < divPixel; nIndex++)
	{
		dv[nIndex] = (nIndex/div);
	}

	yw=yi=0;

	for (nHIndex = 0; nHIndex < nHeight; nHIndex++)
	{
		sum = 0;
		for(nIndex = -radius; nIndex <= radius; nIndex++)
		{
			p = (yi + MIN(wm, MAX(nIndex,0)));
			sum += pTempImage[p];
		}
		for (nWIndex = 0; nWIndex < nWidth; nWIndex++)
		{
			Y[yi] = dv[sum];

			if(nHIndex == 0)
			{
				vMIN[nWIndex] = MIN(nWIndex+radius+1,wm);
				vMAX[nWIndex] = MAX(nWIndex-radius,0);
			}
			p1 = (yw+vMIN[nWIndex]);
			p2 = (yw+vMAX[nWIndex]);

			sum += pTempImage[p1] - pTempImage[p2];;

			yi++;
		}
		yw += nWidth;
	}

	for (nWIndex = 0; nWIndex < nWidth; nWIndex++)
	{
		sum = 0;
		yp  = -radius*nWidth;
		for(nIndex = -radius; nIndex <= radius; nIndex++)
		{
			yi  = MAX(0,yp)+nWIndex;
			sum += Y[yi];
			yp  += nWidth;
		}

		yi = nWIndex;

		for (nHIndex = 0; nHIndex < nHeight; nHIndex++)
		{
			pTempImage[yi] = dv[sum];

			if(nWIndex == 0)
			{
			
				vMAX[nHIndex] = MAX(nHIndex-radius,0)*nWidth;
			}

			p1 = nWIndex+vMIN[nHIndex];
			p2 = nWIndex+vMAX[nHIndex];

			sum += Y[p1]-Y[p2];
			yi  += nWidth;
		}
	}

	delete Y;
	delete vMIN;
	delete vMAX;
	delete dv;
}

int BeautyGlass(unsigned char *pInImage , unsigned char *pGrayImage , unsigned char *pOutImage, int width, int height, int radius, int nBrigntnessLevel)
{
	int nHIndex = 0 , nWIndex = 0;
	int tmp = 0;
	int nImageSize = 0;
	unsigned char *pTempSrcImageY = NULL;
	unsigned char *pTempSrcImageU = NULL;
	unsigned char *pTempSrcImageV = NULL;
	unsigned char *pTempOutImageY = NULL;
	unsigned char *pTempOutImageU = NULL;
	unsigned char *pTempOutImageV = NULL;
	unsigned char *pTempGrayImage = NULL;
	unsigned int  *pInteralImage = NULL;
	unsigned char nTempGrayVal = 0;

	unsigned char *pMark;

	int nBright = 0;
	switch(nBrigntnessLevel)
	{
	case 0:
		nBright = -50;
		break;
	case 1:
		nBright = -30;
		break;
	default:
		nBright = -10;
		break;
	}
	pTempSrcImageY = pInImage;
	pTempSrcImageU = pTempSrcImageY + height * width;
	pTempSrcImageV = pTempSrcImageU + (height * width >> 2);

	pTempOutImageY = pOutImage;
	pTempOutImageU = pTempOutImageY + height * width;
	pTempOutImageV = pTempOutImageU + (height * width >> 2);

	pTempGrayImage = pGrayImage;
	nImageSize = width * height;

	//YUV数据的积分图数据
	//pInteralImage = (unsigned int *)malloc(nImageSize * 3 / 2 * sizeof(unsigned int));
	//memset(pInteralImage , 0 , nImageSize * 3 / 2 * sizeof(unsigned int));

	AVE_superFastBlur_gray2(pTempGrayImage, width, height, radius, pTempGrayImage);

	//CalIntegralY(pTempSrcImageY, width, height, pInteralImage);
	//计算Y通道的boxfilter
	//BoxFilterY(pInteralImage, 3, width>>1, height>>1, pTempGrayImage, pTempSrcImageY);

	for (nHIndex = 0; nHIndex < height; nHIndex++)
	{
		for (nWIndex = 0; nWIndex < width; nWIndex++)
		{
			nTempGrayVal = *(pTempGrayImage ++);
			tmp = *(pTempSrcImageY ++);

			tmp = (tmp+nBright)>0?(tmp+nBright):0;

			*(pTempOutImageY ++) = tmp;
			//*(pTempOutImageY ++) = (nTempGrayVal + nTempGrayVal - (tmp * nTempGrayVal)/255)>255? 255:(nTempGrayVal + nTempGrayVal - (tmp * nTempGrayVal)/255);
		}

	}
	for (nHIndex = 0; nHIndex < (height>>1); nHIndex++)
	{
		for (nWIndex = 0; nWIndex < (width>>1) ; nWIndex++)
		{
			*(pTempOutImageU ++) = *(pTempSrcImageU ++);
			*(pTempOutImageV ++) = *(pTempSrcImageV ++);
			//*(pTempOutImageU ++) =128;
			//*(pTempSrcImageV ++) = 128;
		}
	}
	return 1;
}

void AVE_FILTER_BEAUTY_RGB(unsigned char* pInImgBuff, unsigned char *pOutImgBuff, int nWidth, int nHeight, int nStrites, int nInSubID, VideoEffectAPI* apiImgProcessing, int nParams[2])
{
	int nVal, nImgSize = 0;
	
	nImgSize = nWidth * nHeight * 3;

	//RGB通道
	memcpy(pOutImgBuff, pInImgBuff, nImgSize*sizeof(unsigned char));

	//
	switch (nInSubID)
	{
	case 1: // Lighting soft
		BrightnessAndContrast(pInImgBuff, pOutImgBuff, nWidth, nHeight, -5, 5, 129);
		break;
	case 2: // Lighting medium
		BrightnessAndContrast(pInImgBuff, pOutImgBuff, nWidth, nHeight, 0, 10, 129);
		break;
	case 3: // Lighting heave
		//
		BrightnessAndContrast(pInImgBuff, pOutImgBuff, nWidth, nHeight, 10, 15, 129);
		break;
	default: // Smoothing higher
		//
		BrightnessAndContrast(pInImgBuff, pOutImgBuff, nWidth, nHeight, nParams[0], nParams[1], 129);
		//if(nParams != NULL)
		//	apiImgProcessing->_AdjustBrightnessAndContrast(pOutImgBuff, pOutImgBuff, nWidth, nHeight, nWidth*3, nParams[0], nParams[1]);
		//else
        //    apiImgProcessing->_AdjustBrightnessAndContrast(pOutImgBuff, pOutImgBuff, nWidth, nHeight, nWidth*3, AVE_CONTRAST_BRIGHTNESS_VALUE1, AVE_CONTRAST_CONTRAST_VALUE1);

		break;
	}
}

void AVE_FILTER_BEAUTY_YUV(AVE_Video_Frame* pInImgBuff, AVE_Video_Frame *pOutImgBuff, int nInSubID, VideoEffectAPI* apiImgProcessing)
{
	int nWidth = 0 , nHeight = 0;
	int nWidthU = 0;
	int nHeightU = 0;
	int nImgSize = 0;
	int nUSize= 0;
	int nDirect = 0;
	int nLayer = 0;
	int nTmp;
	int nShift = 0;
	int nLayerLen = 0;
	int i = 0 , j = 0 , m = 0 , n = 0;
	int nVal, nOrigVal;

	int k;

	//输入的YUV通道的图像
	unsigned char *pTempInImgYBuff = NULL;
	unsigned char *pTempInImgUBuff = NULL;
	unsigned char *pTempInImgVBuff = NULL;

	//中间输出的y通道图像
	unsigned char *pTempImgYBuff = NULL;

	//输出的YUV通道的图像
	unsigned char *pTempOutImgYBuff = NULL;

	unsigned char *pTempOutImgUBuff = NULL;
	unsigned char *pTempOutImgVBuff = NULL;

	//static FILE *file = NULL;
	//static int uvalue = 100;
	//int vvalue = 100;
	//if(file == NULL)
	//{
	//	file = fopen("D:\\data\\color_640x480_100_200","wb");
	//}
	nWidth = pInImgBuff->nWidth;
	nHeight = pInImgBuff->nHeight;
	nImgSize = nWidth * nHeight;
	nWidthU = nWidth>>1;
	nHeightU = nHeight>>1;
	nUSize = nImgSize >> 2;
	
	pTempInImgYBuff = pInImgBuff->data[0];
	pTempOutImgYBuff = pOutImgBuff->data[0];

	pTempImgYBuff = (unsigned char *)malloc(nImgSize*sizeof(unsigned char));

	//Y通道
	memcpy(pTempOutImgYBuff, pTempInImgYBuff, nImgSize*sizeof(unsigned char));
	memcpy(pTempImgYBuff, pTempInImgYBuff, nImgSize*sizeof(unsigned char));

	//UV通道
	pTempInImgUBuff = pInImgBuff->data[0] + nImgSize;
	pTempInImgVBuff = pTempInImgUBuff + nUSize;

	pTempOutImgUBuff = pOutImgBuff->data[0] + nImgSize;
	pTempOutImgVBuff = pTempOutImgUBuff + nUSize;

	memcpy(pTempOutImgUBuff, pTempInImgUBuff, nUSize*sizeof(unsigned char));
	memcpy(pTempOutImgVBuff, pTempInImgVBuff, nUSize*sizeof(unsigned char));
		
	//memset(pTempOutImgUBuff, 128, nUSize*sizeof(unsigned char));
	//memset(pTempOutImgVBuff, 128, nUSize*sizeof(unsigned char));

	//	for (j = 0 ; j < nUSize; j ++)
	//	{
	//		*(pTempOutImgUBuff++) = 66; //(*(pTempInImgUBuff++) - 100)>0?(*(pTempInImgUBuff++) - 100):0
	//		*(pTempOutImgVBuff++) = 128; // (*(pTempInImgVBuff++) + 50)<255?(*(pTempInImgVBuff++) + 50):255
	//	}

	switch (nInSubID)
	{
	case 2: // Medium
		BeautyGlass(pTempInImgYBuff, pTempImgYBuff, pTempOutImgYBuff, nWidth, nHeight, RADIUS1, 1);
		memcpy(pTempOutImgUBuff, pTempInImgUBuff, nUSize*sizeof(unsigned char));
		memcpy(pTempOutImgVBuff, pTempInImgVBuff, nUSize*sizeof(unsigned char));
		
		break;

	case 1: // Low
		BeautyGlass(pTempInImgYBuff, pTempImgYBuff, pTempOutImgYBuff, nWidth, nHeight, RADIUS3, 1);
		memcpy(pTempOutImgUBuff, pTempInImgUBuff, nUSize*sizeof(unsigned char));
		memcpy(pTempOutImgVBuff, pTempInImgVBuff, nUSize*sizeof(unsigned char));

		// Using new method in RGB
		
		break;
	case 3: // light enhance
		//apiImgProcessing->_HdrBasedOnGuiderFilter(pTempOutImgYBuff, pTempOutImgYBuff, nWidth, nHeight, nWidth, 10);
		// Using new menthod in RGB
		break;

		//
	default:
		for (j = 0 ; j < nUSize; j ++)
		{
			*(pTempOutImgUBuff++) = *(pTempInImgUBuff++); //(*(pTempInImgUBuff++) - 100)>0?(*(pTempInImgUBuff++) - 100):0
			*(pTempOutImgVBuff++) = *(pTempInImgVBuff++);
		}
		break;
	}

	if (pTempImgYBuff != NULL)
	{
		free(pTempImgYBuff);
		pTempImgYBuff = NULL;
	}
	//fflush(file);
}
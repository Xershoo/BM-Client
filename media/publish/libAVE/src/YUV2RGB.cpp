#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "YUV2RGB.h"

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

int rgb2yuv420(unsigned char* RgbBuf, int nWidth, int nHeight,	unsigned char* yuvBuf, bool bSwitch) // bSwitch switch orders
{
	//printf("1");
	int i, j;
	unsigned char*bufY, *bufU, *bufV, *bufRGB, *tmpRGB; // *bufYuv, testu, testv
	int len = nWidth * nHeight * 3 / 2;
	int nRGBSize = len<<1;
	//printf("2");
	memset(yuvBuf, 0,  len);

	//		bufU = yuvBuf + nWidth * nHeight;
	//		bufV = bufU + 1;
	//*len = 0;
	//printf("4");
	unsigned char y, u, v, r, g, b;
	int ylen = nWidth * nHeight;
	int ulen = (nWidth * nHeight) / 4;
	int vlen = (nWidth * nHeight) / 4;

	if(bSwitch)
	{
		tmpRGB = (unsigned char*)malloc(nRGBSize*sizeof(uint8_t));

		for(int k=0;k<nHeight;k++)  
			memcpy(tmpRGB+(nWidth*3)*(nHeight-k-1),RgbBuf+(nWidth*3)*k,nWidth*3);
	}

	bufY = yuvBuf;
	bufV = yuvBuf + nWidth * nHeight;
	bufU = bufV + (nWidth * nHeight* 1 / 4);

	//printf("7");
	for (j = 0; j < nHeight; j++) {
		if(!bSwitch)
			bufRGB = RgbBuf + nWidth * (j) * 3;
		else
			bufRGB = tmpRGB + nWidth * (j) * 3;

		for (i = 0; i < nWidth; i++) {
			int pos = nWidth * j + i;
			r = *(bufRGB++);
			g = *(bufRGB++);
			b = *(bufRGB++);

			y = (unsigned char)((66 * r + 129 * g + 25 * b + 128) >> 8)  + 16;
			u = (unsigned char)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
			v = (unsigned char)((112 * r - 94 * g - 18 * b + 128) >> 8)  + 128;

			//y = (unsigned char)((77 * r) + (150 * g) + (29 * b)+128)>>8;
			//u = (unsigned char)((-(44 * r) - (87 * g) + (131 * b)+128)>>8) + 128; 
			//v = (unsigned char)(((131 * r) - (110 * g) - (21 * b)+128)>>8) + 128 ;

			*(bufY++) = MAX(0, MIN(y, 255));

			if (j % 2 == 0 && i % 2 == 0) {
				if (u > 255) {
					u = 255;
				}
				if (u < 0) {
					u = 0;
				}

				*(bufU) = u;
				bufU ++;
				//存u分量
			} else {
				//存v分量
				if (i % 2 == 0) {
					if (v > 255) {
						v = 255;
					}
					if (v < 0) {
						v = 0;
					}

					*(bufV) = v;
					bufV ++;
				}
			}
		}
	}
	//
	if(bSwitch)
	{
		delete tmpRGB;
	}
	return 1;
}

//RGBA
int RGBA2YUVA420(unsigned char* RgbBuf, int nWidth, int nHeight , unsigned char* yuvaBuf)
 {
		//printf("1");
		int i, j;
		unsigned char*bufY, *bufU, *bufV , *bufA , *bufRGB;
		int len = nWidth * nHeight * 3 / 2 + nWidth * nHeight;

		//printf("2");
		memset(yuvaBuf, 0,  len);
		//printf("3");
		bufY = yuvaBuf;
		//bufU = yuvaBuf + nWidth * nHeight;
		//bufV= bufU + (nWidth * nHeight * 1 / 4);
		//bufA = bufV + (nWidth * nHeight * 1 / 4);
		bufU = yuvaBuf + nWidth * nHeight;
		bufV= bufU + (nWidth * nHeight * 1 / 4);
		bufA = bufV + (nWidth * nHeight * 1 / 4);

//		bufU = yuvBuf + nWidth * nHeight;
//		bufV = bufU + 1;
		//*len = 0;
		//printf("4");
		unsigned char y, u, v, r, g, b , a;
		int ylen = nWidth * nHeight;
		int ulen = (nWidth * nHeight) / 4;
		int vlen = (nWidth * nHeight) / 4;

		//printf("7");
		for (j = 0; j < nHeight; j++) {
			bufRGB = RgbBuf + nWidth * (j) * 4;
			for (i = 0; i < nWidth; i++) {
				int pos = nWidth * j + i;
				r = *(bufRGB++);
				g = *(bufRGB++);
				b = *(bufRGB++);
				a = *(bufRGB++);

				y = (unsigned char) ((66 * r + 129 * g + 25 * b + 128) >> 8)
						+ 16;
				u = (unsigned char) ((-38 * r - 74 * g + 112 * b + 128) >> 8)
						+ 128;
				v = (unsigned char) ((112 * r - 94 * g - 18 * b + 128) >> 8)
						+ 128;
				*(bufY++) = MAX(0, MIN(y, 255));
				*(bufA ++) = a;
				if (j % 2 == 0 && i % 2 == 0) 
				{
					if (u > 255) 
					{
						u = 255;
					}
					if (u < 0) 
					{
						u = 0;
					}

					*(bufU) = u;
					bufU ++;
					//存u分量
				} 
				else
				{
					//存v分量
					if (i % 2 == 0) 
					{
						if (v > 255) 
						{
							v = 255;
						}
						if (v < 0) {
							v = 0;
						}

						*(bufV) = v;
						bufV ++;
					}
				}
			}
		}
		//*len = nWidth * nHeight + (nWidth * nHeight) / 2;
		//printf("3");
		return 1;
}

// RGB2YUV, RGBA2YUV, RGBA2YUVA, RGB2YUVA
int RGB2YUV420(unsigned char* RgbBuf, int nRGBChannels, int nRGBWidth, int nRGBHeight , unsigned char* yuvBuf, int nYUVChannel)
{
    int  i, j;
	int nModels;
	unsigned char*bufY, *bufU, *bufV , *bufA , *bufRGB;
	unsigned char y, u, v, r, g, b, a;
	int nYUVWidth = nRGBWidth & ~3;
	int nYUVHeight = nRGBHeight & ~3;
	int ylen = nYUVWidth * nYUVHeight;
	int ulen = (nYUVWidth * nYUVHeight)>>2;
	int vlen = (nYUVWidth * nYUVHeight)>>2;
	
	int len  = 0;

    
	//nModels
	if(nYUVChannel == 3)
	{
		// RGB2YUV, RGB2YUVA
		nModels = nRGBChannels == 3?0:2;

		len = ylen + 2*ulen;
	}
	else if(nYUVChannel == 4)
	{
		// RGBA2YUV, RGBA2YUVA
		nModels = nRGBChannels == 3?1:3;
		len = ylen + 2*ulen + ylen;
	}
	else
	{
		return -1;
	}
           
	memset(yuvBuf, 0,  len);
	//printf("3");
	bufY = yuvBuf;
	//bufU = yuvaBuf + nWidth * nHeight;
	//bufV= bufU + (nWidth * nHeight * 1 / 4);
	//bufA = bufV + (nWidth * nHeight * 1 / 4);
	bufU = yuvBuf + ylen;
	bufV= bufU + ulen;
	if(nYUVChannel == 4)
	{
		bufA = bufV + vlen;
	}

//
	//printf("4");
	

	//printf("7");
	for (j = 0; j < nYUVHeight; j++) 
	{
		bufRGB = RgbBuf + nRGBWidth * (j) * nRGBChannels;
		for (i = 0; i < nYUVWidth; i++) 
		{
			int pos = nYUVWidth * j + i;

			r = *(bufRGB++);
			g = *(bufRGB++);
			b = *(bufRGB++);
			y = (unsigned char) ((66 * r + 129 * g + 25 * b + 128) >> 8)
					+ 16;
			u = (unsigned char) ((-38 * r - 74 * g + 112 * b + 128) >> 8)
					+ 128;
			v = (unsigned char) ((112 * r - 94 * g - 18 * b + 128) >> 8)
					+ 128;

			*(bufY++) = MAX(0, MIN(y, 255));
			if (j % 2 == 0 && i % 2 == 0) 
			{
				*(bufU++) = MAX(0, MIN(u, 255));
				*(bufV++) = MAX(0, MIN(v, 255));
				//存u分量
			}

			switch(nModels)
			{
			case 0:
			case 2:
                a = *(bufRGB++);
				break;
			case 1:
                a = 255;
				*(bufA++) = a;
				break;
			case 3:
                a = *(bufRGB++);
                *(bufA++) = a;
				break;
			}			
		}
	}

    return 0;
}

// RGB8882RGB565
#define RGB565_MASK_RED        0xF800  
#define RGB565_MASK_GREEN                         0x07E0  
#define RGB565_MASK_BLUE                         0x001F  
  
void rgb565_2_rgb24(unsigned char *rgb24, short rgb565)  
{   
 //extract RGB  
 rgb24[2] = (rgb565 & RGB565_MASK_RED) >> 11;     
 rgb24[1] = (rgb565 & RGB565_MASK_GREEN) >> 5;  
 rgb24[0] = (rgb565 & RGB565_MASK_BLUE);  
  
 //amplify the image  
 rgb24[2] <<= 3;  
 rgb24[1] <<= 2;  
 rgb24[0] <<= 3;
}

int yuv2rgb(unsigned char* rgb, int width, int height, unsigned char* yuv, bool bSwitch) 
{

	int frameSize = width * height;
	unsigned char *bufRGB, *tmpRGB;

	if(bSwitch)
	{
		tmpRGB = (unsigned char*)malloc(frameSize*3*sizeof(uint8_t));
		bufRGB = tmpRGB;
	}
	else
		bufRGB = rgb;

	for (int j = 0, yp = 0; j < height; j++) {
		int up = frameSize + (j >> 1) * (width >> 1), u = 0, v = 0;
		int vp = frameSize + (frameSize >> 2) + (j >> 1) * (width >> 1);
		for (int i = 0; i < width; i++, yp++) {
			int y = (0xff & ((int) yuv[yp])) - 16;
			if (y < 0)
				y = 0;
			if ((i & 1) == 0) {
				u = (0xff & yuv[up++]) - 128;
				v = (0xff & yuv[vp++]) - 128;
			}

			int y1192 = 1192 * y;
			int r = (y1192 + 1634 * v);
			int g = (y1192 - 833 * v - 400 * u);
			int b = (y1192 + 2066 * u);

			if (r < 0)
				r = 0;
			else if (r > 262143)
				r = 262143;
			if (g < 0)
				g = 0;
			else if (g > 262143)
				g = 262143;
			if (b < 0)
				b = 0;
			else if (b > 262143)
				b = 262143;

			//			int temp = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
			int temp = 0xff000000 | ((b << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((r >> 10) & 0xff);

			bufRGB[3 * yp] = (unsigned char) ((temp & 0x00ff0000) >> 16);
			bufRGB[3 * yp + 1] = (unsigned char) ((temp & 0x0000ff00) >> 8);
			bufRGB[3 * yp + 2] = (unsigned char) (temp & 0x000000ff);

		}
	}

	if(bSwitch)
	{
		for(int k=0;k<height;k++)  
			memcpy(rgb+(width*3)*(height-k-1),tmpRGB+(width*3)*k,width*3);

		delete tmpRGB;
	}

	return 1;
}

unsigned char* NV21toYUV420(unsigned char *nv21, int nWidth, int nHeight)
{
	unsigned char *yuv;
	int i, j;
	unsigned char* dstY,*dstU, *dstV, *srcUV;
	int nSize = nWidth*nHeight;
	int nSizeU = nSize>>2;

	yuv = (unsigned char*)malloc(nWidth*nHeight*3/2);

	srcUV = nv21+nSize;
	dstY = yuv;
	dstU = yuv+nWidth*nHeight;
	dstV = dstU+nSizeU;

	memcpy(yuv, nv21, nWidth*nHeight);
	// Y is same
	for(j = 0; j<nSizeU; j++)
	{
		dstU[j] = srcUV[j*2];
		dstV[j] = srcUV[j*2+1];
	}

	return yuv;
}
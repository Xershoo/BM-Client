#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include "xgdi.h"
#include "xgdidef.h"
#include "xstretch.h"

#include "xstring.h"


NAMESPACE_BEGIN(xgdi)

struct CImgStreRect : public RECT
{
    CImgStreRect(xgdi::IImage* pImage, LPRECT srcRect)
    {
        SIZE sizeImage = pImage->GetSize();
        if(srcRect == NULL)
        {
            left = top = bottom = right = 0;
        }else{
            *(LPRECT)this = *srcRect;
        }

        if(right <= 0)   right	 = sizeImage.cx + right;
        if(bottom <= 0)  bottom  = sizeImage.cy + bottom;		
    }
};
inline BOOL DrawImage(xgdi::ICanvas* pCanvas, int dLeft, int dTop, int dw, int dh, 
                      xgdi::IImage* pImage, int sLeft, int sTop, int sw, int sh, int nFlag, COLORREF clrKey)
{
    CRect destRect(dLeft, dTop, dLeft+dw, dTop+dh);
    CRect srcRect(sLeft, sTop, sLeft+sw, sTop+sh);

    if(destRect.IsRectEmpty() || srcRect.IsRectEmpty()) return FALSE;

    return pCanvas->DrawImage(pImage, destRect, srcRect,nFlag,clrKey);		
}

inline BOOL DrawStretchImage(xgdi::ICanvas* pCanvas, xgdi::IImage* pImage,
                             RECT rcDest, RECT outSrc, RECT inSrc, int nFlag,  COLORREF clrKey)
{
    BOOL bRet  = TRUE;
    CRect rectDest		= rcDest;
    CRect rcOutsrc		= outSrc;

    int leftBorder      = inSrc.left	-	rcOutsrc.left;
    int topBorder       = inSrc.top	-	rcOutsrc.top;
    int rightBorder     = rcOutsrc.right	-	inSrc.right;
    int bottomBorder    = rcOutsrc.bottom	-	inSrc.bottom;



    // left-top right-top left-bottom right-bottom
    bRet &= DrawImage(pCanvas, rectDest.left, rectDest.top, leftBorder, topBorder,
        pImage, rcOutsrc.left, rcOutsrc.top, leftBorder, topBorder, nFlag,clrKey);
    bRet &= DrawImage(pCanvas, rectDest.right - rightBorder, rectDest.top, rightBorder, topBorder,
        pImage, rcOutsrc.right - rightBorder, rcOutsrc.top, rightBorder, topBorder, nFlag,clrKey);
    bRet &= DrawImage(pCanvas, rectDest.left, rectDest.bottom - bottomBorder, leftBorder, bottomBorder,
        pImage, rcOutsrc.left, rcOutsrc.bottom - bottomBorder, leftBorder, bottomBorder, nFlag,clrKey);
    bRet &= DrawImage(pCanvas, rectDest.right - rightBorder, rectDest.bottom - bottomBorder, rightBorder, bottomBorder,
        pImage, rcOutsrc.right - rightBorder, rcOutsrc.bottom - bottomBorder, rightBorder, bottomBorder, nFlag,clrKey);

    // left right top bottom stretch
    bRet &= DrawImage(pCanvas, rectDest.left, rectDest.top + topBorder, leftBorder, rectDest.Height() - topBorder - bottomBorder,
        pImage, rcOutsrc.left, rcOutsrc.top + topBorder, leftBorder, rcOutsrc.Height() - topBorder - bottomBorder, nFlag,clrKey);
    bRet &= DrawImage(pCanvas, rectDest.right - rightBorder, rectDest.top + topBorder, rightBorder, rectDest.Height() - topBorder - bottomBorder,
        pImage, rcOutsrc.right -rightBorder, rcOutsrc.top + topBorder, rightBorder, rcOutsrc.Height() - topBorder - bottomBorder, nFlag,clrKey);
    bRet &= DrawImage(pCanvas, rectDest.left + leftBorder, rectDest.top, rectDest.Width() - leftBorder - rightBorder, topBorder,
        pImage, rcOutsrc.left + leftBorder, rcOutsrc.top, rcOutsrc.Width() - leftBorder - rightBorder, topBorder, nFlag,clrKey);
    bRet &= DrawImage(pCanvas, rectDest.left + leftBorder, rectDest.bottom - bottomBorder, rectDest.Width() - leftBorder - rightBorder, bottomBorder,
        pImage, rcOutsrc.left + leftBorder, rcOutsrc.bottom - bottomBorder, rcOutsrc.Width() - leftBorder - rightBorder, bottomBorder, nFlag,clrKey);

    if(nFlag & DIF_STRE_NO_CENTER){return bRet;}

    rectDest.DeflateRect(leftBorder, topBorder, rightBorder, bottomBorder);
    rcOutsrc.DeflateRect(leftBorder, topBorder, rightBorder, bottomBorder);
    bRet &= DrawImage(pCanvas, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
        pImage, rcOutsrc.left, rcOutsrc.top, rcOutsrc.Width(), rcOutsrc.Height(), nFlag,clrKey);

    return bRet;
}

//  [1/21/2011 魏永赳]
inline HBITMAP GetBlurBorderImage(HBITMAP hBitmap, RECT rcBlur, COLORREF clrblur)
{
	// 获取对象信息   
	BITMAP bm = {0};      
	if (!::GetObject(hBitmap, sizeof(BITMAP), (LPBYTE)&bm))   
		return NULL;   
	// 定义位图信息   
	BITMAPINFO bi;   
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);   
	bi.bmiHeader.biWidth = bm.bmWidth;   
	bi.bmiHeader.biHeight = -bm.bmHeight;   
	bi.bmiHeader.biPlanes = 1;   
	bi.bmiHeader.biBitCount = 32;    
	bi.bmiHeader.biCompression = BI_RGB;    
	bi.bmiHeader.biSizeImage = bm.bmWidth * 4 * bm.bmHeight; // 32 bit   
	bi.bmiHeader.biClrUsed = 0;   
	bi.bmiHeader.biClrImportant = 0;   

	// 获取位图数据   
	HDC hdc = GetDC(NULL);   
	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];   
	::ZeroMemory(pBits, bi.bmiHeader.biSizeImage);   
	if (!::GetDIBits(hdc, hBitmap, 0, bm.bmHeight, pBits, &bi, DIB_RGB_COLORS))   
	{   
		delete pBits;   
		pBits = NULL;   
	}   

	// 对位图数据进行处理   
	COLORREF* pSrc = (COLORREF*)pBits;   
	COLORREF* pDest = (COLORREF*)new BYTE[bi.bmiHeader.biSizeImage];   
	::ZeroMemory(pDest, bi.bmiHeader.biSizeImage);  
	memcpy_s(pDest, bi.bmiHeader.biSizeImage, pSrc, bi.bmiHeader.biSizeImage);

	int fR,fG,fB;
	fR = GetRValue(clrblur);
	fG = GetGValue(clrblur);
	fB = GetBValue(clrblur);
	for (LONG i = 0; i < bm.bmHeight; i++)      
	{      
		for (LONG j = 0; j < bm.bmWidth; j++)      
		{      
			COLORREF* pRGBDest = &pDest[i * bm.bmWidth + j];  
			int numerator = -1;
			int denominator = -1;
			if(j<rcBlur.left )
			{
				numerator = j;
				denominator = rcBlur.left;
			}
			else if(bm.bmWidth-j <= rcBlur.right)
			{
				numerator = bm.bmWidth-j-1;
				denominator = rcBlur.right;
			}
			else if(i< rcBlur.top)
			{
				numerator = i;
				denominator = rcBlur.top;
			}
			else if(bm.bmHeight-i <= rcBlur.bottom)
			{
				numerator = bm.bmHeight-i-1;
				denominator = rcBlur.bottom;
			}

			if(numerator>=0 && denominator>0)
			{
				//GetDIBits顺序为 BGR
				int b = GetRValue(*pRGBDest);
				int g = GetGValue(*pRGBDest);
				int r = GetBValue(*pRGBDest);
				r = (r*numerator + fR*(denominator-numerator))/(denominator);
				g = (g*numerator + fG*(denominator-numerator))/(denominator);
				b = (b*numerator + fB*(denominator-numerator))/(denominator);
				*pRGBDest = RGB(b,g,r);
			} 

		} //end of for j    
	}//end of for i
	// 创建新位图，设置位图数据   
	HBITMAP hblur = ::CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);   
	::SetDIBits(hdc, hblur, 0, bm.bmHeight, pDest, &bi, DIB_RGB_COLORS);   

	// 释放资源   
	delete[] pBits;   
	pBits = NULL;   
	delete[] pDest;    
	pDest = NULL;   
	::ReleaseDC(NULL,hdc);      

	return hblur;  

}


inline int MeasureText(ICanvas* pCanvas, const xstring& strText, IFont* pFont, RECT& rcText) //测量str的高宽
{
    CSize size;
    if(pCanvas)
    {
        ::GetTextExtentPoint32(pCanvas->GetUIHandle(), strText.c_str(), strText.length(), &size);
        rcText.right = rcText.left + size.cx;
        rcText.bottom = rcText.top + size.cy;
    }
    
    return size.cx;
}

inline IFont* GetDefaultFont()
{
    static XGDIObject<IFont> font = CreateGDI<IFont, IID_UIFont>();
    font->Attach(AtlGetDefaultGuiFont());
    return font;
}
inline IBrush* CreateSolidBrush(COLORREF clr)
{
    IBrush* brush = CreateGDI<IBrush, IID_UIBrush>();
    if(brush){
        brush->CreateSolidBrush(clr);
    }
    return brush;
}

NAMESPACE_END(xgdi)


class XGDIUtil
{
public:
	static BITMAPINFO* PrepareRGBBitmapInfo(int nWidth, int nHeight, int	nColorDep)
	{
		BITMAPINFO *pRes = new BITMAPINFO;
		::ZeroMemory(pRes, sizeof(BITMAPINFO));
		pRes->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pRes->bmiHeader.biWidth = nWidth;
		pRes->bmiHeader.biHeight = nHeight;
		pRes->bmiHeader.biPlanes = 1;
		pRes->bmiHeader.biBitCount = nColorDep;

		DWORD	dwRowSize			= ((nColorDep / 8 )* abs(nWidth) + 3 ) & ~3;

		pRes->bmiHeader.biSizeImage= dwRowSize * abs(nHeight);
		return pRes;
	}
	static HBITMAP GetPartBmp(HBITMAP hSrcBmp, int cx, int cy, int x, int y)
	{
		assert(hSrcBmp); 
		if(NULL == hSrcBmp)
			return NULL;

		BITMAP bmp;
		::GetObject(hSrcBmp, sizeof(BITMAP), &bmp);

		if(cx + x > bmp.bmWidth ||
			cy	 + y > bmp.bmHeight)
			return NULL;

		// check for valid size

		HDC hDC = ::GetDC(NULL);

		int bitCount =bmp.bmBitsPixel;
		int colorBits = bitCount / 8;
		BITMAPINFO* pbi = PrepareRGBBitmapInfo(bmp.bmWidth, 
			bmp.bmHeight, bitCount);
		BYTE *pData = new BYTE[pbi->bmiHeader.biSizeImage];
		ZeroMemory(pData, pbi->bmiHeader.biSizeImage);
		::GetDIBits(hDC, hSrcBmp, 0, bmp.bmHeight, pData, pbi, DIB_RGB_COLORS);

		BITMAPINFO* pnewbi = PrepareRGBBitmapInfo(cx, cy, bitCount);
		BYTE*		pData3		=NULL;
		HBITMAP	hDestBmp	=CreateDIBSection(
			hDC,                // handle to DC
			pnewbi,				// bitmap data
			DIB_RGB_COLORS,		// data type indicator
			(void**)&pData3,    // bit values
			NULL,				// handle to file mapping object
			0					// offset to bitmap bit values
			);

		int	Step		=(colorBits * bmp.bmWidth + 3) & ~3;
		BYTE*  pDestOper	=pData + Step * (bmp.bmHeight - cy - y);

		int	nRowOffset	=x * colorBits;
		int	nnewRowSize	=(colorBits * cx + 3) & ~3;

		for(int i=0 ; i<cy; i++)
		{
			memcpy(pData3 + i * nnewRowSize, pDestOper + Step * i + nRowOffset, nnewRowSize);
		}

		delete[]	pData;
		pData	=NULL;

		delete	pbi;
		pbi		=NULL;

		delete	pnewbi;
		pnewbi	=NULL;

		::ReleaseDC(NULL, hDC);

		return  hDestBmp;
	}

	static HRGN BitmapToRegion (HBITMAP hBmp, POINT ptStart,
		COLORREF cTransparentColor, COLORREF cTolerance,
		const RECT& rcMaxBounds,DWORD rgnMode = RGN_XOR)
	{
		assert(hBmp);
		if(NULL == hBmp)
			return NULL;
		int	nHitCount				=0;
		int				x=0 , y=0;
		HRGN			wndRgn;		//按钮的RGN


		BITMAP BitmapSize;
		::GetObject(hBmp, sizeof(BITMAP), &BitmapSize);

		const int COLORDEPTH	=32;
		int		  nSQUADSize    =COLORDEPTH / 8;

		BITMAPINFO *pbi = new BITMAPINFO;
		::ZeroMemory(pbi, sizeof(BITMAPINFO));
		pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbi->bmiHeader.biWidth = BitmapSize.bmWidth;
		pbi->bmiHeader.biHeight =  BitmapSize.bmHeight;
		pbi->bmiHeader.biPlanes = 1;
		pbi->bmiHeader.biBitCount = COLORDEPTH;

		WORD	nBytes				=COLORDEPTH / 8;
		pbi->bmiHeader.biSizeImage= 
			((nBytes * abs( BitmapSize.bmWidth) + 3) & ~3) * abs( BitmapSize.bmHeight);

		if(NULL == pbi)
			return NULL;

		BYTE *pData = new BYTE[pbi->bmiHeader.biSizeImage];
		if(NULL == pData)
		{
			SAFE_DELETE(pbi);
			return NULL;
		}

		HDC	hDC	=GetDC(NULL);

		::GetDIBits(hDC, hBmp, 
			0, BitmapSize.bmHeight, pData, pbi, DIB_RGB_COLORS);

		ReleaseDC(NULL, hDC);

		RECT	rcSrcRgn = {ptStart.x, ptStart.y, ptStart.x + BitmapSize.bmWidth, ptStart.y + BitmapSize.bmHeight};
		wndRgn	=::CreateRectRgnIndirect(&rcSrcRgn);

		BYTE	rTrans	=GetRValue(cTransparentColor);
		BYTE	gTrans	=GetGValue(cTransparentColor);
		BYTE	bTrans	=GetBValue(cTransparentColor);

		RECT	rcTempRgn = {0, 0, 0, 0};
		bool bRectZeroSize = true;
		int rgncount = 0;
		for(y=0; y<BitmapSize.bmHeight; y++)
		{
			rcTempRgn = NULL_RECT;
			bRectZeroSize = true;		

			for(x=0; x<BitmapSize.bmWidth; x++)
			{
				POINT point = {ptStart.x + x, ptStart.y + y};

				if(!::IsRectEmpty(&rcMaxBounds) &&
					::PtInRect(&rcMaxBounds,point))
				{
					nHitCount ++;

					if(!::IsRectEmpty(&rcTempRgn))
					{
						HRGN	hTemp	=::CreateRectRgnIndirect(&rcTempRgn);
						::CombineRgn(wndRgn, wndRgn, hTemp, rgnMode);
						SAFE_DELETEGDIOBJECT(hTemp);

						rcTempRgn = NULL_RECT;
					}
					continue;
				}

				BYTE*	pRgbData=x * nSQUADSize + pData + pbi->bmiHeader.biWidth * nSQUADSize * sizeof(BYTE) * (BitmapSize.bmHeight-y-1);
				BYTE	b		=*(pRgbData + 0);
				BYTE	g		=*(pRgbData + 1);
				BYTE	r		=*(pRgbData + 2);

				if(r == rTrans &&
					g == gTrans &&
					b == bTrans)
				{
					if(bRectZeroSize)
					{	//初始化
						rcTempRgn.left	=ptStart.x + x;
						rcTempRgn.right	=rcTempRgn.left + 1;
						rcTempRgn.top	=ptStart.y + y;
						rcTempRgn.bottom=rcTempRgn.top + 1;
						bRectZeroSize = false;
					}
					else
					{
						rcTempRgn.right	++;
					}

				}
				else
				{
					if(!bRectZeroSize)
					{
						HRGN	hTemp	=::CreateRectRgnIndirect(&rcTempRgn);
						::CombineRgn(wndRgn, wndRgn, hTemp, rgnMode);

						SAFE_DELETEGDIOBJECT(hTemp);

						rcTempRgn = NULL_RECT;

						bRectZeroSize = true;
					}				
				}
			}

			//创建rgn，并把rcTempRgn置为0
			if(!bRectZeroSize)
			{
				HRGN	hTemp	=::CreateRectRgnIndirect(&rcTempRgn);

				::CombineRgn(wndRgn, wndRgn, hTemp, rgnMode);

				SAFE_DELETEGDIOBJECT(hTemp);
			}
		}

		SAFE_DELETE_ARRAY(pData);
		SAFE_DELETE(pbi);
		return wndRgn;
	}



	/******************************************************
	******************************************************/

	static HRGN GetImageRgn(xgdi::IImage* pImage,const geometry::Rect& rect,COLORREF clrKey)
	{
		using namespace geometry;
		if(pImage == NULL) return NULL;
		HBITMAP hBmp = XGDIUtil::GetPartBmp(pImage->GetUIHandle(), rect.w, rect.h, rect.left, rect.top);
		if(hBmp){
			HRGN hTempRgn = ::CreateRectRgn(0,0,rect.w,rect.h) ;

			HRGN hRgn = XGDIUtil::BitmapToRegion(hBmp, NULL_POINT, clrKey, 0, NULL_RECT);

			::CombineRgn(hRgn,hTempRgn,hRgn,RGN_DIFF);

			SAFE_DELETEGDIOBJECT(hBmp);
			SAFE_DELETEGDIOBJECT(hTempRgn);
			return hRgn;
		}
		return NULL;
	}
	static xgdi::StretchRgn CreateStretchRgn(xgdi::StretchImg& stretch, xgdi::IImage* pImage, COLORREF clrKey)
	{
		using namespace geometry;
		xgdi::StretchRgn results;

		results.rgnLT	= GetImageRgn(pImage, stretch.GetPart(xgdi::StretchPart_LT),clrKey);
		results.rgnRT	= GetImageRgn(pImage, stretch.GetPart(xgdi::StretchPart_RT),clrKey);
		results.rgnLB	= GetImageRgn(pImage, stretch.GetPart(xgdi::StretchPart_LB),clrKey);
		results.rgnRB	= GetImageRgn(pImage, stretch.GetPart(xgdi::StretchPart_RB),clrKey);
		return results;
	}
	static HRGN CombineRgn(xgdi::StretchImg& stretch,const xgdi::StretchRgn& corners,const geometry::Rect& rcBounds)
	{
		HRGN hRgn = ::CreateRectRgnIndirect(&(RECT)rcBounds);

		if(corners.rgnLT != NULL)
		{
			::CombineRgn(hRgn,hRgn,corners.rgnLT,RGN_DIFF);
		}
		if(corners.rgnRT != NULL)
		{
			geometry::Rect rect= stretch.GetPart(xgdi::StretchPart_RT);
			::OffsetRgn(corners.rgnRT,rcBounds.w - rect.w,0);
			::CombineRgn(hRgn,hRgn,corners.rgnRT,RGN_DIFF);
			::OffsetRgn(corners.rgnRT,rect.w - rcBounds.w,0);
		}
		if(corners.rgnLB != NULL)
		{
			geometry::Rect rect= stretch.GetPart(xgdi::StretchPart_LB);
			::OffsetRgn(corners.rgnLB,0,rcBounds.h - rect.h);
			::CombineRgn(hRgn,hRgn,corners.rgnLB,RGN_DIFF);
			::OffsetRgn(corners.rgnLB,0,rect.h - rcBounds.h);
		}
		if(corners.rgnRB != NULL)
		{
			geometry::Rect rect= stretch.GetPart(xgdi::StretchPart_RB);
			::OffsetRgn(corners.rgnRB,rcBounds.w - rect.w,rcBounds.h - rect.h);
			::CombineRgn(hRgn,hRgn,corners.rgnRB,RGN_DIFF);
			::OffsetRgn(corners.rgnRB,rect.w - rcBounds.w,rect.h - rcBounds.h);
		}
		return hRgn;
	}
};


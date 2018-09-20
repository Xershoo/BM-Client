// XPdfView.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "XPdfView.h"
#include "PdfShow.h"
#include <stdio.h>

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"msimg32.lib")


#define PRINT_BUF_SIZE			(4096)

#define ALLOC_UNIT  (100)

extern HINSTANCE g_hInstance;

INT WINAPI PrintTraceW(WCHAR *format, ...)
{ 
	va_list ap; 
	static HANDLE s_hLog = NULL;
	WCHAR buf[PRINT_BUF_SIZE] = { 0 };
	DWORD dwSize = 0;

	va_start(ap, format); 

	int n = _vsnwprintf_s(buf,PRINT_BUF_SIZE, format, ap);
	lstrcatW(buf,L"\r\n");

	OutputDebugStringW(buf);
	va_end(ap);

	return n;
}

INT WINAPI PrintTraceA(CHAR *format, ...)
{ 
	va_list ap; 
	static HANDLE s_hLog = NULL;
	CHAR buf[PRINT_BUF_SIZE] = { 0 };
	DWORD dwSize = 0;

	va_start(ap, format); 

	int n = _vsnprintf_s(buf,PRINT_BUF_SIZE,format, ap);
	lstrcatA(buf,"\r\n");


	OutputDebugStringA(buf);
	va_end(ap);

	return n;
}

HRGN BitmapToRegion(UINT nBmpId,COLORREF cTransparentColor,COLORREF cTolerance)
{
	HRGN hRgn = NULL;
	HBITMAP hBmp = NULL;
	HBITMAP hBmp32 = NULL;
	HDC hMemDC = NULL;
	HDC hDC = NULL;
	BITMAP bmp = { 0 };
	BITMAP bmp32 = { 0 };
    BITMAPINFOHEADER bmpHdr = { 0 };
	void * pBmpData = NULL;
	HBITMAP hOldBmp = NULL;
	HBITMAP hOldBmp2 = NULL;
	
	do 
	{
		hBmp = LoadBitmap(g_hInstance,MAKEINTRESOURCE(nBmpId));
		if(NULL==hBmp)
		{
			break;
		}
		hMemDC = CreateCompatibleDC(NULL);
		if(NULL==hMemDC)
		{
			break;
		}

		GetObject(hBmp, sizeof(bmp), &bmp);
		bmpHdr.biSize = sizeof(BITMAPINFOHEADER);
		bmpHdr.biWidth = bmp.bmWidth;
		bmpHdr.biHeight = bmp.bmHeight;
		bmpHdr.biPlanes = 1;
		bmpHdr.biBitCount = 32;
		bmpHdr.biCompression = BI_RGB;

		hBmp32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&bmpHdr, DIB_RGB_COLORS, &pBmpData, NULL, 0);
		if(NULL == hBmp32)
		{
			break;
		}

		hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp32);
		hDC = CreateCompatibleDC(hMemDC);
		if(NULL == hDC)
		{
			break;
		}
			
		// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
		GetObject(hBmp32, sizeof(BITMAP), &bmp32);
		while (bmp32.bmWidthBytes % 4)
				bmp32.bmWidthBytes++;

		// Copy the bitmap into the memory DC
		hOldBmp2 = (HBITMAP)SelectObject(hDC, hBmp);
		BitBlt(hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hDC, 0, 0, SRCCOPY);
		
		DWORD maxRects = ALLOC_UNIT;
		HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
		RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
		pData->rdh.dwSize = sizeof(RGNDATAHEADER);
		pData->rdh.iType = RDH_RECTANGLES;
		pData->rdh.nCount = pData->rdh.nRgnSize = 0;
		SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

		// Keep on hand highest and lowest values for the "transparent" pixels
		BYTE lr = GetRValue(cTransparentColor);
		BYTE lg = GetGValue(cTransparentColor);
		BYTE lb = GetBValue(cTransparentColor);
		BYTE hr = min(0xff, lr + GetRValue(cTolerance));
		BYTE hg = min(0xff, lg + GetGValue(cTolerance));
		BYTE hb = min(0xff, lb + GetBValue(cTolerance));

		// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
		BYTE *p32 = (BYTE *)bmp32.bmBits + (bmp32.bmHeight - 1) * bmp32.bmWidthBytes;
		for (int y = 0; y < bmp.bmHeight; y++)
		{
			// Scan each bitmap pixel from left to right
			for (int x = 0; x < bmp.bmWidth; x++)
			{
				// Search for a continuous range of "non transparent pixels"
				int x0 = x;
				LONG *p = (LONG *)p32 + x;
				while (x < bmp.bmWidth)
				{
					BYTE b = GetRValue(*p);
					if (b >= lr && b <= hr)
					{
						b = GetGValue(*p);
						if (b >= lg && b <= hg)
						{
							b = GetBValue(*p);
							if (b >= lb && b <= hb)
								// This pixel is "transparent"
								break;
						}
					}
					p++;
					x++;
				}

				if (x > x0)
				{
					// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
					if (pData->rdh.nCount >= maxRects)
					{
						GlobalUnlock(hData);
						maxRects += ALLOC_UNIT;
						hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
						pData = (RGNDATA *)GlobalLock(hData);
					}
					RECT *pr = (RECT *)&pData->Buffer;
					SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
					if (x0 < pData->rdh.rcBound.left)
						pData->rdh.rcBound.left = x0;
					if (y < pData->rdh.rcBound.top)
						pData->rdh.rcBound.top = y;
					if (x > pData->rdh.rcBound.right)
						pData->rdh.rcBound.right = x;
					if (y+1 > pData->rdh.rcBound.bottom)
						pData->rdh.rcBound.bottom = y+1;
					pData->rdh.nCount++;

					// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
					// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
					if (pData->rdh.nCount == 2000)
					{
						HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
						if (hRgn)
						{
							CombineRgn(hRgn, hRgn, h, RGN_OR);
							DeleteObject(h);
						}
						else
							hRgn = h;
						pData->rdh.nCount = 0;
						SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
					}
				}
			}

			// Go to next row (remember, the bitmap is inverted vertically)
			p32 -= bmp32.bmWidthBytes;
		}

		// Create or extend the region with the remaining rectangles
		HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
		if (hRgn)
		{
			CombineRgn(hRgn, hRgn, h, RGN_OR);
			DeleteObject(h);
		}
		else
			hRgn = h;

		// Clean up
		GlobalFree(hData);
		SelectObject(hDC, hOldBmp2);
		SelectObject(hMemDC, hOldBmp);	
	} while(FALSE);

	SAFE_DELETE_OBJECT(hBmp);
	SAFE_DELETE_OBJECT(hBmp32);

	if(hMemDC){
		DeleteDC(hMemDC);
		hMemDC = NULL;
	}

	if(hDC){
		DeleteDC(hDC);
		hDC = NULL;
	}
	return hRgn;
}


//////////////////////////////////////////////////////////////////////////
///
/// 2014.12.18 xiewenbing
/// export api
///
XPDFVIEW_API int WINAPI OpenPDFFile(WCHAR* pwszFileName,HWND hShowWnd,RECT rc,LPARAM lParam/*=NULL*/,BOOL bCtrl/*=TRUE*/)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	return pMgr->OpenPDFFile(pwszFileName,hShowWnd,rc,lParam,bCtrl);
}

XPDFVIEW_API void WINAPI ClosePDFFile(int nId)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	pMgr->ClosePDFFile(nId);
}

XPDFVIEW_API void WINAPI PDFNextPage(int nId)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	pMgr->PdfNextPage(nId);
}

XPDFVIEW_API void WINAPI PDFPrevPage(int nId)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	pMgr->PdfPrevPage(nId);
}

XPDFVIEW_API void WINAPI PDFZoomIn(int nId)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	pMgr->PdfZoomIn(nId);
}

XPDFVIEW_API void WINAPI PDFZoomOut(int nId)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	pMgr->PdfZoomOut(nId);
}

XPDFVIEW_API void WINAPI PDFShow(int nId,BOOL bShow)
{
	CPdfShowMgr* pMgr = CPdfShowMgr::GetInstance();

	pMgr->PdfShow(nId,bShow);
}
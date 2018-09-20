#include "stdafx.h"
#include"SourScreen.h"
#include "gdiPlus.h"
using namespace Gdiplus;

CSourScreen::CSourScreen(HWND hShowWindow,string sname):ISourData(SOURCESCREEN,sname)
{
	m_hShowWindow = hShowWindow;
	m_bIsEffective = false;
	m_iSrcVideoWidth  = 0;
	m_iSrcVideoHeight = 0;

	m_hDesktopCompatibleBitmap = NULL;
	m_hDesktopCompatibleDC = NULL;
	m_hDesktopDC  = NULL;
	m_hDesktopWnd = NULL;
	m_pBits       = NULL;
	m_pBuffer = NULL;
	m_bIsOpen = false;
	m_pBmpInfo = NULL;
	 m_nSourID = -1;
	if(strcmp(sname.c_str(),"screen") == 0)
		m_bIsFullScreen = false;
	else
		m_bIsFullScreen = true;
}

CSourScreen::~CSourScreen()
{
	if(m_pBmpInfo == NULL)
	{
		delete m_pBmpInfo;
		m_pBmpInfo = NULL;
	}
	m_bIsOpen = false;

	if(m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	if(m_hDesktopCompatibleDC2)
		::DeleteDC(m_hDesktopCompatibleDC2);
	if(m_hDesktopCompatibleBitmap2)
		::DeleteObject(m_hDesktopCompatibleBitmap2);
	if(m_hDesktopCompatibleDC)
		::DeleteDC(m_hDesktopCompatibleDC);
	if(m_hDesktopCompatibleBitmap)
		::DeleteObject(m_hDesktopCompatibleBitmap);
	if(m_hDesktopWnd)
	    ::ReleaseDC(m_hDesktopWnd,m_hDesktopDC);

}

void CSourScreen::ScreenCapture()
{
	if(!m_bIsOpen)
	{
		HWND HPARENT = NULL;
		if(m_bIsFullScreen)
		{
			HPARENT = ::GetDesktopWindow();
		}
		else
		{
			HPARENT = m_hShowWindow;
		}

		RECT  rc;
		GetClientRect(HPARENT,&rc);

		m_iSrcVideoWidth = (rc.right - rc.left)/4 * 4;
		m_iSrcVideoHeight = (rc.bottom - rc.top)/4 * 4;

		BITMAPINFO	bmpInfo;
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biBitCount= 24;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biWidth=m_iSrcVideoWidth;//GetSystemMetrics(SM_CXSCREEN);
		bmpInfo.bmiHeader.biHeight= m_iSrcVideoHeight; //GetSystemMetrics(SM_CYSCREEN);
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biSizeImage=abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biWidth*bmpInfo.bmiHeader.biBitCount/8;

			//À´ª∫≥Â£¨∑¿÷π Û±Í…¡À∏
		m_hDesktopWnd=HPARENT;//::GetDesktopWindow();
		m_hDesktopDC=::GetDC(m_hDesktopWnd);
		m_hDesktopCompatibleDC=::CreateCompatibleDC(m_hDesktopDC);
		m_hDesktopCompatibleBitmap=::CreateCompatibleBitmap(m_hDesktopDC,m_iSrcVideoWidth,m_iSrcVideoHeight);
		if(m_hDesktopCompatibleDC && m_hDesktopCompatibleBitmap)
		{
			::SelectObject(m_hDesktopCompatibleDC,m_hDesktopCompatibleBitmap);
		}

		m_hDesktopCompatibleDC2=::CreateCompatibleDC(m_hDesktopCompatibleDC);
		m_hDesktopCompatibleBitmap2=::CreateDIBSection(m_hDesktopCompatibleDC2,&bmpInfo,DIB_RGB_COLORS,&m_pBits,NULL,0);
		if(m_hDesktopCompatibleDC2 && m_hDesktopCompatibleBitmap2)
		{
			::SelectObject(m_hDesktopCompatibleDC2,m_hDesktopCompatibleBitmap2);
		}

		m_pBmpInfo = NULL;//BmpInfo÷∏’Î
		m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*255);

		HANDLE hloc = NULL;
		RGBQUAD *rgbquad = NULL;
		hloc = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
		rgbquad = (RGBQUAD *) LocalLock(hloc);
		//≥ı ºªØµ˜…´∞Â
		for(int i=0;i<256;i++)
		{
			rgbquad[i].rgbBlue=i;
			rgbquad[i].rgbGreen=i;
			rgbquad[i].rgbRed=i;
			rgbquad[i].rgbReserved=0;
		}

		m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pBmpInfo->bmiHeader.biPlanes = 1;
		m_pBmpInfo->bmiHeader.biBitCount = 24;
		m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
		memcpy(m_pBmpInfo->bmiColors, rgbquad, sizeof(RGBQUAD) * 256);

		LocalUnlock(hloc);
		LocalFree(hloc);
		m_bIsOpen = true;
	}
}

bool CSourScreen::GetBuffer(unsigned char** pBuf,unsigned int& nBufSize)
{
	if(m_bIsEffective)
	{
		BitBlt(m_hDesktopCompatibleDC,0,0,m_iSrcVideoWidth,m_iSrcVideoHeight,m_hDesktopDC,0,0,SRCCOPY);
		BitBlt(m_hDesktopCompatibleDC2,0,0,m_iSrcVideoWidth,m_iSrcVideoHeight,m_hDesktopCompatibleDC,0,0,SRCCOPY|CAPTUREBLT);
		if(m_pBuffer)
		{
			nBufSize = m_iSrcVideoWidth*m_iSrcVideoHeight*3;
			memcpy(m_pBuffer,m_pBits,nBufSize);
			*pBuf = m_pBuffer; 

			//if(m_bIsFullScreen)
			//	ShowImage(*pBuf);
			return true;
		}
	}
	return false;
}


bool CSourScreen::SetSourType(int nSourType)
{
	int nTemp = (nSourType & SOURCESCREEN);
	if(nTemp == SOURCESCREEN)
		m_bIsEffective = true;
	else
		m_bIsEffective = false;
	return true;
}

int  CSourScreen::getSourID() 
{
	return m_nSourID;
}

bool CSourScreen::SourOpen(void* param) 
{
	bool bIsOpen = true;
	if( m_bIsEffective )
	{
		StreamParam sParam;
		if(param)
			 sParam = (*(StreamParam*)param);
		m_nSourID = sParam.VU[0].nSelectCameraID;
		if(sParam.hShowHwnd)
			m_hShowWindow = sParam.hShowHwnd;

		bool bIsCmp = (sParam.bIsFullScreen == m_bIsFullScreen);

		if(bIsCmp)
		{
			ScreenCapture();
			if(m_pBuffer == NULL)
			{
				m_pBuffer = new unsigned char[m_iSrcVideoWidth * m_iSrcVideoHeight * 3];
			}
			bIsOpen = true;
		}
		else
		{
			m_bIsEffective = false;
		}
	}
	return bIsOpen;
}

bool CSourScreen::SourClose(bool bIsColseDev)
{
	m_bIsEffective = false;
	return true;
}

void CSourScreen::GetVideoWAndH(int& nW,int& nH)
{
	if(m_bIsEffective)
	{
		nW = m_iSrcVideoWidth;
		nH = m_iSrcVideoHeight;
	}
	else
	{
		nW = 0;
		nH = 0;
	}
}

bool CSourScreen::AVE_SetID(int nMainValue, int nSubValue, int nParams[2])
{
	
	return true;
}

long CSourScreen::GetVideoFPS()
{
	return 0;
}

void CSourScreen::ShowImage(unsigned char* pBuf)
{
	if(m_hShowWindow && pBuf)
	{
		RECT  rc;
		GetClientRect(m_hShowWindow,&rc);
		int nPicWidth = m_iSrcVideoWidth;
		int nPicHeight = m_iSrcVideoHeight;
		m_pBmpInfo->bmiHeader.biHeight = -m_iSrcVideoHeight;	
		m_pBmpInfo->bmiHeader.biWidth =  m_iSrcVideoWidth;
		Rect  showRect;	
		showRect.X=rc.left;
		showRect.Y=rc.top;
		showRect.Width=rc.right-rc.left;
		showRect.Height=rc.bottom-rc.top;

		HDC myHdc=GetDC(m_hShowWindow);
		int ret = SetStretchBltMode(myHdc,STRETCH_HALFTONE);

		ret = StretchDIBits(myHdc,0,showRect.Height,showRect.Width,-showRect.Height,
			0,0,nPicWidth,nPicHeight,pBuf,m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
		int n = GetLastError();
		ReleaseDC(m_hShowWindow,myHdc);
	}
}

void CSourScreen::ShowImage(unsigned char* pBuf,unsigned int nBufSize,int nVW,int nVH)
{
	if(m_hShowWindow && pBuf)
	{
		RECT  rc;
		GetClientRect(m_hShowWindow,&rc);
		int nPicWidth = nVW;
		int nPicHeight = nVH;
		m_pBmpInfo->bmiHeader.biHeight = -nVH;	
		m_pBmpInfo->bmiHeader.biWidth =  nVW;
		Rect  showRect;	
		showRect.X=rc.left;
		showRect.Y=rc.top;
		showRect.Width=rc.right-rc.left;
		showRect.Height=rc.bottom-rc.top;

		HDC myHdc=GetDC(m_hShowWindow);
		int ret = SetStretchBltMode(myHdc,STRETCH_HALFTONE);

		ret = StretchDIBits(myHdc,0,showRect.Height,showRect.Width,-showRect.Height,
			0,0,nPicWidth,nPicHeight,pBuf,m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
		int n = GetLastError();
	}
	
}

void CSourScreen::GetShowHwndRect(ScRect& rc)
{
	if(m_hShowWindow)
	{
		RECT  show;
		GetClientRect(m_hShowWindow,&show);
		rc.nLeft = 0;
		rc.nRight = show.right - show.left;
		rc.nBottom =show.bottom - show.top;
		rc.nTop =0;
	}
}
#include "ShowBGByGDI.h"

CShowBGByGDI::CShowBGByGDI(PBITMAP hBGbitmap,HWND hShowWnd):CShowVideo(DRAWTYPEGDI)
{
	m_pBmpInfo = NULL;//BmpInfo指针
	m_pRGBBuf = NULL;
	m_nRGBSize = 0;
	m_nBGPicWidth = 0;
	m_nBGPicHeight = 0;
	m_pRGBBackBuf = NULL;
	m_nRGBBackSize = 0;
	InitShowEnv();
	memset(&m_showBGRect,0,sizeof(m_showBGRect));
	memset(&m_show_rect,0,sizeof(HShowWindow));
	m_show_rect.hwnd = hShowWnd;
	CopyBGImage(hBGbitmap);
}

CShowBGByGDI::~CShowBGByGDI()
{
	UninitShowEnv();
	if( m_pRGBBackBuf )
	{
		delete m_pRGBBackBuf;
		m_pRGBBackBuf = NULL;
	}
	m_nRGBBackSize = 0;
}

bool CShowBGByGDI::ShowVideo(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFlie,IMediaPlayerEvent* pShowCallback) 
{
	return false;
}


bool CShowBGByGDI::InitShowEnv()
{
	m_pBmpInfo = (BITMAPINFO *)malloc(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*255);
	HANDLE hloc = NULL;
	RGBQUAD *rgbquad = NULL;
	hloc = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	rgbquad = (RGBQUAD *) LocalLock(hloc);
	//初始化调色板
	for(int i=0;i<256;i++)
	{
		rgbquad[i].rgbBlue=i;
		rgbquad[i].rgbGreen=i;
		rgbquad[i].rgbRed=i;
		rgbquad[i].rgbReserved=0;
	}

	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 32;
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	memcpy(m_pBmpInfo->bmiColors, rgbquad, sizeof(RGBQUAD) * 256);

	LocalUnlock(hloc);
	LocalFree(hloc);
	return true;
}

bool CShowBGByGDI::UninitShowEnv()
{
	//m_drawBGLock.Lock();
	if(m_pBmpInfo)
	{
		delete m_pBmpInfo;
		m_pBmpInfo = NULL;
	}

	if(m_pRGBBuf)
	{
		delete m_pRGBBuf;
		m_pRGBBuf = NULL;
	}
	//m_drawBGLock.Unlock();
	return true;
}

void CShowBGByGDI::CopyBGImage(PBITMAP hBGbitmap)
{
	if(hBGbitmap)
	{
		HBITMAP hbitmap = CreateBitmapIndirect(hBGbitmap);
		m_drawBGLock.Lock();
		if(m_pRGBBuf)
		{
			delete m_pRGBBuf;
			m_pRGBBuf = NULL;
		}
		m_nRGBSize = ((PBITMAP)hBGbitmap)->bmHeight * ((PBITMAP)hBGbitmap)->bmWidth * 4;
		m_pRGBBuf =  new char [m_nRGBSize];

		if(hbitmap && !GetBitmapBits(hbitmap,m_nRGBSize,m_pRGBBuf))
		{
			m_nRGBSize = 0;
		}
		else
		{
			m_nBGPicWidth = ((PBITMAP)hBGbitmap)->bmWidth;
			m_nBGPicHeight = ((PBITMAP)hBGbitmap)->bmHeight;
		}
		m_drawBGLock.Unlock();
		if(hbitmap)
		{
			DeleteObject(hbitmap);
			hbitmap = NULL;
		}
	}
}

bool CShowBGByGDI::DrawBGImage()
{
	//m_drawBGLock.Lock();
	if(m_pBmpInfo && m_pRGBBuf && m_nRGBSize > 0 && m_show_rect.hwnd)
	{
		m_pBmpInfo->bmiHeader.biHeight = m_nBGPicHeight;	
		m_pBmpInfo->bmiHeader.biWidth =  m_nBGPicWidth;
		m_pBmpInfo->bmiHeader.biBitCount = 32;

		RECT  rc;
		GetClientRect(m_show_rect.hwnd,&rc);
		HDC myHdc=GetDC(m_show_rect.hwnd);
		int ret = SetStretchBltMode(myHdc,STRETCH_DELETESCANS);

				//yuv出来的是左下角原点，windows是左上角原点，所以这里要反着显示
		int nShowWidth = rc.right-rc.left;
		int nShowHeight = rc.bottom-rc.top;

		if(nShowWidth > 0 && nShowHeight > 0)
		{
			float fltRate = (m_nBGPicWidth * 1.0) / m_nBGPicHeight;
			RECT  showRect;
			memset(&showRect,0,sizeof(RECT));

			int nTopx = rc.left + nShowWidth / 2;
			int nTopy = rc.top +  nShowHeight / 2;

			
			if(nShowWidth < m_nBGPicWidth || nShowHeight < nShowHeight)
			{
				float fltShow = ((nShowWidth * 1.0) / nShowHeight);
				if(fltShow < fltRate)
				{
					 nShowHeight = nShowWidth / fltRate;
				}
				else if(fltShow > fltRate)
				{
					 nShowWidth = nShowHeight * fltRate;
				}

				showRect.left = nTopx - nShowWidth / 2;
				showRect.right = showRect.left + nShowWidth;

				showRect.top = nTopy - nShowHeight / 2;
				showRect.bottom = showRect.top + nShowHeight;

				nShowWidth = showRect.right - showRect.left;
				nShowHeight = showRect.bottom - showRect.top;

			}
			else
			{
				showRect.left = nTopx - m_nBGPicWidth / 2;
				showRect.right = showRect.left + m_nBGPicWidth;

				showRect.top = nTopy - m_nBGPicHeight / 2;
				showRect.bottom = showRect.top + m_nBGPicHeight;

				nShowWidth = showRect.right - showRect.left;
				nShowHeight = showRect.bottom - showRect.top;
			}

			ret = StretchDIBits(myHdc,showRect.left,showRect.top,nShowWidth,nShowHeight,
						0,0,m_nBGPicWidth,m_nBGPicHeight,
						m_pRGBBuf, m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
		}
		ReleaseDC(m_show_rect.hwnd,myHdc);
	}
	//m_drawBGLock.Unlock();
	return false;
}

bool CShowBGByGDI::DrawBGImage(unsigned char* pRGB,unsigned int nRGBSize, unsigned int nVH,int nVW,bool bIsPlayFlie)
{
	//m_drawBGLock.Lock();
	if(m_pBmpInfo && pRGB && nRGBSize > 0 && m_show_rect.hwnd)
	{
		m_pBmpInfo->bmiHeader.biHeight = nVH;	
		m_pBmpInfo->bmiHeader.biWidth =  nVW;
		m_pBmpInfo->bmiHeader.biBitCount = 24;
		RECT  rc;
		GetClientRect(m_show_rect.hwnd,&rc);
		HDC myHdc=GetDC(m_show_rect.hwnd);
		int ret = SetStretchBltMode(myHdc,STRETCH_DELETESCANS);
					//yuv出来的是左下角原点，windows是左上角原点，所以这里要反着显示
		if(bIsPlayFlie)
		{
			m_show_rect.bottom = rc.bottom;
			m_show_rect.left = rc.left;
			m_show_rect.right = rc.right;
			m_show_rect.top = rc.top;
			int nShowVideoW =  abs(m_show_rect.right - m_show_rect.left);
			int nShowVideoH =  abs(m_show_rect.bottom - m_show_rect.top);
			int nWindowHeight = nShowVideoH;

			if(nShowVideoH <= 0 || nShowVideoW <= 0 || nVH <= 0)
			{
				ReleaseDC(m_show_rect.hwnd,myHdc);
				return false;
			}

			float fltRate = (nVW * 1.0) / nVH;

			int nTopx = m_show_rect.left + nShowVideoW / 2;
			int nTopy = m_show_rect.top +  nShowVideoH / 2;	

			if(nShowVideoW !=  nVW && nShowVideoH != nVH)
			{
				float fltShow = ((nShowVideoW * 1.0) / nShowVideoH);
				if(fltShow < fltRate)
				{
					nShowVideoH = nShowVideoW / fltRate;
				}
				else if(fltShow > fltRate)
				{
					nShowVideoW = nShowVideoH * fltRate;
				}
				rc.left = nTopx - (nShowVideoW/2);
				rc.right = nTopx + (nShowVideoW/2);
				rc.top =  nTopy - (nShowVideoH/2);
				//rc.bottom = nTopy + (nShowVideoH/2);
				if(fltRate > 1.0)
				{
					rc.bottom = nTopy + (nShowVideoH/2);
				}
				else
				{
					rc.bottom = m_show_rect.bottom;
				}
			}
			else
			{
				rc.left = m_show_rect.left;
				rc.right = m_show_rect.right;
				rc.top = m_show_rect.top;
				rc.bottom = m_show_rect.bottom;
			}	

			if(m_pRGBBackBuf == NULL)
			{
				m_nRGBBackSize = 640 * 480 * 3;
				m_pRGBBackBuf = new unsigned char[m_nRGBBackSize];
				memset(m_pRGBBackBuf,0,m_nRGBBackSize);

			}
			bool bIsDraw = ((m_showBGRect.left != m_show_rect.left) || (m_showBGRect.right != m_show_rect.right) 
				|| (m_showBGRect.top != m_show_rect.top) ||( m_showBGRect.bottom != m_show_rect.bottom));

			if(bIsDraw )
			{
				DrawBGImage(m_pRGBBackBuf,m_nRGBBackSize,480,640,false);
				m_showBGRect.left = m_show_rect.left;
				m_showBGRect.right = m_show_rect.right;
				m_showBGRect.top = m_show_rect.top;
				m_showBGRect.bottom = m_show_rect.bottom;
			}

			int nShowWidth = rc.right-rc.left;
			int nShowHeight = rc.bottom-rc.top;

			ret = StretchDIBits(myHdc,rc.left,nShowHeight+(nWindowHeight-nShowHeight)/2,nShowWidth,-nShowHeight,
					0,0,nVW,nVH,pRGB, m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);

		}
		else
		{

			int nPicWidth = nVW;
			int nPicHeight = nVH;
			int nShowWidth = rc.right-rc.left;
			int nShowHeight = rc.bottom-rc.top;

			if(nShowWidth > 0 && nShowHeight > 0)
			{
				if(nPicWidth /4 != nPicHeight/3)
				{
					m_show_rect.bottom = rc.bottom;
					m_show_rect.left = rc.left;
					m_show_rect.right = rc.right;
					m_show_rect.top = rc.top;
					int nShowVideoW =  abs(m_show_rect.right - m_show_rect.left);
					int nShowVideoH =  abs(m_show_rect.bottom - m_show_rect.top);
					int nWindowHeight = nShowVideoH;

					if(nShowVideoH <= 0 || nShowVideoW <= 0 || nVH <= 0)
					{
						ReleaseDC(m_show_rect.hwnd,myHdc);
						return false;
					}

					float fltRate = (nVW * 1.0) / nVH;

					int nTopx = m_show_rect.left + nShowVideoW / 2;
					int nTopy = m_show_rect.top +  nShowVideoH / 2;	

					if(nShowVideoW !=  nVW && nShowVideoH != nVH)
					{
						float fltShow = ((nShowVideoW * 1.0) / nShowVideoH);
						if(fltShow < fltRate)
						{
							nShowVideoH = nShowVideoW / fltRate;
						}
						else if(fltShow > fltRate)
						{
							nShowVideoW = nShowVideoH * fltRate;
						}
						rc.left = nTopx - (nShowVideoW/2);
						rc.right = nTopx + (nShowVideoW/2);
						rc.top =  nTopy - (nShowVideoH/2);
						//rc.bottom = nTopy + (nShowVideoH/2);
						if(fltRate > 1.0)
						{
							rc.bottom = nTopy + (nShowVideoH/2);
						}
						else
						{
							rc.bottom = m_show_rect.bottom;
						}
					}
					else
					{
						rc.left = m_show_rect.left;
						rc.right = m_show_rect.right;
						rc.top = m_show_rect.top;
						rc.bottom = m_show_rect.bottom;
					}	

					if(m_pRGBBackBuf == NULL)
					{
						m_nRGBBackSize = 640 * 480 * 3;
						m_pRGBBackBuf = new unsigned char[m_nRGBBackSize];
						memset(m_pRGBBackBuf,0,m_nRGBBackSize);

					}
					bool bIsDraw = ((m_showBGRect.left != m_show_rect.left) || (m_showBGRect.right != m_show_rect.right) 
						|| (m_showBGRect.top != m_show_rect.top) ||( m_showBGRect.bottom != m_show_rect.bottom));

					if(bIsDraw )
					{
						DrawBGImage(m_pRGBBackBuf,m_nRGBBackSize,480,640,false);
						m_showBGRect.left = m_show_rect.left;
						m_showBGRect.right = m_show_rect.right;
						m_showBGRect.top = m_show_rect.top;
						m_showBGRect.bottom = m_show_rect.bottom;
					}

					//yuv出来的是左下角原点，windows是左上角原点，所以这里要反着显示
					int nShowWidth = rc.right-rc.left;
					int nShowHeight = rc.bottom-rc.top;

					ret = StretchDIBits(myHdc,rc.left,nShowHeight+(nWindowHeight-nShowHeight)/2,nShowWidth,-nShowHeight,
							0,0,nVW,nVH,pRGB, m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
				}
				else
				{
					RECT  showRect;	
					showRect.left=0;
					showRect.right = showRect.left + nPicWidth;
					showRect.top=0;
					showRect.bottom = showRect.top + nPicHeight;

					m_pBmpInfo->bmiHeader.biWidth =  nVW;
					m_pBmpInfo->bmiHeader.biHeight = -nVH;

					ret = StretchDIBits(myHdc,showRect.left,showRect.top,nShowWidth,nShowHeight,
					0,0,nPicWidth,nPicHeight,pRGB,m_pBmpInfo, DIB_RGB_COLORS,SRCCOPY);
				}
			}
		}
		ReleaseDC(m_show_rect.hwnd,myHdc);
	}
	return false;
}

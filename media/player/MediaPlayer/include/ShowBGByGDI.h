#ifndef SHOWBGBYGDI_H
#define SHOWBGBYGDI_H
#include "ShowVideo.h"
#include "PlayHeader.h"
#include "sc_CSLock.h"

class CShowBGByGDI : public CShowVideo
{
public:
	CShowBGByGDI(PBITMAP hBGbitmap,HWND hShowWnd);
	virtual ~CShowBGByGDI();
public:
	virtual bool ShowVideo(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFlie = false,IMediaPlayerEvent* pShowCallback = NULL) ;
	virtual bool DrawBGImage();
	virtual bool DrawBGImage(unsigned char* pRGB,unsigned int nRGBSize, unsigned int nVH,int nVW,bool bIsPlayFlie);
private:
	virtual bool InitShowEnv();
	virtual bool UninitShowEnv();
	void CopyBGImage(PBITMAP hBGbitmap);
private:

	BITMAPINFO*     m_pBmpInfo;//BmpInfo÷∏’Î
	char*           m_pRGBBuf;
	unsigned int    m_nRGBSize;
	int				m_nBGPicWidth ;
	int				m_nBGPicHeight;
	HShowWindow		m_show_rect;
	CMutexLock      m_drawBGLock;
	RECT            m_showBGRect;
	unsigned char* m_pRGBBackBuf;
	unsigned int   m_nRGBBackSize;
};

#endif
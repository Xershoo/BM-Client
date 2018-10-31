#ifndef SHOWVIDEOBYDX_H
#define SHOWVIDEOBYDX_H
#include "ShowVideo.h"
#include "PlayHeader.h"
#include "ShowBGByGDI.h"
#include "MediaDecode.h"

#include "ddraw.h"
#include "sc_CSLock.h"

class CShowVideoByDX : public CShowVideo
{
public:
	CShowVideoByDX(PBITMAP hBGbitmap,HWND hShowWnd);
	virtual ~CShowVideoByDX();
public:
	virtual bool ShowVideo(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFlie = false,IMediaPlayerEvent* pShowCallback = NULL);
	virtual bool DrawBGImage();
private:
	virtual bool InitShowEnv();
	virtual bool UninitShowEnv();
	void get_show_window_width_and_height();
	bool getShowHwndStatus();

	//bool DrawYUVImage(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFlie = false);


	bool InitSurface();
	void InitConvertTable();
	bool CutRGBFromYUV420(unsigned char *yuv420,int nWidth,int nHeight);
	bool CutBGRFromYUV420(unsigned char *yuv420,int nWidth,int nHeight);
private:
	LPDIRECTDRAW7           m_lpDD;    // DirectDraw 对象指针
	LPDIRECTDRAWSURFACE7    m_lpDDSPrimary;  // DirectDraw 主表面指针
	LPDIRECTDRAWSURFACE7    m_lpDDSOffScr;  // DirectDraw 离屏表面指针
	DDSURFACEDESC2          m_ddsd;    // DirectDraw 表面描述
	HShowWindow				m_show_rect;
	int				m_video_width;
	int				m_video_height;
	int				m_show_window_width;
	int				m_show_window_height;

	RECT            m_showBGRect;
private:
	CShowBGByGDI*    m_gdiShowBG;
	bool             m_bIsStatus;

private:
	unsigned char* m_pRGBBuf;
	unsigned int   m_nRGBSize;
	unsigned char* m_pRGBBackBuf;
	unsigned int   m_nRGBBackSize;
	SwsContext*    m_sws_context;

	bool		m_bIsInitYUVEnv;
	int         m_nErrCountNum;

	CMutexLock	m_lockRGBBuf;

private:
		//用于YUV转RGB数组
	long int crv_tab[256];
	long int cbu_tab[256];
	long int cgu_tab[256];
	long int cgv_tab[256];
	long int tab_76309[256];
	unsigned char clp[1024];   //for clip in CCIR601
	bool   m_bFirst;
	bool   m_bYUVSucess;
};

#endif
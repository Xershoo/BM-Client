#pragma once
#include "afxwin.h"
#include "SourData.h"

class CSourScreen:ISourData
{
public:
	CSourScreen(HWND hShowWindow,string sname);
	virtual ~CSourScreen();
public:
	virtual bool GetBuffer(unsigned char** pBuf,unsigned int& nBufSize);

	virtual void GetVideoWAndH(int& nW,int& nH);

	virtual long GetVideoFPS();

	virtual bool SetSourType(int nSourType);

	virtual bool SourOpen(void* param) ;

	virtual bool SourClose(bool bIsColseDev);

	virtual bool AVE_SetID(int nMainValue, int nSubValue, int nParams[2]);

	inline bool GetScreenCaptureIsFullScreen() {return m_bIsFullScreen;}

	virtual void ShowImage(unsigned char* pBuf,unsigned int nBufSize,int nVW,int nVH);

	virtual void GetShowHwndRect(ScRect& rc);

	virtual HWND GetShowHand() {return m_hShowWindow;}

	int  getSourID() ;
private:
	void ShowImage(unsigned char* pBuf);

	void ScreenCapture();



private:
	int    m_iSrcVideoWidth;
	int    m_iSrcVideoHeight;
	bool    m_bIsEffective;

	HBITMAP	m_hDesktopCompatibleBitmap;
	HDC		m_hDesktopCompatibleDC;
	HBITMAP	m_hDesktopCompatibleBitmap2;
	HDC		m_hDesktopCompatibleDC2;
	HDC		m_hDesktopDC;
	HWND    m_hDesktopWnd;
	void*   m_pBits;
	unsigned char* m_pBuffer;
	HWND	m_hShowWindow;
	BITMAPINFO* m_pBmpInfo;//BmpInfo÷∏’Î
	bool     m_bIsOpen;
	bool     m_bIsFullScreen;
	int    m_nSourID;
};
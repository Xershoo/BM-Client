#pragma once

#include "MPdfApp.h"
#include <map>

class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION * pCrist)
	{
		m_pCriSt = pCrist;
		if(m_pCriSt) {
			::EnterCriticalSection(m_pCriSt);
		};
	}

	virtual ~CAutoLock()
	{
		if(m_pCriSt) {
			::LeaveCriticalSection(m_pCriSt);
		};
	}
protected:
	CRITICAL_SECTION * m_pCriSt;
};

class CPdfShowMgr;
class CPdfShow
{
public:
	CPdfShow();
	virtual ~CPdfShow();

public:
	BOOL OpenFile(LPCWSTR pwszFileName,HWND hParent,RECT rc,LPARAM lParam=NULL,BOOL bCtrl=TRUE);
	void CloseFile();

	void ZoomIn(BOOL bPostMsg = FALSE);
	void ZoomOut(BOOL bPostMsg = FALSE);
	void PrevPage(BOOL bPostMsg = FALSE);
	void NextPage(BOOL bPostMsg = FALSE);

	void Show(BOOL bShow);
protected:
	void SetToolbarTrackMouse();
	void RedrawToolbar(BYTE alpha);
	void ResetToolbarPos();

	void CreateGDIObject();
	void DeleteGDIObject();

	BOOL CreateShowWnd(HWND hParent,RECT rc);
protected:
	static LRESULT WINAPI ToolbarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI PdfShowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT WINAPI doToolbarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WINAPI doPdfShowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


protected:
	PDFAPP	m_AppPDF;
	HWND	m_hWnd;
	HWND	m_hToolbar;
	BOOL	m_bOpen;
	BOOL	m_bTBTrackMouse;
	BYTE	m_nTBAlpha;
	HFONT	m_hTBFont;
	HFONT	m_hWndTextFont;
	HBRUSH  m_hBrushBack;
	HCURSOR	m_hWaitCursor;
	HCURSOR	m_hHandCursor;
	HCURSOR	m_hArrowCursor;
	HWND	m_hParent;

	RECT	m_rcNormal;
	LPARAM	m_lParam;
	BOOL	m_bCtrl;
	friend class CPdfShowMgr;
};

using namespace std;
typedef map<int,CPdfShow*>  MapPDFShow;

class CPdfShowMgr
{
public:
	CPdfShowMgr();
	virtual ~CPdfShowMgr();

public:
	static CPdfShowMgr* GetInstance();

	void	Initialize();
	void	Uninitialize();

	int		OpenPDFFile(WCHAR* wszFileName,HWND hParent,RECT rc,LPARAM lParam=NULL,BOOL bCtrl=TRUE);
	void	ClosePDFFile(int nId);

	void	PdfZoomIn(int nId);
	void	PdfZoomOut(int nId);
	void	PdfPrevPage(int nId);
	void	PdfNextPage(int nId);
	void	PdfShow(int nId,BOOL bShow);
	int		GetPdfShowCount();
protected:
	void	RegisterToolbarWndClass();
	void	UnRegisterToolbarWndClass();

	void	RegisterShowWndClass();
	void	UnRegisterShowWndClass();

	void	FreeAllPdfShow();

protected:
	CRITICAL_SECTION	m_mapLock;
	MapPDFShow			m_mapPDFShow;
};


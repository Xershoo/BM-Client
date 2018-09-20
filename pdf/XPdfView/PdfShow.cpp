#include "stdafx.h"
#include "PdfShow.h"
#include "Resource.h"
#include "../../../UI/eduApp/Curseware/Cursewave.h"

#define MIN_ALPHA	(10)
#define MAX_ALPHA	(224)

#define ID_PDF_SHOW_WND		(64001)

enum
{
	TOOLBAR_TIMER_DELAY_TRANSPARENT = 0,
	TOOLBAR_TIMER_TRANSPARENT,
};

extern HINSTANCE g_hInstance;

const WCHAR PROP_PDF_WND_PROC[] = L"XPdf Viewer Window Proc";
const WCHAR VIEW_WND_NAME[] = L"XPdf Viewer 2014";
const WCHAR TOOLBAR_WND_CLASS[] = L"XPdf Viewer Toolbar";
const WCHAR PDF_SHOW_WND_CLASS[] = L"XPdf Viewer Window";

//////////////////////////////////////////////////////////////////////////
///
///	2014.12.18  XieWenBing 
///	PDF File Show Class
///
CPdfShow::CPdfShow():m_hWnd(NULL),
	m_hToolbar(NULL),
	m_bOpen(FALSE),
	m_bTBTrackMouse(FALSE),
	m_nTBAlpha(MAX_ALPHA),
	m_hTBFont(NULL),
	m_hWndTextFont(NULL),
	m_hBrushBack(NULL),
	m_hWaitCursor(NULL),
	m_hHandCursor(NULL),
	m_hArrowCursor(NULL),
	m_hParent(NULL),
	m_lParam(NULL),
	m_bCtrl(TRUE)
{
	memset(&m_rcNormal,0,sizeof(RECT));
	CreateGDIObject();
}

CPdfShow::~CPdfShow()
{
	DeleteGDIObject();
}

BOOL CPdfShow::CreateShowWnd(HWND hParent,RECT rc)
{
	int nId = CPdfShowMgr::GetInstance()->GetPdfShowCount()+ ID_PDF_SHOW_WND;

	m_hWnd = CreateWindowExW(WS_EX_TOPMOST,PDF_SHOW_WND_CLASS,L"",WS_CHILD|WS_VISIBLE,rc.left,rc.top,
		rc.right - rc.left,rc.bottom-rc.top,hParent,(HMENU)nId,g_hInstance,this);

	if(NULL==m_hWnd)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	m_hParent = hParent;
	m_rcNormal = rc;
	::SetPropW(m_hWnd,PROP_PDF_WND_PROC,(HANDLE)this);
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

BOOL CPdfShow::OpenFile(LPCWSTR pwszFileName,HWND hParent,RECT rc,LPARAM lParam/*=NULL*/,BOOL bCtrl/*=TRUE*/)
{
	if(m_bOpen)
	{
		return FALSE;
	}

	if(!::IsWindow(hParent))
	{
		return FALSE;
	}

	if(NULL==pwszFileName||NULL==pwszFileName[0])
	{
		return FALSE;
	}

	do 
	{
		if(!CreateShowWnd(hParent,rc))
		{
			break;
		};
		
		if(!pdfapp_init(&m_AppPDF,NULL))
		{
			break;
		};
		
		RECT rcScr = { 0 };
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScr, 0);
		m_AppPDF.scrw = rcScr.right - rcScr.left;
		m_AppPDF.scrh = rcScr.bottom - rcScr.top;

		m_AppPDF.hwnd = m_hWnd;
		
		if(!pdfapp_open_with_file_w(&m_AppPDF,(wchar_t*)pwszFileName,NULL,0))
		{
			break;
		}

		//tool bar
		m_hToolbar = CreateWindowEx(WS_EX_TOPMOST,TOOLBAR_WND_CLASS, NULL, WS_CHILD,
			0, 0, CW_USEDEFAULT, CW_USEDEFAULT,m_hWnd, NULL, g_hInstance, NULL);
		if(NULL==m_hToolbar)
		{
			break;
		}

		::SetPropW(m_hToolbar,PROP_PDF_WND_PROC,(HANDLE)this);

		SetWindowRgn(m_hToolbar,BitmapToRegion(IDB_TOOLBAR_BACK,RGB(0x70,0x70,0x70),0),TRUE);
		SetToolbarTrackMouse();
		ResetToolbarPos();
		pdfapp_onresize(&m_AppPDF, rc.right - rc.left, rc.bottom - rc.top);
				
		::SetCursor(m_hArrowCursor);

		m_bCtrl = bCtrl;

		m_lParam = lParam;
		m_bOpen = TRUE;
	} while (FALSE);

	return m_bOpen;
}

void CPdfShow::CloseFile()
{
	if(!m_bOpen)
	{
		return;
	}

	pdfapp_close(&m_AppPDF);
	
	if(m_hToolbar && ::IsWindow(m_hToolbar))
	{
		::DestroyWindow(m_hToolbar);
	}

	if(m_hWnd && ::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd,SW_HIDE);
		::DestroyWindow(m_hWnd);
	}

	m_hWnd = NULL;
	m_bOpen = FALSE;
	m_hParent = NULL;
	m_hToolbar = NULL;
	m_lParam = NULL;
	m_bCtrl = TRUE;
	memset(&m_rcNormal,0,sizeof(RECT));
}

void CPdfShow::Show(BOOL bShow)
{
	if(bShow)
	{
		::ShowWindow(m_hWnd,SW_SHOW);
	}
	else
	{
		::ShowWindow(m_hWnd,SW_HIDE);
	}
}
void CPdfShow::ZoomIn(BOOL bPostMsg /* = FALSE */)
{
	if(!m_bOpen)
	{
		return;
	}
	
	BOOL br = pdfapp_zoomin(&m_AppPDF);

	if(bPostMsg && br)
	{
		::PostMessage(m_hParent,WM_CURSEWAVE_CTRL,CTRL_ZOOMIN_PAGE,m_lParam);
	}
}

void CPdfShow::ZoomOut(BOOL bPostMsg /* = FALSE */)
{
	if(!m_bOpen)
	{
		return;
	}

	BOOL br = pdfapp_zoomout(&m_AppPDF);

	if(bPostMsg && br)
	{
		::PostMessage(m_hParent,WM_CURSEWAVE_CTRL,CTRL_ZOOMOUT_PAGE,m_lParam);
	}
}

void CPdfShow::PrevPage(BOOL bPostMsg /* = FALSE */)
{
	if(!m_bOpen)
	{
		return;
	}

	BOOL br = pdfapp_prevpage(&m_AppPDF);

	if(bPostMsg && br)
	{
		::PostMessage(m_hParent,WM_CURSEWAVE_CTRL,CTRL_PREV_PAGE,m_lParam);
	}
}

void CPdfShow::NextPage(BOOL bPostMsg /* = FALSE */)
{
	if(!m_bOpen)
	{
		return;
	}

	BOOL br = pdfapp_nextpage(&m_AppPDF);

	if(bPostMsg && br)
	{
		::PostMessage(m_hParent,WM_CURSEWAVE_CTRL,CTRL_NEXT_PAGE,m_lParam);
	}
}

void CPdfShow::SetToolbarTrackMouse()
{
	if(m_bTBTrackMouse) {
		return;
	};

	POINT	pt = { 0 };
	RECT	rt = { 0 };
	TRACKMOUSEEVENT tme = { 0 };

	GetCursorPos(&pt);
	GetWindowRect(m_hToolbar,&rt);
	
	if(!PtInRect(&rt,pt)) 
	{
		return;
	}
		
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.hwndTrack = m_hToolbar;
	tme.dwHoverTime = 10;
	tme.dwFlags = TME_HOVER | TME_LEAVE;

	m_bTBTrackMouse = ::TrackMouseEvent(&tme);
}

void CPdfShow::RedrawToolbar(BYTE alpha)
{
	if(m_nTBAlpha == alpha)
	{
		return;
	}

	m_nTBAlpha = alpha;

	POINT pt = { 0 };
	RECT rt = { 0 };
	GetWindowRect(m_hToolbar,&rt);

	pt.x = rt.left;
	pt.y = rt.top;

	ScreenToClient(m_hWnd,(LPPOINT)&pt);
	OffsetRect(&rt,pt.x-rt.left,pt.y-rt.top);
	
	InvalidateRect(m_hWnd,&rt,FALSE);
}

void CPdfShow::ResetToolbarPos()
{
	if(!::IsWindow(m_hToolbar))
	{
		return;
	};

	RECT rcFrame = { 0 };
	UINT nWidth = 0;
	UINT nHeight = 0;
	POINT pos = { 0 };
	BITMAP	bmp = { 0 };
	HBITMAP hBmp = ::LoadBitmap(g_hInstance,MAKEINTRESOURCE(IDB_TOOLBAR_BACK));
	if(NULL==hBmp)
	{
		return;
	};

	GetObject(hBmp,sizeof(BITMAP),&bmp);
	GetClientRect(m_hWnd,&rcFrame);
	nWidth = rcFrame.right - rcFrame.left;
	nHeight = rcFrame.bottom - rcFrame.top;

	pos.x = (nWidth - bmp.bmWidth)/2 ;
	pos.y = nHeight - bmp.bmHeight - 20;

	::SetWindowPos(m_hToolbar,NULL,pos.x,pos.y,bmp.bmWidth,bmp.bmHeight,SWP_NOZORDER);
}

LRESULT WINAPI CPdfShow::ToolbarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPdfShow* pShow = (CPdfShow*)::GetPropW(hWnd,PROP_PDF_WND_PROC);
	if(NULL != pShow)
	{
		return pShow->doToolbarWndProc(hWnd, message,wParam,lParam);
	}

	return ::DefWindowProc(hWnd,message,wParam,lParam);
}

LRESULT WINAPI CPdfShow::PdfShowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPdfShow* pShow = (CPdfShow*)::GetPropW(hWnd,PROP_PDF_WND_PROC);
	if(NULL != pShow)
	{
		return pShow->doPdfShowWndProc(hWnd, message,wParam,lParam);
	}

	return ::DefWindowProc(hWnd,message,wParam,lParam);
}

LRESULT WINAPI CPdfShow::doPdfShowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!::IsWindow(hWnd))
	{
		return ::DefWindowProc(hWnd,message,wParam,lParam);
	}		
	
	switch(message)
	{
	case WM_LBUTTONDOWN:
		{
			::SetFocus(hWnd);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			::SetFocus(hWnd);
			
		}
		break;
	case WM_RBUTTONDOWN:
		{
			::SetFocus(hWnd);
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			RECT rcPaint = { 0 };
			RECT rcText = { 0 };
			HFONT hOldFont = NULL;
			HDC hdc = BeginPaint(hWnd, &ps);
			
			// TODO: 在此添加任意绘图代码...
			GetClientRect(hWnd,&rcPaint);

			FillRect(hdc,&rcPaint,m_hBrushBack);
			/*
			hOldFont = (HFONT)SelectObject(hdc,m_hWndTextFont);
			SetBkMode(hdc,TRANSPARENT);
			rcText = rcPaint;
			SetTextColor(hdc,RGB(0x40,0x40,0x40));
			DrawText(hdc,VIEW_WND_NAME,lstrlen(VIEW_WND_NAME),&rcText,DT_SINGLELINE| DT_VCENTER | DT_CENTER); 

			rcText.top -= 1;
			rcText.left -= 2;
			rcText.bottom -= 1;
			rcText.right -= 2;

			SetTextColor(hdc,RGB(0xf0,0xf0,0xf0));
			DrawText(hdc,VIEW_WND_NAME,lstrlen(VIEW_WND_NAME),&rcText,DT_SINGLELINE| DT_VCENTER | DT_CENTER);

			SelectObject(hdc,hOldFont);
			*/
			if(m_bOpen)
			{
				pdfapp_show(&m_AppPDF,hdc);
			}

			EndPaint(hWnd, &ps);
			
			return 0;
		}

	case WM_SIZING:
		{
			m_AppPDF.shrinkwrap = 0;

			ResetToolbarPos();
			break;
		}
	case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				break;
			};

			if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPDF.shrinkwrap = 0;
			}

			pdfapp_onresize(&m_AppPDF, LOWORD(lParam), HIWORD(lParam));

			ResetToolbarPos();
			break;
		}      

	case WM_MOUSEMOVE:
		{	
			break;
		}

	case WM_SYSCOMMAND:		
		{
			WORD nId    = LOWORD(wParam); 
			WORD nEvent = HIWORD(wParam); 
			// 分析菜单选择:
			switch (nId)
			{
			
			case SC_MAXIMIZE:
				{
					m_AppPDF.shrinkwrap = 0;					
				}

				break;
			default:
				break;
			}
			
		}

	case WM_PDFAPP_RESIZE:
		{
			break;
		}
	case WM_PDFAPP_SET_CURSOR:
		{
			UINT nCursor = (UINT)lParam;
			switch(nCursor)
			{
			case CURSOR_WAIT:
				SetCursor(m_hWaitCursor);
				break;

			case CURSOR_HAND:
				SetCursor(m_hHandCursor);
				break;

			case CURSOR_ARROW:
				SetCursor(m_hArrowCursor);
				break;
			}

			break;
		}
	case WM_MOUSEWHEEL:
		{
			if(m_bCtrl)
			{
				int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
				if(zDelta<0)
				{
					ZoomOut(TRUE);
				}
				else
				{
					ZoomIn(TRUE);
				}
			}
		}
		break;
	case WM_KEYDOWN:		
		{
			UINT nKey = (UINT)wParam;
			switch(nKey)
			{
			case VK_LEFT:
				if(m_bCtrl)
				{
					PrevPage(TRUE);
				}
				break;
			case VK_RIGHT:
				if(m_bCtrl)
				{
					NextPage(TRUE);
				}
				break;

			case VK_UP:
				if(m_bCtrl)
				{
					PrevPage(TRUE);
				}
				break;
			case VK_DOWN:
				if(m_bCtrl)
				{
					NextPage(TRUE);
				}
				break;
			case VK_ESCAPE:
				{
				}
				break;
			}
		}
		::SetFocus(hWnd);

		break;
	default:
		break;
	};

	return ::DefWindowProc(hWnd,message,wParam,lParam);
}

LRESULT WINAPI CPdfShow::doToolbarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!::IsWindow(hWnd))
	{
		return ::DefWindowProc(hWnd,message,wParam,lParam);
	}

	switch(message)
	{
	case WM_COMMAND:
		{
			break;
		}

	case WM_ERASEBKGND:
		{
			return TRUE;
		}

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			// draw background
			HDC hMemDC = CreateCompatibleDC(hdc);
			HBITMAP hBmp = LoadBitmap(g_hInstance,MAKEINTRESOURCE(IDB_TOOLBAR_BACK));
			BITMAP bmp = { 0 };
			GetObject(hBmp,sizeof(BITMAP),&bmp);
			HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC,hBmp);

			//draw text
			WCHAR	wszText[32] = { 0 };
			RECT	rtText = { 0,0,bmp.bmWidth,bmp.bmHeight };
			swprintf(wszText,L"%d/%d",m_AppPDF.pageno,m_AppPDF.pagecount);
			SetBkMode(hMemDC,TRANSPARENT);
			SetTextColor(hMemDC,RGB(224,224,224));
			SelectObject(hMemDC,m_hTBFont);

			DrawTextW(hMemDC,wszText,wcslen(wszText),&rtText,DT_CENTER | DT_VCENTER |DT_SINGLELINE);

			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = m_nTBAlpha; 
			bf.AlphaFormat = 0;

			AlphaBlend(hdc,0,0,bmp.bmWidth,bmp.bmHeight,hMemDC,1,1,bmp.bmWidth-2,bmp.bmHeight-2,bf);			
			SelectObject(hMemDC,hOldBmp);

			SAFE_DELETE_OBJECT(hBmp);
			DeleteDC(hMemDC);		

			EndPaint(hWnd, &ps);

			break;
		}
	case WM_LBUTTONDOWN:
		{
			RECT rtZoomIn = {268,12,292,36};
			RECT rtZoomOut = {236,12,260,36};
			RECT rtPrevPage = {78,12,102,36};
			RECT rtNextPage = {110,12,134,36};

			POINT pt = { 0 };
			GetCursorPos(&pt);
			ScreenToClient(m_hToolbar,&pt);

			if(::PtInRect(&rtZoomIn,pt)) {
				ZoomIn(TRUE);
				break;
			};

			if(::PtInRect(&rtZoomOut,pt)) {
				ZoomOut(TRUE);
				break;
			};
			if(::PtInRect(&rtPrevPage,pt)) {
				PrevPage(TRUE);
				break;
			};

			if(::PtInRect(&rtNextPage,pt)) {
				NextPage(TRUE);
				break;
			};

			break;
		}

	case WM_LBUTTONUP:
		{
			break;
		}
	case WM_MOUSEMOVE:
		{
			if(!m_bTBTrackMouse)
			{
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.hwndTrack = hWnd;
				tme.dwHoverTime = 10;
				tme.dwFlags = TME_HOVER | TME_LEAVE;

				::TrackMouseEvent(&tme);

				m_bTBTrackMouse = TRUE;
			}

			break;
		}

	case WM_MOUSEHOVER:
		{
			if(!IsWindowEnabled(hWnd))
			{
				break;
			}

			KillTimer(hWnd,TOOLBAR_TIMER_DELAY_TRANSPARENT);
			KillTimer(hWnd,TOOLBAR_TIMER_TRANSPARENT);

			RedrawToolbar(MAX_ALPHA);			

			break;
		}

	case WM_MOUSELEAVE:
		{			
			m_bTBTrackMouse = FALSE;

			if(!IsWindowEnabled(hWnd))
			{
				break;
			}

			SetTimer(hWnd,TOOLBAR_TIMER_DELAY_TRANSPARENT,5000,NULL);
			break;
		}

	case WM_TIMER:
		{
			UINT nEvent = (UINT)wParam;
			switch(nEvent)
			{
			case TOOLBAR_TIMER_DELAY_TRANSPARENT:
				{
					SetTimer(hWnd,TOOLBAR_TIMER_TRANSPARENT,40,NULL);					
					KillTimer(hWnd,nEvent);
					break;
				}

			case TOOLBAR_TIMER_TRANSPARENT:
				{					
					BYTE ap = m_nTBAlpha > 20 ? m_nTBAlpha - 10 : MIN_ALPHA;

					RedrawToolbar(ap);

					if(ap == MIN_ALPHA)
					{							
						KillTimer(hWnd,nEvent);						
					}
				}

				break;
			}	

			break;
		}
	default:
		{
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}		
	};

	return 0;
}

void CPdfShow::CreateGDIObject()
{
	m_hWndTextFont = ::CreateFontW(48,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_CHARACTER_PRECIS,
		DRAFT_QUALITY,
		DEFAULT_PITCH,
		L"楷体"
		);

	m_hTBFont = ::CreateFontW(14,0,0,0,
		FW_NORMAL,0,0,0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_CHARACTER_PRECIS,
		DRAFT_QUALITY,
		DEFAULT_PITCH,
		L"宋体"
		);

	m_hArrowCursor = LoadCursor(NULL, IDC_ARROW);
	m_hHandCursor = LoadCursor(NULL, IDC_HAND);
	m_hWaitCursor = LoadCursor(NULL, IDC_WAIT);

	m_hBrushBack = CreateSolidBrush(RGB(0x70,0x70,0x70));
}

void CPdfShow::DeleteGDIObject()
{
	SAFE_DELETE_OBJECT(m_hTBFont);

	SAFE_DELETE_OBJECT(m_hArrowCursor);
	SAFE_DELETE_OBJECT(m_hHandCursor);
	SAFE_DELETE_OBJECT(m_hWaitCursor);
	SAFE_DELETE_OBJECT(m_hBrushBack);
}

//////////////////////////////////////////////////////////////////////////
///
///	2014.12.18  XieWenBing 
///	PDF File Show Class Manager
///
static CPdfShowMgr m_mgrPdfShow;

CPdfShowMgr::CPdfShowMgr()
{
	
}

CPdfShowMgr::~CPdfShowMgr()
{
	
}

void CPdfShowMgr::Initialize()
{
	::InitializeCriticalSectionAndSpinCount(&m_mapLock,4000);
	RegisterToolbarWndClass();
	RegisterShowWndClass();
}

void CPdfShowMgr::Uninitialize()
{
	FreeAllPdfShow();
	::DeleteCriticalSection(&m_mapLock);
	UnRegisterToolbarWndClass();
	UnRegisterShowWndClass();
}

void CPdfShowMgr::RegisterShowWndClass()
{
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
	wcex.lpfnWndProc = CPdfShow::PdfShowWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = PDF_SHOW_WND_CLASS;

	ATOM ir = RegisterClassEx(&wcex);

	if(ir == 0)
	{
		DWORD err = GetLastError();
	}

	return;
}

void CPdfShowMgr::UnRegisterShowWndClass()
{
	UnregisterClassW(PDF_SHOW_WND_CLASS,g_hInstance);
}

void CPdfShowMgr::RegisterToolbarWndClass()
{
	INITCOMMONCONTROLSEX icex;
	WNDCLASSEX wcex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style = CS_HREDRAW | CS_VREDRAW ;
	wcex.lpfnWndProc = CPdfShow::ToolbarWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TOOLBAR_WND_CLASS;
	
	ATOM ir = RegisterClassEx(&wcex);

	if(ir == 0)
	{
		DWORD err = GetLastError();
	}

	return;
}

void CPdfShowMgr::UnRegisterToolbarWndClass()
{
	UnregisterClassW(TOOLBAR_WND_CLASS,g_hInstance);
}

void CPdfShowMgr::FreeAllPdfShow()
{
	if(m_mapPDFShow.empty())
	{
		return;
	}

	CAutoLock autoLock(&m_mapLock);

	for(MapPDFShow::iterator itr = m_mapPDFShow.begin();
		itr!=m_mapPDFShow.end();itr++)
	{
		CPdfShow* pShow = itr->second;
		if(NULL != pShow)
		{
			pShow->CloseFile();
			delete pShow;
			pShow = NULL;
		};
	}

	m_mapPDFShow.clear();
}

int	CPdfShowMgr::OpenPDFFile(WCHAR* wszFileName,
	HWND hParent,
	RECT rc,
	LPARAM lParam/* =NULL */,
	BOOL bCtrl/* =TRUE */)
{
	CPdfShow * pShow = new CPdfShow;
	if(NULL == pShow)
	{
		return -1;
	}

	if(!pShow->OpenFile(wszFileName,hParent,rc,lParam,bCtrl))
	{
		delete pShow;
		return -1;
	}

	CAutoLock autoLock(&m_mapLock);
	int nIndex = (int)m_mapPDFShow.size();

	m_mapPDFShow.insert(MapPDFShow::_Val_type(nIndex,pShow));
	return nIndex;
}

void CPdfShowMgr::ClosePDFFile(int nId)
{
	CAutoLock autoLock(&m_mapLock);
	MapPDFShow::iterator itr = m_mapPDFShow.find(nId);
	if(itr == m_mapPDFShow.end())
	{
		return ;
	}

	CPdfShow* pShow = itr->second;
	if(NULL != pShow)
	{
		pShow->CloseFile();
		delete pShow;
		pShow = NULL;
	};

	m_mapPDFShow.erase(itr);
}

void CPdfShowMgr::PdfZoomIn(int nId)
{
	CAutoLock autoLock(&m_mapLock);
	MapPDFShow::iterator itr = m_mapPDFShow.find(nId);
	if(itr == m_mapPDFShow.end())
	{
		return ;
	}

	CPdfShow* pShow = itr->second;
	if(NULL != pShow)
	{
		pShow->ZoomIn();
	};
}

void CPdfShowMgr::PdfZoomOut(int nId)
{
	CAutoLock autoLock(&m_mapLock);
	MapPDFShow::iterator itr = m_mapPDFShow.find(nId);
	if(itr == m_mapPDFShow.end())
	{
		return ;
	}

	CPdfShow* pShow = itr->second;
	if(NULL != pShow)
	{
		pShow->ZoomOut();		
	};
}

void CPdfShowMgr::PdfPrevPage(int nId)
{
	CAutoLock autoLock(&m_mapLock);
	MapPDFShow::iterator itr = m_mapPDFShow.find(nId);
	if(itr == m_mapPDFShow.end())
	{
		return ;
	}

	CPdfShow* pShow = itr->second;
	if(NULL != pShow)
	{
		pShow->PrevPage();	
	};
}

void CPdfShowMgr::PdfNextPage(int nId)
{
	CAutoLock autoLock(&m_mapLock);
	MapPDFShow::iterator itr = m_mapPDFShow.find(nId);
	if(itr == m_mapPDFShow.end())
	{
		return ;
	}

	CPdfShow* pShow = itr->second;
	if(NULL != pShow)
	{
		pShow->NextPage();
	};
}

void CPdfShowMgr::PdfShow(int nId,BOOL bShow)
{
	CAutoLock autoLock(&m_mapLock);
	MapPDFShow::iterator itr = m_mapPDFShow.find(nId);
	if(itr == m_mapPDFShow.end())
	{
		return ;
	}

	CPdfShow* pShow = itr->second;
	if(NULL != pShow)
	{
		pShow->Show(bShow);
	};
}

CPdfShowMgr* CPdfShowMgr::GetInstance()
{
	return &m_mgrPdfShow;
}

int CPdfShowMgr::GetPdfShowCount()
{
	return m_mapPDFShow.size();
}

#include "stdafx.h"
#include "updatedlg.h"
#include "xlayout.h"
#include "ClUpdater.h"
#include <atlstr.h>
#include "HiddenWnd.h"
#include "common/lang.h"

const int nUpdataProcess = 100;
const int nUpdataProcessLen = 1000;

CLUpdateDlg::CLUpdateDlg()
{
	m_nTotalPos = 0;
	m_nNowPos = 0;
	m_hPwnBrush = NULL;
}

CLUpdateDlg::~CLUpdateDlg()
{

}

LRESULT CLUpdateDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{	
	LoadEncryptLayout(TEXT("update.gt"));

	if (xgdi::IFont* pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont)))  
	{ 
		m_fontDefault = pFont->GetUIHandle();
	}   

	SetResLoader(xskin::CResLoader::inst());

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	__super::OnCreate(uMsg, wParam, lParam, bHandle);

	if(CMessageLoop* pLoop = _Module.GetMessageLoop())
	{
		pLoop->AddMessageFilter(this); 
	}

	CreateLayoutElement();
	SetTimer(nUpdataProcess, nUpdataProcessLen);
	return 0;
}

void CLUpdateDlg::CreateLayoutElement()
{
    WCHAR wszTitle[MAX_PATH] = { 0 };
    WCHAR wszText[MAX_PATH] = { 0 };
    WCHAR wszBtnTxt[MAX_PATH] = { 0 };

    char szProduct[MAX_PATH] = {0};
    ::LoadStringA(NULL,IDS_PRODUCT,szProduct,MAX_PATH);
    if(LangSet::loadLangSet("szProduct") == LangSet::LANG_ENG)
    {
        ::LoadStringW(NULL,IDS_UPDATE_DLG_TITLE_EN,wszTitle,MAX_PATH);
        ::LoadStringW(NULL,IDS_UPDATING_EN,wszText,MAX_PATH);
        ::LoadStringW(NULL,IDS_START_CLIENT,wszBtnTxt,MAX_PATH);
    }
    else
    {
        ::LoadStringW(NULL,IDS_UPDATE_DLG_TITLE,wszTitle,MAX_PATH);
        ::LoadStringW(NULL,IDS_UPDATING,wszText,MAX_PATH);
        ::LoadStringW(NULL,IDS_START_CLIENT,wszBtnTxt,MAX_PATH);
    }
    
	XLAYOUT_CREATE_EX(TxtTitel, wszTitle, WS_DISABLED);
	XLAYOUT_CREATE(TxtUpdateInfo, wszText);

	XLAYOUT_CREATE(BtnMin, NULL);
	XLAYOUT_CREATE(BtnClose, NULL);
	XLAYOUT_CREATE(TxtPos, L"0%");
	XLAYOUT_CREATE_EX(SldProgress, NULL, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    XLAYOUT_CREATE(BtnOK, wszBtnTxt);

	XLAYOUT_ELEMENT(BtnOK).ShowWindow(SW_HIDE);
}


LRESULT CLUpdateDlg::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnPaint(uMsg, wParam, lParam, bHandled);
	return 0;
}

LRESULT CLUpdateDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if ( CMessageLoop *pLoop = _Module.GetMessageLoop())
	{
		pLoop->RemoveMessageFilter(this);
	}
	::DeleteObject(m_hPwnBrush);
	return 0;
}

LRESULT CLUpdateDlg::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if ( wParam == HTCLIENT )
		SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
	return __super::OnLButtonDown(uMsg, wParam, lParam, bHandle);
}

LRESULT CLUpdateDlg::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if (wParam == HTCLIENT)
		SendMessage(WM_NCLBUTTONUP, HTCAPTION, lParam);
	return __super::OnLButtonUp(uMsg, wParam, lParam, bHandle);
}

LRESULT CLUpdateDlg::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if (wParam == HTCLIENT)
		SendMessage(WM_MOUSEMOVE, HTCAPTION, lParam);
	return __super::OnMouseMove(uMsg, wParam, lParam, bHandle);
}

LRESULT CLUpdateDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if (wParam == SC_CLOSE)
	{
		DoClose(0, 0, 0, bHandle);
		return 0;
	}
	bHandle = FALSE;
	return 0;
}

LRESULT CLUpdateDlg::OnSliProgressPos(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{	
	return 0;
}

LRESULT CLUpdateDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	if(wParam == nUpdataProcess)
	{	
		int fPos = CLUpdater::GetInstance()->GetCompletePercent();		
		XLAYOUT_ELEMENT(SldProgress).SetPos(fPos);
		XLAYOUT_ELEMENT(SldProgress).Invalidate();
		ATL::CString str;
		int nComplete = fPos;
		if(nComplete >= 100)
		{
			fPos = 100;
			XLAYOUT_ELEMENT(BtnOK).ShowWindow(SW_SHOW);
			XLAYOUT_ELEMENT(TxtPos).ShowWindow(SW_HIDE);

            WCHAR wszText[MAX_PATH] = { 0 };
            char szProduct[MAX_PATH] = {0};
            ::LoadStringA(NULL,IDS_PRODUCT,szProduct,MAX_PATH);
            if(LangSet::loadLangSet("szProduct") == LangSet::LANG_ENG)
            {
                ::LoadStringW(NULL,IDS_UPDATE_FINISHED_EN,wszText,MAX_PATH);
            }
            else
            {
                ::LoadStringW(NULL,IDS_UPDATE_FINISHED,wszText,MAX_PATH);
            }

            
			XLAYOUT_ELEMENT(TxtUpdateInfo).SetWindowText(wszText);
			return 0;
		}
		str.Format(L"%d%%", fPos);
		XLAYOUT_ELEMENT(TxtPos).SetWindowText(str);
	}
	return 0;
}

LRESULT CLUpdateDlg::OnNcLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	return 0;
}

LRESULT CLUpdateDlg::DoMin(WORD, WORD, HWND, BOOL&)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CLUpdateDlg::DoClose(WORD, WORD, HWND, BOOL&)
{	
	ShowWindow(SW_HIDE);
	return 0;
}

LRESULT CLUpdateDlg::DoOk(WORD, WORD, HWND, BOOL&)
{	
    m_HiddenWnd.ExeClass8();

	PostMessage(WM_QUIT);
	DestroyWindow();
	return 0;
}
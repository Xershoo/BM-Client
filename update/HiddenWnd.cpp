// TestView.cpp : implementation of the CTestView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "HiddenWnd.h"
#include "ClUpdater.h"
#include "CLFile.h"
#include "updatedlg.h"

CHiddenWnd m_HiddenWnd;

typedef int  (WINAPI *PFN_CheckUpdata)(wchar_t*, wchar_t* );
LRESULT CHiddenWnd::OnCreate(UINT, WPARAM, LPARAM, BOOL&)
{
	
	// 设置窗口图标以避免关于对话框在Ctrl-Tab时无图标 [5/12/2010 何春龙]
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_MAINBIG), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);
    	
	m_pUpdateDlg = NULL;

	m_curIcon = ::LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_MAINFRAME));
	
	return 0;
}

void CHiddenWnd::ShowUpdateDlg()
{
	BOOL bCreate = FALSE;
	RECT rcView = {50, 50, 482, 410};

	if(!m_pUpdateDlg)
	{
        m_pUpdateDlg = new CLUpdateDlg;
    }
    
	if (!m_pUpdateDlg->IsWindow())
	{
		if (m_pUpdateDlg->CreateEx(&rcView,  DS_MODALFRAME, m_HiddenWnd.m_hWnd) == NULL)
		{
			return ;
		}

		bCreate = TRUE;
	}
    
	m_pUpdateDlg->ShowWindow(SW_SHOW);
	m_pUpdateDlg->CenterWindow();

	::SetWindowPos(m_pUpdateDlg->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	if (!::IsIconic(m_pUpdateDlg->m_hWnd) && ::IsWindowVisible(m_pUpdateDlg->m_hWnd))
    {
	    ::SetForegroundWindow(m_pUpdateDlg->m_hWnd);
	}

	ResetTrayIcon(TRUE);	

	return;
}

void CHiddenWnd::BeginUpdate()
{
	CLUpdater::GetInstance()->Run();
}

LRESULT CHiddenWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL&)
{
	::Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
	return 0;
}

LRESULT CHiddenWnd::OnTimer(UINT msg, WPARAM wParam , LPARAM, BOOL&bHandled)
{
	float fPos = CLUpdater::GetInstance()->GetCompletePercent();
	::PostMessage(::GetParent(m_hWnd), WM_EDU_CHANGE_PROGRESSPOS, (WPARAM)fPos, (LPARAM)m_hWnd);
	return 0;
}

void CHiddenWnd::SetTrayTimer(BOOL bStart)
{
	bStart ? SetTimer(TIMERID_FLASH_TRAY, 500, NULL):KillTimer(TIMERID_FLASH_TRAY);
}

void CHiddenWnd::ResetTrayIcon(BOOL bCreate)
{
	m_NotifyIconData.cbSize				= sizeof(NOTIFYICONDATA);  
	m_NotifyIconData.hWnd				= m_hWnd;  
	m_NotifyIconData.uFlags				= NIF_ICON|NIF_MESSAGE;  
	m_NotifyIconData.uCallbackMessage	= WM_UPDATE_TRAY_NOTIFY;


	m_NotifyIconData.uID	= IDR_MAINFRAME;	
	m_NotifyIconData.hIcon	= m_curIcon;

	// 设置托盘提示	
	if (bCreate)
	{
		::Shell_NotifyIcon(NIM_ADD,	&m_NotifyIconData);		
		return;
	}

	::Shell_NotifyIcon(NIM_MODIFY,&m_NotifyIconData);
}


LRESULT CHiddenWnd::OnTrayNotify(UINT, WPARAM, LPARAM lParam, BOOL&)
{

	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:	
		if(m_pUpdateDlg && m_pUpdateDlg->IsWindow())
		{
			m_pUpdateDlg->ShowWindow(SW_SHOWNORMAL);
			::SetForegroundWindow(m_pUpdateDlg->m_hWnd);			
		}
		break;
	}
	return 0;
}

LRESULT CHiddenWnd::OnDownloadOneFileSuccess(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	if(wParam == kDontNeedUpdate)
	{
		ExeClass8();		
		return 0;
	}
	else if(wParam == kUpdateDownloadProtocolVersion)
	{
		ShowUpdateDlg();
		CLUpdater::GetInstance()->DownloadManifestVersion();
	}
	else if(wParam == kUpdateStageDownloadLatestManifest)
	{
        CLUpdater::GetInstance()->DonwloadFiles();
    }
	else if(wParam == kUpdateLocalFiles)
	{
        CLUpdater::GetInstance()->DonwloadFiles();
    }
	else if(wParam == kUpdateComplete)
	{
		if(!m_pUpdateDlg)		
		{
			ExeClass8();
		}
	}
	else if(wParam == kUpdateFailed)
	{		
		ExeClass8();
	}
	return 0;
}

void CHiddenWnd::ExeClass8()
{
    ATL::CString strVersion;	
    ATL::CString strUpdateExe;

    wchar_t install_path_[MAX_PATH] = { 0 };
    wchar_t * pwszSpr = NULL;
    
    WCHAR wszProductFile[MAX_PATH] = { 0 };
    ::LoadStringW(NULL,IDS_PRODUCT_FILE,wszProductFile,MAX_PATH);

    GetModuleFileName(NULL, install_path_, 512);  
    
    pwszSpr = wcsrchr(install_path_,L'\\');
    if(NULL == pwszSpr)
    {
        return;
    }
    *pwszSpr = NULL;
    
    pwszSpr = wcsrchr(install_path_,L'\\');
    if(NULL == pwszSpr)
    {
        return;
    }

    *pwszSpr = NULL;    

    strVersion.Format(L"%s\\bin\\%s", install_path_,wszProductFile);
    strUpdateExe.Format(L"%s\\update\\update.exe", install_path_);
	
    ATL::CString parameters = L"updater_start";
    if (m_commandLine != L"")
    {
        parameters = m_commandLine;
    }

	::ShellExecute(NULL, _T("OPEN"), strVersion, parameters, NULL, SW_SHOW);	
    
    //xiewb 2016.04.28
    //::ShellExecute(NULL, _T("OPEN"), strUpdateExe, L"hotfix", NULL, SW_SHOW);
	
    //updateRegedit(strVersion);
	
    PostMessage(WM_QUIT); 
	DestroyWindow();
}

void CHiddenWnd::updateRegedit(ATL::CString strExePath)
{
	strExePath = strExePath.Left(strExePath.ReverseFind(L'\\'));
	strExePath += L"\\update.exe";

    WCHAR wszProduct[MAX_PATH] = {0};
    WCHAR wszKey[MAX_PATH] = {0};

    ::LoadStringW(NULL,IDS_PRODUCT,wszProduct,MAX_PATH);

	HKEY hRoot = HKEY_CLASSES_ROOT;
	HKEY keyHandle;
	if (RegOpenKeyEx(hRoot, wszProduct, 0, REG_OPENED_EXISTING_KEY, &keyHandle))
		return;
	if (keyHandle)
	{
		RegSetValueEx(keyHandle, L"URL Protocol", 0, REG_SZ, (BYTE*)strExePath.GetBuffer(), strExePath.GetLength() * 2);
		RegCloseKey(keyHandle);
	}

    swprintf(wszKey,L"%s\\DefaultIcon",wszProduct);
	if (RegOpenKeyEx(hRoot, wszKey, 0, REG_OPENED_EXISTING_KEY, &keyHandle))
		return;
	if (keyHandle)
	{
		ATL::CString defaultIdcon = strExePath;
		defaultIdcon += L",1";
		RegSetValueEx(keyHandle, L"", 0, REG_SZ, (BYTE*)defaultIdcon.GetBuffer(), defaultIdcon.GetLength() * 2);
		RegCloseKey(keyHandle);
	}

    swprintf(wszKey,L"%s\\shell\\open\\command",wszProduct);
	if (RegOpenKeyEx(hRoot, wszKey, 0, REG_OPENED_EXISTING_KEY, &keyHandle))
		return;
	if (keyHandle)
	{
		ATL::CString command = strExePath;
		command = L"\"" + strExePath;
		command += L"\",\"%1\"";
		RegSetValueEx(keyHandle, L"", 0, REG_SZ, (BYTE*)command.GetBuffer(), command.GetLength() * 2);
		RegCloseKey(keyHandle);
	}
}
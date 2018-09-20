
#include "stdafx.h"
#include <atlframe.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <atlstr.h>
#include <ObjBase.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#include "Env.h"
#include "xgdi/xgdiplus.h"
#include "common/Config.h"
#include "common/lang.h"

#pragma comment(lib, "gdiplus.lib")

extern std::wstring hotfix_exe_path;

#include "HiddenWnd.h"

CAppModule _Module;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 单实例机制
    char szMutex[MAX_PATH]={0};
    ::LoadStringA(NULL,IDS_UPDATE_MUTEX,szMutex,MAX_PATH);
	HANDLE handle = CreateMutexA(NULL, FALSE, szMutex);
	DWORD error = GetLastError();
	if (error == ERROR_ALREADY_EXISTS || error == ERROR_ACCESS_DENIED)
	{
        WCHAR wszTitle[MAX_PATH] = {0};
        WCHAR wszMsg[MAX_PATH] = {0};
        ::LoadStringW(NULL,IDS_MSG_TITLE,wszTitle,MAX_PATH);
        ::LoadStringW(NULL,IDS_EXSIT_UPDATE,wszMsg,MAX_PATH);

        char szProduct[MAX_PATH] = {0};
        ::LoadStringA(NULL,IDS_PRODUCT,szProduct,MAX_PATH);
        if(LangSet::loadLangSet("szProduct") == LangSet::LANG_ENG)
        {
            ::LoadStringW(NULL,IDS_MSG_TITLE_EN,wszTitle,MAX_PATH);
            ::LoadStringW(NULL,IDS_EXSIT_UPDATE_EN,wszMsg,MAX_PATH);
        }
        else
        {
            ::LoadStringW(NULL,IDS_MSG_TITLE,wszTitle,MAX_PATH);
            ::LoadStringW(NULL,IDS_EXSIT_UPDATE,wszMsg,MAX_PATH);
        }

		::MessageBoxW(NULL, wszMsg, wszTitle, MB_OK);
		
        if (handle)
		{
			CloseHandle(handle);
			handle = NULL;
		}

		return 0;
	}

#ifdef _DEBUG
    ::MessageBoxA(NULL,"消息","调试",MB_OK);
#endif

	CoInitialize(NULL); 

    Config::getConfig();
	_Module.Init(NULL, hInstance);
	xgdi::CXGdiplus gdi;
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
  
	int numarg = 1;
	LPWSTR* cmd = CommandLineToArgvW(GetCommandLine(), &numarg);
	if(numarg > 1)
	{
		if(_tcscmp(cmd[1], L"hotfix") == 0)
		{
			CloseHandle(handle);

			//进行相关的更新后操作，然后退出
			return 0;
		}
	}
	LocalFree(cmd); 

    WCHAR wszProduct[MAX_PATH] = { 0 };
    WCHAR wszFlag[MAX_PATH] = { 0 };
    ::LoadStringW(NULL,IDS_PRODUCT,wszProduct,MAX_PATH);
    swprintf(wszFlag,L"\"%s://",wszProduct);

	ATL::CString commandLine = lpCmdLine;
	commandLine = commandLine.Right(commandLine.GetLength() - commandLine.Find(wszFlag));
	m_HiddenWnd.setCommandLine(commandLine);
	if (m_HiddenWnd.Create(NULL, CRect(0,0,0,0), NULL, WS_THICKFRAME) == NULL)
	{
		return false;
	}

    TCHAR szWndTxt[MAX_PATH]={0};
    ::LoadString(NULL,IDS_UPDATE_WND_TXT,szWndTxt,MAX_PATH);
	m_HiddenWnd.SetWindowText(szWndTxt);

	m_HiddenWnd.BeginUpdate();

	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();

	CloseHandle(handle);
    Config::freeConfig();
	CoUninitialize();
	return 0;
}

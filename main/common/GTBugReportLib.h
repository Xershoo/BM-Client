#ifndef GT_BUG_REPORT_LIB_INCLUDE_H_FILE
#define GT_BUG_REPORT_LIB_INCLUDE_H_FILE

#define GT_DEBUG_MODE_AUTO	0
#define GT_DEBUG_MODE_GUI	1

typedef BOOL (_stdcall *LPSETEXCEPTIONHANDLER)(LPCSTR lpszModuleName,DWORD dwClientVersion,UINT nMode);
typedef DWORD (_stdcall *LPCREATECLIENTVERSIONINFO)(BYTE chMainVer,BYTE chSubVer,BYTE chExVer,DWORD dwBuildNO);
HINSTANCE g_hDebugInstance = NULL;
#include <qDebug>
BOOL SetExceptionFilter(UINT nMode,LPCSTR lpszModuleName,BYTE chMainVer,BYTE chSubVer,BYTE chExVer,DWORD dwBuildNO)
{
	if(g_hDebugInstance)
		return TRUE;
	LPSETEXCEPTIONHANDLER lpSETH = NULL;
	LPCREATECLIENTVERSIONINFO lpCCVI = NULL;
    wchar_t dllName[] = L"BugReport.dll";
	g_hDebugInstance = ::LoadLibrary(dllName);
    
	BOOL bFlag = FALSE;
	if(g_hDebugInstance)
	{
		lpSETH = (LPSETEXCEPTIONHANDLER)::GetProcAddress(g_hDebugInstance,"SetExceptionHandler");
		lpCCVI = (LPCREATECLIENTVERSIONINFO)::GetProcAddress(g_hDebugInstance,"CreateClientVersionInfo");
		if(lpSETH && lpCCVI)
		{
			DWORD dwVer = lpCCVI(chMainVer,chSubVer,chExVer,dwBuildNO);
			lpSETH(lpszModuleName,dwVer,nMode);
            bFlag = TRUE;
		}
		else
		{
			::FreeLibrary(g_hDebugInstance);
			g_hDebugInstance = NULL;
		}
	}

	return bFlag;
}

void UnSetExceptionFilter()
{
	if(g_hDebugInstance)
	{
		::FreeLibrary(g_hDebugInstance);
		g_hDebugInstance = NULL;
	}
}

#endif //GT_BUG_REPORT_LIB_INCLUDE_H_FILE
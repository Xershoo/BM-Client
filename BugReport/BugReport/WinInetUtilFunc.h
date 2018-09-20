// WinInetUtilFunc.h: interface for the WinInetUtilFunc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WININETUTILFUNC_H__ED20E05D_6B08_42C2_B9EC_AB9E3447CCC5__INCLUDED_)
#define AFX_WININETUTILFUNC_H__ED20E05D_6B08_42C2_B9EC_AB9E3447CCC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxinet.h>

BOOL HttpQueryInfo(HINTERNET hHttpFile, DWORD dwInfoLevel, DWORD& dwResult,
	LPDWORD lpdwIndex = NULL);

BOOL HttpQueryInfo(HINTERNET hHttpFile, DWORD dwInfoLevel, SYSTEMTIME* pSystemTime,
	LPDWORD lpdwIndex = NULL);

BOOL HttpQueryInfo(HINTERNET hHttpFile, DWORD dwInfoLevel, CString& str, 
	LPDWORD lpdwIndex = NULL);

BOOL HttpQueryInfoStatusCode(HINTERNET hHttpFile, DWORD& dwStatusCode);


#endif // !defined(AFX_WININETUTILFUNC_H__ED20E05D_6B08_42C2_B9EC_AB9E3447CCC5__INCLUDED_)

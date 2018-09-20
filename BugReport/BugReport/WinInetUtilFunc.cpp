// WinInetUtilFunc.cpp: implementation of the WinInetUtilFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinInetUtilFunc.h"

BOOL HttpQueryInfo(HINTERNET hHttpFile, DWORD dwInfoLevel, DWORD& dwResult,
	LPDWORD lpdwIndex /* = NULL */)
{
	ASSERT(hHttpFile != NULL);
	dwInfoLevel |= HTTP_QUERY_FLAG_NUMBER;
	DWORD dwDWSize = sizeof(DWORD);
	return ::HttpQueryInfo(hHttpFile, dwInfoLevel, &dwResult, &dwDWSize, lpdwIndex);
}

BOOL HttpQueryInfo(HINTERNET hHttpFile, DWORD dwInfoLevel, SYSTEMTIME* pSystemTime,LPDWORD lpdwIndex /* = NULL */)
{
	ASSERT(hHttpFile != NULL);
	dwInfoLevel |= HTTP_QUERY_FLAG_SYSTEMTIME;
	DWORD dwTimeSize = sizeof(SYSTEMTIME);
	return ::HttpQueryInfo(hHttpFile, dwInfoLevel, pSystemTime, &dwTimeSize, lpdwIndex);
}

BOOL HttpQueryInfo(HINTERNET hHttpFile, DWORD dwInfoLevel, CString& str, LPDWORD lpdwIndex /* = NULL */)
{
	ASSERT(hHttpFile != NULL);
	BOOL bRet;
	DWORD dwLen = 0;

	// ask for nothing to see how long the return really is
	str.Empty();
	if (HttpQueryInfo(hHttpFile, dwInfoLevel, NULL, &dwLen, 0))
	{
		bRet = TRUE;
	}
	else
	{
		// now that we know how long it is, ask for exactly that much
		// space and really request the header from the API
		LPTSTR pstr = str.GetBufferSetLength(dwLen);
		bRet = HttpQueryInfo(hHttpFile, dwInfoLevel, pstr, &dwLen, lpdwIndex);
		if (bRet)
			str.ReleaseBuffer(dwLen);
		else
			str.ReleaseBuffer(0);
	}
	return bRet;
}

BOOL HttpQueryInfoStatusCode(HINTERNET hHttpFile, DWORD& dwStatusCode)
{
	ASSERT(hHttpFile != NULL);
	TCHAR szBuffer[80];
	DWORD dwLen = sizeof(szBuffer) / sizeof(TCHAR);
	BOOL bRet = ::HttpQueryInfo(hHttpFile, HTTP_QUERY_STATUS_CODE,szBuffer, &dwLen, NULL);
	if (bRet)
		dwStatusCode = (DWORD) _ttol(szBuffer);
	return bRet;
}

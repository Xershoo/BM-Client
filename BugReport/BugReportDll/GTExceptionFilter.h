// GTExceptionFilter.h: interface for the CGTExceptionFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GTEXCEPTIONFILTER_H__4CDBBF6A_08F7_471C_AFB7_BF6AFB9DB910__INCLUDED_)
#define AFX_GTEXCEPTIONFILTER_H__4CDBBF6A_08F7_471C_AFB7_BF6AFB9DB910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <afxtempl.h>

struct MODULEENTRY
{
	std::string imageName;
	std::string moduleName;
	DWORD baseAddress;
	DWORD size;
};

typedef std::vector< MODULEENTRY > ModuleList;

class CGTExceptionFilter  
{
public:
	//////////////cuncun/////////////////////
	static CString GetRegisterInformation(LPEXCEPTION_POINTERS pEX);
	static CString GetCPUShare();
	static CString GetCPUTime(HANDLE hProcess);
	static CString GetProcessMemInfo(HANDLE hProcess);
	static CString GetProHdlUserGdi(HANDLE hProcess);
	static CString GetProThdPidExe(DWORD dwCurrentProcessId);
	///////////////cuncun//////////////////
	static long _stdcall UnExpectedExceptionFilter(LPEXCEPTION_POINTERS pEX);
	static void ShowStack(HANDLE hThread, CONTEXT&  Context,CPtrArray* pInfoArry,CPtrArray* pModuleInfo);
	static void TranslateSEHtoCE(UINT nCode, PEXCEPTION_POINTERS pEP);
	static long ExceptionFilterProc(UINT nCode,LPEXCEPTION_POINTERS pEX);
	static void enumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid,CPtrArray* pModuleInfo);
	static BOOL FillModuleList( ModuleList& modules, DWORD pid, HANDLE hProcess );
	static BOOL FillModuleListTH32( ModuleList& modules, DWORD pid );
	static BOOL FillModuleListPSAPI( ModuleList& modules, DWORD pid, HANDLE hProcess );
	static CString GetWindowsVersion();
	static CString GetDebugMD5HashString(CDWordArray* pArray);
	static CString GetMiniDumpDataFile(LPEXCEPTION_POINTERS pEX);
protected:
	static BOOL	GetCallStackInfo(CPtrArray& arDebugInfo,CPtrArray& arModuleInfo,LPEXCEPTION_POINTERS pEX);
	static CString GetExceptionDes(UINT nCode,LPEXCEPTION_POINTERS pEX);
	static BOOL	WriteDataToServer(CString strOutput,CString strMD5,CString strMiniDumpFile);
	static DWORD GetExceptionOffsetAddress(LPEXCEPTION_POINTERS pEX,CPtrArray* pModuleInfo);
};

#endif // !defined(AFX_GTEXCEPTIONFILTER_H__4CDBBF6A_08F7_471C_AFB7_BF6AFB9DB910__INCLUDED_)

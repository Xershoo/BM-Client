// GTExceptionFilter.cpp: implementation of the CGTExceptionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GTExceptionFilter.h"
#include <vector>
#include <float.h>
#include "afx.h"
#include "tlhelp32.h"
#include "winbase.h"
#include   "psapi.h"
#include "winnt.h"
#include "winuser.h"
#include "md5.h"
#include "zlibcpp.h"
#include "Dbghelp.h"
#include "shlwapi.h"
typedef BOOL (__stdcall *tKrnlGetProcessHandleCount)(HANDLE hProcess,PDWORD pdwHandleCount);
typedef LONG ( __stdcall *tQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD );
typedef struct _THREAD_INFO
{
	LARGE_INTEGER CreateTime;
	DWORD dwUnknown1;
	DWORD dwStartAddress;
	DWORD StartEIP;
	DWORD dwOwnerPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD Unknown;
	DWORD WaitReason;
}THREADINFO, *PTHREADINFO;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR Buffer;
} UNICODE_STRING;

typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];
	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	ULONG dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;
	DWORD dCommitCharge;
	THREADINFO ThreadSysInfo[1];
} PROCESSINFO, *PPROCESSINFO;
///////////////////////////cuncun/////////////////////

//////////////////////////////////////////////////////////////////////
typedef ModuleList::iterator ModuleListIter;

// miscellaneous toolhelp32 declarations; we cannot #include the header
// because not all systems may have it
#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPMODULE   0x00000008
#pragma pack( push, 8 )
/*typedef struct tagMODULEENTRY32
{
    DWORD   dwSize;
    DWORD   th32ModuleID;       // This module
    DWORD   th32ProcessID;      // owning process
    DWORD   GlblcntUsage;       // Global usage count on the module
    DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
    BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;            // The hModule of this module in th32ProcessID's context
    char    szModule[MAX_MODULE_NAME32 + 1];
    char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32 *  PMODULEENTRY32;
typedef MODULEENTRY32 *  LPMODULEENTRY32;*/
#pragma pack( pop )

//////////////////////////////////////////////////////////////////////
#define MAXNAMELEN 1024 // max name length for found symbols
#define IMGSYMLEN ( sizeof IMAGEHLP_SYMBOL )

typedef struct tagDEBUG_INFO
{
	DWORD dwEIPOffset;
	DWORD dwPCAddress;
	DWORD dwBaseAddress;
	DWORD dwRetureAddress;
	DWORD dwFrameAddress;
	DWORD dwStackAddress;
	CString strImageName;
}DEBUG_INFO;

typedef struct tagMODULE_INFO
{
	CString	strModuleName;
	DWORD	dwBaseAddress;
	DWORD	dwAddressLength;
	int	nMainVer;
	int nSubVer;
	int nExVer;
	DWORD dwBuildNO;
}MODULE_INFO;

DWORD	g_dwClientVersion = 0;//Client的版本号
CString g_strDebugModuleName = "";//产品模块名
UINT	g_nDebugMode = 0;///Debug表现模式
DWORD	g_dwClientStartTime = 0;

extern "C" _declspec(dllexport) BOOL _stdcall SetExceptionHandler(LPCSTR lpszModuleName,DWORD dwClientVersion,UINT nMode)
{
	g_strDebugModuleName = lpszModuleName;
	g_dwClientVersion = dwClientVersion;
	g_nDebugMode = nMode;
	g_dwClientStartTime = ::GetTickCount();
	
	::SetUnhandledExceptionFilter(CGTExceptionFilter::UnExpectedExceptionFilter);
	return TRUE;
}

extern "C" _declspec(dllexport) DWORD _stdcall CreateClientVersionInfo(BYTE chMainVer,BYTE chSubVer,BYTE chExVer,DWORD dwBuildNO)
{
	DWORD dwInfo = (chMainVer * 10000 + chSubVer * 100 + chExVer) * 10000 + dwBuildNO;
	return dwInfo;
}

BOOL ZipFile(LPCTSTR lpszSrcFile,LPCTSTR lpszDesFile)
{
	if(lpszSrcFile == NULL || lpszDesFile == NULL)
		return FALSE;

	bool bFlag = FALSE;
	CZLib Zip;
	if(Zip.Open(lpszDesFile))
	{
		if(Zip.AddFile(lpszSrcFile))
			bFlag = TRUE;
		Zip.Close();
	}
	return bFlag;
}

void GetClientVersionInfo(DWORD dwVerInfo,BYTE& chMainVer,BYTE& chSubVer,BYTE& chExVer,DWORD& dwBuildNO)
{
	chMainVer = (dwVerInfo / 10000 ) / 10000;
	chSubVer = ((dwVerInfo / 10000) % 10000) / 100;
	chExVer = ((dwVerInfo / 10000) % 10000) % 100;
	dwBuildNO = dwVerInfo % 10000;
}

BOOL GetFileVersion(LPCTSTR lpszFileName,int& nMainVer,int& nSubVer,int& nExVer,DWORD& dwBuildNO)
{
	BOOL bFlag = FALSE;
	if(lpszFileName)
	{
		DWORD dwLen = ::GetFileVersionInfoSize(lpszFileName,NULL);
		if(dwLen != 0)
		{
			BYTE* pBuffer = new BYTE[dwLen];
			if(pBuffer)
			{
				if(::GetFileVersionInfo(lpszFileName,NULL,dwLen,pBuffer))
				{
					VS_FIXEDFILEINFO* lpFileInfo = NULL;
					UINT nLen = 0;
					if(::VerQueryValue(pBuffer,TEXT("\\"),(void**)&lpFileInfo,&nLen))
					{
						DWORD dwNumber = 65536;
						nMainVer = lpFileInfo->dwFileVersionMS / dwNumber;
						nSubVer = lpFileInfo->dwFileVersionMS % dwNumber;
						nExVer = lpFileInfo->dwFileVersionLS / dwNumber;
						dwBuildNO = lpFileInfo->dwFileVersionLS % dwNumber;
						bFlag = TRUE;
					}
				}
				delete []pBuffer;
			}
		}
	}
	return bFlag;
}

CString CGTExceptionFilter::GetWindowsVersion()
{
	CString strOSVer = "";
	OSVERSIONINFO OSVer;
	OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&OSVer))
	{
		DWORD dwMajVer = OSVer.dwMajorVersion;
		DWORD dwMinVer = OSVer.dwMinorVersion;
		if(dwMajVer == 4 && dwMinVer == 0)
		{
			if(strcmp(OSVer.szCSDVersion," C") == 0)
				strOSVer.Format("Microsoft Windows 95 OSR2 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,LOWORD(OSVer.dwBuildNumber));
			else
				strOSVer.Format("Microsoft Windows 95 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,LOWORD(OSVer.dwBuildNumber));
		}
		else if(dwMajVer == 4 && dwMinVer == 10)
		{
			if(strcmp(OSVer.szCSDVersion," A") == 0)
				strOSVer.Format("Microsoft Windows 98 Second Edition [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,LOWORD(OSVer.dwBuildNumber));
			else
				strOSVer.Format("Microsoft Windows 98 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,LOWORD(OSVer.dwBuildNumber));
		}
		else if(dwMajVer == 4 && dwMinVer == 90)
		{
			strOSVer.Format("Microsoft Windows ME [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,LOWORD(OSVer.dwBuildNumber));
		}
		else if(dwMajVer == 3 && dwMinVer == 51)
		{
			strOSVer.Format("Microsoft Windows NT 3.51 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else if(dwMajVer == 4 && dwMinVer == 0)
		{
			strOSVer.Format("Microsoft Windows NT 4.0 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else if(dwMajVer == 5 && dwMinVer == 0)
		{
			strOSVer.Format("Microsoft Windows 2000 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else if(dwMajVer == 5 && dwMinVer == 1)
		{
			if(strlen(OSVer.szCSDVersion) > 0)
				strOSVer.Format("Microsoft Windows XP %s [Build:%ld.%ld.%ld]",OSVer.szCSDVersion,dwMajVer,dwMinVer,OSVer.dwBuildNumber);
			else
				strOSVer.Format("Microsoft Windows XP [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else if(dwMajVer == 5 && dwMinVer == 1)
		{
			if(strlen(OSVer.szCSDVersion) > 0)
				strOSVer.Format("Microsoft Windows XP %s [Build:%ld.%ld.%ld]",OSVer.szCSDVersion,dwMajVer,dwMinVer,OSVer.dwBuildNumber);
			else
				strOSVer.Format("Microsoft Windows XP [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else if(dwMajVer == 5 && dwMinVer == 2)
		{
			strOSVer.Format("Microsoft Windows Server 2003 [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else if(dwMajVer == 6 && dwMinVer == 0)
		{
			strOSVer.Format("Microsoft Windows Vista [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
		}
		else
			strOSVer.Format("Microsoft Windows Later Version [Build:%ld.%ld.%ld]",dwMajVer,dwMinVer,OSVer.dwBuildNumber);
	}
	return strOSVer;
}

void CGTExceptionFilter::TranslateSEHtoCE(UINT nCode, PEXCEPTION_POINTERS pEP)
{
	CGTExceptionFilter::ExceptionFilterProc(nCode,pEP);


    /*	
    if(nCode == EXCEPTION_STACK_OVERFLOW || nCode == EXCEPTION_FLT_STACK_CHECK)
	{
		throw new CUnRecoverableSEHException(nCode, pEP);
	}
	else
	{
		throw new CRecoverableSEHException(nCode, pEP);
	}	
    */
}

CString CGTExceptionFilter::GetDebugMD5HashString(CDWordArray* pArray)
{
	CString str;
	int nSize = pArray->GetSize();
	if(nSize > 0)
	{
		int nLen = nSize * 4;
		BYTE* pBuffer = new BYTE[nLen];
		int i = 0;
		for(i = 0;i < nSize;i++)
		{
			DWORD dw = pArray->GetAt(i);
			memcpy(&pBuffer[i * 4],&dw,4);
		}

		BYTE chMD5[16];
		md5_t::build(chMD5,pBuffer,nLen);
		delete pBuffer;
		
		char ch[32];
		for(i = 0;i < 16;i++)
		{
			sprintf(ch,"%02X",chMD5[i]);
			str += ch;
		}
	}
	else
		str = "00000000000000000000000000000000";

	return str;	
}

CString CGTExceptionFilter::GetMiniDumpDataFile(LPEXCEPTION_POINTERS pEX)
{
	CString strFileName = _T("");
	TCHAR chPathName[1024];
	TCHAR chFileName[1024];
	BOOL bFlag = FALSE;
	DWORD dwLen = ::GetTempPath(1024,chPathName);
	BYTE chMainVer = 0,chSubVer = 0,chExVer = 0;
	DWORD dwBuildNO = 0;
	
    GetClientVersionInfo(g_dwClientVersion,chMainVer,chSubVer,chExVer,dwBuildNO);
	if(dwLen != 0)
	{
		chPathName[dwLen] = 0;
		UINT nLen = ::GetTempFileName(chPathName,"fgm",0,chFileName);
		CString strBuf = chFileName ;
		strBuf.Replace(_T(".tmp"),_T(".dmp"));
		int nPos = strBuf.ReverseFind(_T('\\'));
		if(nPos >= 0)
		{
			CString strTempVerion;
			strTempVerion.Format(_T("%s\\%s_%d.%d.%d.%d.%s"), strBuf.Mid(0,nPos), g_strDebugModuleName,chMainVer, chSubVer, chExVer, dwBuildNO, strBuf.Mid(nPos + 1));
			strBuf = strTempVerion;
		}
		_tcsncpy( chFileName,strBuf,strBuf.GetLength());
		chFileName[strBuf.GetLength()] = _T('\0');
		if(nLen != 0)
		{
			CFile FF;
			if(FF.Open(chFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			{
				FF.Seek(0,CFile::begin);
				FF.SetLength(0);
				
				MINIDUMP_EXCEPTION_INFORMATION ExceptionInfo;
				ExceptionInfo.ThreadId = GetCurrentThreadId();
				ExceptionInfo.ExceptionPointers = pEX;
				ExceptionInfo.ClientPointers = FALSE;
				
				if(MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),
					FF.m_hFile,MiniDumpNormal,pEX ? &ExceptionInfo : NULL,NULL,
                    NULL))
				{
					strFileName = chFileName;
					bFlag = TRUE;
				}
				
				FF.Close();
				
			}
			else
			{
				CString strBuf;
				strBuf.Format(_T("Error :%d,%s"),GetLastError(),chPathName);
				::MessageBox(0,strBuf,0,0);
			}
		}
	}
	
	if(bFlag)
	{
		bFlag = FALSE;
		CString strTemp;
		strTemp = strFileName;
		strTemp.Replace(_T(".dmp"),_T(".zip"));
		if(::ZipFile(strFileName,strTemp))
		{
			CFile::Remove(strFileName);
			strFileName = strTemp;
			bFlag = TRUE;
		}
	}
	
	if(!bFlag)
	{
		CFile FF;
		strFileName.Format(_T("C:\\%s.%d.%d.%d.%d_crash.dmp"), g_strDebugModuleName,chMainVer, chSubVer, chExVer, dwBuildNO);
		if(FF.Open(strFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
		{
			FF.SetLength(32);
			FF.Seek(0,CFile::begin);
			BYTE chBuf[32];
			memset(chBuf,0,sizeof(chBuf));
			FF.Write(chBuf,32);
			FF.Close();
		}
	}
		
	return strFileName;
}

BOOL CGTExceptionFilter::GetCallStackInfo(CPtrArray& arDebugInfo,CPtrArray& arModuleInfo,LPEXCEPTION_POINTERS pEX)
{
	HANDLE hThread = NULL;
	BOOL bFlag = ::DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),
		GetCurrentProcess(), &hThread, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if(!bFlag || hThread == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	
	}
	CGTExceptionFilter::ShowStack(hThread, *pEX->ContextRecord,&arDebugInfo,&arModuleInfo);
	::CloseHandle(hThread);
	return TRUE;
}

CString CGTExceptionFilter::GetExceptionDes(UINT nCode,LPEXCEPTION_POINTERS pEX)
{
	CString str = "Type: ";
	switch(nCode)
	{
		//	与内存有关的异常。
	case EXCEPTION_ACCESS_VIOLATION:
		str += "EXCEPTION_ACCESS_VIOLATION";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		str += "EXCEPTION_DATATYPE_MISALIGNMENT";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		str += "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		str += "EXCEPTION_IN_PAGE_ERROR";
		break;
	case EXCEPTION_GUARD_PAGE:
		str += "EXCEPTION_GUARD_PAGE";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		str += "EXCEPTION_ILLEGAL_INSTRUCTION";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		str += "EXCEPTION_PRIV_INSTRUCTION";
		break;
		//	与结构化异常相关的异常。
	case EXCEPTION_INVALID_DISPOSITION:
		str += "EXCEPTION_INVALID_DISPOSITION";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		str += "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		break;
		//	与整数有关的异常。
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		str += "EXCEPTION_INT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_INT_OVERFLOW:
		str += "EXCEPTION_INT_OVERFLOW";
		break;
		//	与浮点数有关的异常。
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		str += "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		str += "EXCEPTION_FLT_DENORMAL_OPERAND";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		str += "EXCEPTION_FLT_INEXACT_RESULT";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		str += "EXCEPTION_FLT_INVALID_OPERATION";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		str += "EXCEPTION_FLT_OVERFLOW";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		str += "EXCEPTION_FLT_STACK_CHECK";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		str += "EXCEPTION_FLT_UNDERFLOW";
		break;
		//	不能恢复的结构化异常。栈溢出
	case EXCEPTION_STACK_OVERFLOW:
		str += "EXCEPTION_STACK_OVERFLOW";
		break;
	default:
		{
			CString ss;
			ss.Format("EXCEPTION_UNKNOWN (Code: %ld)",nCode);
            str += ss;
		}
	}
	str += "\r\n";

	CString s;
	s.Format("Address: 0x%08X\r\n",pEX->ExceptionRecord->ExceptionAddress);
	str += s;

	if(nCode == EXCEPTION_ACCESS_VIOLATION)
	{
		if(pEX->ExceptionRecord->ExceptionInformation[0] == 0)//attempt read the inaccessible data
			s.Format("Description: Read the address 0x%08X\r\n",pEX->ExceptionRecord->ExceptionInformation[1]);
		else if(pEX->ExceptionRecord->ExceptionInformation[0] == 1)//attempt Write the inaccessible data
			s.Format("Description: Write the address 0x%08X\r\n",pEX->ExceptionRecord->ExceptionInformation[1]);
		else
			s = "Description: \r\n";
	}
	else
	{
		s = "Description: \r\n";
	}
	str += s + "\r\n";

	return str;
}
/////////////////////////////////////////////////cuncun//////////////////////////////////////
CString CGTExceptionFilter::GetRegisterInformation(LPEXCEPTION_POINTERS pEX)
{//获取寄存器信息
	CString strRegInfo = _T("");
	DWORD dwEAx = 0,dwEBx = 0,dwECx = 0,dwEDx = 0,dwESi = 0,dwEDi = 0,dwEBp = 0,dwESp = 0;
	dwEAx = pEX->ContextRecord->Eax;
	dwEBx = pEX->ContextRecord->Ebx;
	dwECx = pEX->ContextRecord->Ecx;
	dwEDx = pEX->ContextRecord->Edx;
	dwESi = pEX->ContextRecord->Esi;
	dwEDi = pEX->ContextRecord->Edi;
	dwEBp = pEX->ContextRecord->Ebp;
	dwESp = pEX->ContextRecord->Esp;
	strRegInfo.Format("\r\nRegister Information:\r\nEAX=0x%08X\tEBX=0x%08X\tECX=0x%08X\tEDX=0x%08X\t\nESI=0x%08X\tEDI=0x%08X\tEBP=0x%08X\tESP=0x%08X",dwEAx, dwEBx, dwECx, dwEDx,dwESi,dwEDi,dwEBp,dwESp);
	return strRegInfo;
}

CString CGTExceptionFilter::GetCPUShare()
 {//获得CPU占用率
	 CString strCPUShare = _T("");
	 PVOID pProcInfo = NULL;
	 DWORD dwInfoSize = 0x20000;
	 PPROCESSINFO pProcessInfo = NULL;
	 //long ( __stdcall *NtQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD );
	 //__int64 nTotalProcessCPUUsage = 0;
	 //__int64 nCurrentProcessCPUUsage = 0;
	 DWORD nTotalProcessCPUUsage = 0, nCurrentProcessCPUUsage = 0;
	 pProcInfo = (PVOID)(new BYTE[dwInfoSize]);
	 HINSTANCE hInst = ::LoadLibrary("ntdll.dll");
	 tQuerySystemInformation pProAdd = NULL;
	 if(hInst != NULL)
	 {
		 pProAdd = (tQuerySystemInformation)GetProcAddress( hInst,"NtQuerySystemInformation" ); 
		 if(pProAdd != NULL)
			 pProAdd(5,pProcInfo,dwInfoSize,0);
		 ::FreeLibrary(hInst);
	 }
	 pProcessInfo = (PPROCESSINFO)pProcInfo;
	 do
	 {
		 nTotalProcessCPUUsage += (DWORD)pProcessInfo->KernelTime.QuadPart + (DWORD)pProcessInfo->UserTime.QuadPart;
		 if(pProcessInfo->dwProcessID == GetCurrentProcessId())
			 nCurrentProcessCPUUsage += (DWORD)pProcessInfo->KernelTime.QuadPart + (DWORD)pProcessInfo->UserTime.QuadPart;
		 if(pProcessInfo->dwOffset == 0)
			 break;
		 pProcessInfo = (PPROCESSINFO)((BYTE*)pProcessInfo + pProcessInfo->dwOffset);
	 }while(TRUE);
	 delete [] pProcInfo;
	 if(nTotalProcessCPUUsage != 0)
		 strCPUShare.Format(_T("\r\n\r\nProcess Information:\r\nCPU Share: %d"),100 * nCurrentProcessCPUUsage / nTotalProcessCPUUsage);
	 return strCPUShare;
 }

 CString CGTExceptionFilter::GetCPUTime(HANDLE hProcess)
 { //获得进程CPU时间
	 CString strCPUTime = _T("");
	 ULONGLONG nCreateTime = 0,nExitTime = 0,nKernelTime = 0,nUserTime = 0;
	 BOOL bTemp = FALSE;
	 if(hProcess != NULL)
		  bTemp =::GetProcessTimes(hProcess,(LPFILETIME)&nCreateTime,(LPFILETIME)&nExitTime,(LPFILETIME)&nKernelTime,(LPFILETIME)&nUserTime);
	 if(bTemp == TRUE)
	 {
		 UINT  nTotalSeconds = UINT(((nKernelTime + nUserTime) / 10000000));
		 UINT  nSeconds = nTotalSeconds % 60;
		 UINT  nMinutes = nTotalSeconds / 60 % 60;
		 UINT  nHours = nTotalSeconds / 60 / 60 % 24;
		 strCPUTime.Format(_T("\r\nCPU Time: %d:%02d:%02d"),nHours,nMinutes,nSeconds);
	 }
	 return strCPUTime;
 }

 CString CGTExceptionFilter::GetProcessMemInfo(HANDLE hProcess)
 {//获取进程内存使用，内存使用峰值，虚拟内存
	 CString strTemp = _T("");
	 PROCESS_MEMORY_COUNTERS  pmcProcessMem;
	 if(hProcess != NULL)
	 {
		 if(GetProcessMemoryInfo( hProcess,&pmcProcessMem,sizeof( pmcProcessMem)))
		 {
			 DWORD dwMemUsage,dwVirtualMemory,dwPeakMemoryUsage;
			 dwMemUsage = pmcProcessMem.WorkingSetSize/1024;//内存使用
			 dwVirtualMemory = pmcProcessMem.PagefileUsage/1024;//虚拟内存
			 dwPeakMemoryUsage = pmcProcessMem.PeakWorkingSetSize/1024;//内存使用峰值
			 strTemp.Format(_T("\r\nMemory Usage: %dKB\r\nVirtual Memory: %dKB\r\nPeak Memory Usage: %dKB"),dwMemUsage,dwVirtualMemory,dwPeakMemoryUsage);
		 }
	 }
	 return strTemp;
 }

 CString CGTExceptionFilter::GetProHdlUserGdi(HANDLE hProcess)
 {//获取进程句柄,User对象,Gdi对象
	 CString strTemp = _T("");
	 DWORD dwProcessHandle = 0,dwGdiObject = 0,dwUserObject = 0;	
	 tKrnlGetProcessHandleCount pKrnlGetHandleCnt = NULL;
	 HINSTANCE hInst = ::LoadLibrary("Kernel32.dll");
	 if( hInst != NULL)
	 {
		 pKrnlGetHandleCnt = (tKrnlGetProcessHandleCount)GetProcAddress(hInst,"GetProcessHandleCount");
		 if( pKrnlGetHandleCnt != NULL )
			 pKrnlGetHandleCnt(hProcess,&dwProcessHandle);
		 ::FreeLibrary(hInst);
	 }
	 dwUserObject = GetGuiResources(hProcess, GR_USEROBJECTS);
	 dwGdiObject = GetGuiResources(hProcess, GR_GDIOBJECTS);
	 if( (dwUserObject != 0) && (dwGdiObject != 0))
		 strTemp.Format(_T("\r\nHandle: %d\r\nUSER Object: %d\r\nGDI Object: %d"),dwProcessHandle,dwUserObject,dwGdiObject);
	 return strTemp;
 }

 CString CGTExceptionFilter::GetProThdPidExe(DWORD dwCurrentProcessId)
 {//获取进程的线程数,正在运行的所有进程的PID和应用程序名 
	 PROCESSENTRY32  ProcessInfo;
	 ProcessInfo.dwSize = sizeof(PROCESSENTRY32);
	 HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	 CString strTaskList = _T("");
	 CString strThreadCount = _T("");
	 CString strTemp  = _T("");
	 CString strProThdPidExe = _T("");
	 if(hSnapShot != NULL)
	 {
		 while(Process32Next(hSnapShot,&ProcessInfo) != FALSE)
		 {
			 if(ProcessInfo.th32ProcessID == dwCurrentProcessId)
				 strThreadCount.Format(_T("\r\nThread: %d"), ProcessInfo.cntThreads);
			 strTemp.Format(_T("\r\n%d\t\t%s"), ProcessInfo.th32ProcessID, ProcessInfo.szExeFile);
			 strTaskList += strTemp;
			 strTemp.Empty();
		 }
		 CloseHandle(hSnapShot);
	 }
	 strProThdPidExe += strThreadCount;
	 strTemp.Format(_T("\r\n\r\nTask List"));
	 strProThdPidExe += strTemp;
	 strProThdPidExe += strTaskList;
	 return strProThdPidExe;
 }
/////////////////////////////////////////////////cuncun/////////////////////////////////////

BOOL CGTExceptionFilter::WriteDataToServer(CString strOutput,CString strMD5,CString strMiniDumpFile)
{
	BOOL bFlag = FALSE;
	TCHAR chPathName[1024];
	TCHAR chFileName[1024];
	TCHAR chEXEFile[1024];
	memset(chPathName,0,sizeof(chPathName));
	memset(chFileName,0,sizeof(chFileName));
	memset(chEXEFile,0,sizeof(chEXEFile));

	::GetModuleFileName(NULL,chEXEFile,1024);
	DWORD dwLen = ::GetTempPath(1024,chPathName);
	
	if(dwLen != 0)
	{
		chPathName[dwLen] = 0;
		UINT nLen = ::GetTempFileName(chPathName,"fgb",0,chFileName);
		if(nLen != 0)
		{
			CFile ff;
			if(ff.Open(chFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			{
				ff.Seek(0,CFile::begin);
				ff.SetLength(0);
				int nBufLen = strOutput.GetLength();
				ff.Write(strOutput.GetBuffer(nBufLen),nBufLen);
				strOutput.ReleaseBuffer(nBufLen);
				ff.Close();

				CString strParams;
				strParams.Format("%ld %s %ld %s %ld \"%s\" \"%s\" \"%s\"",g_nDebugMode,g_strDebugModuleName,g_dwClientVersion,strMD5,nBufLen,chFileName,chEXEFile,strMiniDumpFile);
				
#ifdef _DEBUG
				HINSTANCE hIns = ::ShellExecute(NULL,"open","BugReportD.exe",(LPCTSTR)strParams,NULL,SW_SHOWNA);
#else
				HINSTANCE hIns = ::ShellExecute(NULL,"open","BugReport.exe",(LPCTSTR)strParams,NULL,SW_SHOWNA);
#endif
				if((int)hIns > 32)
					bFlag = TRUE;
			}
		}
	}
	return bFlag;
}

DWORD CGTExceptionFilter::GetExceptionOffsetAddress(LPEXCEPTION_POINTERS pEX,CPtrArray* pModuleInfo)
{
	DWORD dwExceptionAddr = (DWORD)pEX->ExceptionRecord->ExceptionAddress;
	int nSize = pModuleInfo->GetSize();
	for(int i = 0;i < nSize;i++)
	{
		MODULE_INFO* pModule = (MODULE_INFO*)(pModuleInfo->GetAt(i));
		if(dwExceptionAddr >= pModule->dwBaseAddress && dwExceptionAddr < (pModule->dwAddressLength + pModule->dwBaseAddress))
		{
			dwExceptionAddr = dwExceptionAddr - pModule->dwBaseAddress;
			break;
		}
	}
	return dwExceptionAddr;
}


long CGTExceptionFilter::ExceptionFilterProc(UINT nCode,LPEXCEPTION_POINTERS pEX)
{
	CPtrArray arDebugInfo;
	CPtrArray arModuleInfo;
	
		
	CString strMiniDumpFile = GetMiniDumpDataFile(pEX);
	
	//BOOL bFlag = CGTExceptionFilter::GetCallStackInfo(arDebugInfo,arModuleInfo,pEX);

	CString str;
	CString strOutput = GetWindowsVersion() + "\r\n\r\n";
	CDWordArray	arMD5Data;///////用于准备需要MD5的信息
	arMD5Data.Add(g_dwClientVersion);//将Client版本号做为MD5信息之一

	BYTE chMainVer = 0,chSubVer = 0,chExVer = 0;
	DWORD dwBuildNO = 0;
	////////////////获取程序启动到崩溃时存活的时间///////
	DWORD dwTime = (::GetTickCount() - g_dwClientStartTime) / 1000;
	UINT nHour = dwTime / (60 * 60);
	UINT nMin = (dwTime % (60 * 60)) / 60;
	UINT nSec = (dwTime % (60 * 60)) % 60;

	GetClientVersionInfo(g_dwClientVersion,chMainVer,chSubVer,chExVer,dwBuildNO);
	str.Format("%s %ld.%ld.%ld.%ld   MD5_HASH_DATA_REPLACE_HERE   %02d:%02d:%02d\r\n",g_strDebugModuleName,chMainVer,chSubVer,
		chExVer,dwBuildNO,nHour,nMin,nSec);
	strOutput += str;
	strOutput += "----------------------------------------------------\r\n";
	
	//将异常代码做为MD5信息之一
	arMD5Data.Add(nCode);
	//////将发生崩溃的基于对应模块基地址的偏移地址做为MD5信息之一
	arMD5Data.Add(GetExceptionOffsetAddress(pEX,&arModuleInfo));

	strOutput += CGTExceptionFilter::GetExceptionDes(nCode,pEX);
	
	strOutput += "Call Stack:\r\n";
	int nSize = arDebugInfo.GetSize();
	int nNumber = 3;///用于控制将多少层的Call Stack Address加入到MD5hash数据中,现在是3层
	for(int i = 0;i < nSize;i++)
	{
		DEBUG_INFO* pInfo = (DEBUG_INFO*)arDebugInfo[i];
		str.Format("0x%08X[%X] %s: (0x%08X,0x%08X,0x%08X)\r\n",pInfo->dwBaseAddress,
					pInfo->dwEIPOffset,pInfo->strImageName,pInfo->dwRetureAddress,
					pInfo->dwFrameAddress,pInfo->dwStackAddress);
		strOutput += str;
		if(i < nNumber)
			arMD5Data.Add(pInfo->dwPCAddress);	
		delete pInfo;
	}	
	arDebugInfo.RemoveAll();
		
	CString strMD5 = CGTExceptionFilter::GetDebugMD5HashString(&arMD5Data);
	strOutput.Replace("MD5_HASH_DATA_REPLACE_HERE",strMD5);
	arMD5Data.RemoveAll();

	///获取进程加载的所有模块信息
	strOutput += "\r\nLoad Module List:\r\n";
	nSize = arModuleInfo.GetSize();
	for(int i = 0;i < nSize;i++)
	{
		MODULE_INFO* pModule = (MODULE_INFO*)arModuleInfo[i];
		str.Format("%s [0x%08X] (Ver:%ld.%ld.%ld.%ld)\r\n",pModule->strModuleName,pModule->dwBaseAddress,
			pModule->nMainVer,pModule->nSubVer,pModule->nExVer,pModule->dwBuildNO);
		strOutput += str;
		delete pModule;
	}
	arModuleInfo.RemoveAll();
	//////////////////////////////////////////////cuncun///////////////////////////////////////////////
	CString strTemp = _T("");
	//获取寄存器信息
	strOutput += GetRegisterInformation(pEX);
	
	//获得系统时间
	SYSTEMTIME stSystemDateTime;
	GetLocalTime(&stSystemDateTime);
	strTemp.Format(_T("\r\n\r\nSystem Time: %4d-%02d-%02d %02d:%02d:%02d"),stSystemDateTime.wYear,stSystemDateTime.wMonth,stSystemDateTime.wDay,
	stSystemDateTime.wHour,stSystemDateTime.wMinute,stSystemDateTime.wSecond);
	strOutput += strTemp;
	
	//获得CPU占用率，进程时间和所有进程的总时间的比值即CPU占有率
//	strOutput += GetCPUShare();
	
	//获得进程CPU时间
	HANDLE hProcess = GetCurrentProcess();
	strOutput += GetCPUTime(hProcess);
	
	//获取进程内存使用，内存使用峰值，虚拟内存
	strOutput += GetProcessMemInfo(hProcess);
	
	//获取进程句柄,User对象,Gdi对象
	strOutput += GetProHdlUserGdi(hProcess);
	::CloseHandle(hProcess);
	
	//获取进程的线程数,正在运行的所有进程的PID和应用程序名 
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	strOutput += GetProThdPidExe(dwCurrentProcessId);
//////////////////////////////////////////////cuncun////////////////////////////////////

	CString strPath = _T("");
	TCHAR tczPath[MAX_PATH] = {0};

	HRESULT hRet = ::SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, tczPath);
	if(SUCCEEDED(hRet))
	{
		strPath = tczPath;
		strPath = strPath + _T("\\GT\\dump");
		SHCreateDirectoryEx(NULL, strPath, NULL);
		
		int nPos = strMiniDumpFile.ReverseFind(_T('\\'));
		if(nPos >= 0)
		{
			strPath.AppendFormat(_T("\\%s"),strMiniDumpFile.Mid(nPos + 1));
			::CopyFile(strMiniDumpFile, strPath, FALSE);
		}
	}
///////////数据准备好，交给服务程序进行发送////////////////

	CGTExceptionFilter::WriteDataToServer(strOutput,strMD5,strMiniDumpFile);		
	CString strCmdLine;
	DWORD dwNumFile =1;
	CString strTaskDatabase = MakeFullPath( GetExePath(), _T("Profiles\\tasks.dat"));
	CString strQuitReport = MakeFullPath( GetExePath(), _T("QuitReport.exe"));
	if( strQuitReport.GetLength() && ::PathFileExists( strQuitReport) )
	{
		
		strCmdLine = GetReportComment(strMD5);
		strCmdLine += _T(" ");
		if( PathFileExists( strTaskDatabase ))
		{
			strMiniDumpFile += (_T("*")+strTaskDatabase);
			dwNumFile ++;
		}
		strCmdLine.AppendFormat(_T("%d 0 %d %s"),dwTime,dwNumFile,strMiniDumpFile );
		DWORD dwRet = (DWORD) ::ShellExecute(NULL, _T("open"), strQuitReport, strCmdLine, NULL, SW_HIDE);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}



long CGTExceptionFilter::UnExpectedExceptionFilter(LPEXCEPTION_POINTERS pEX)
{
	return CGTExceptionFilter::ExceptionFilterProc(pEX->ExceptionRecord->ExceptionCode,pEX);
}

void CGTExceptionFilter::ShowStack(HANDLE hThread, CONTEXT& Context,CPtrArray* pInfoArry,CPtrArray* pModuleInfo)
{
	//char chOutput[10240];
	// normally, call ImageNtHeader() and use machine info from PE header
	DWORD dwOffsetFromSymbol = 0; // tells us how far from the symbol we were
	DWORD dwSymOptions = 0; // symbol handler settings
	IMAGEHLP_SYMBOL *pSym = (IMAGEHLP_SYMBOL*)malloc(IMGSYMLEN + MAXNAMELEN);
	char undName[MAXNAMELEN]; // undecorated name
	char undFullName[MAXNAMELEN]; // undecorated name with all shenanigans
	CString strSearchPath = "";
	char chPathName[MAXNAMELEN];

	IMAGEHLP_MODULE Module;
	IMAGEHLP_LINE Line;
	STACKFRAME StackFrame; // in/out stackframe
	memset(&StackFrame,0,sizeof(StackFrame));
	
	// current directory
	if(GetCurrentDirectory(sizeof(chPathName),chPathName))
	{
		strSearchPath += chPathName;
		strSearchPath += ";";
	}
	// dir with executable
	if(GetModuleFileName(NULL,chPathName,sizeof(chPathName)) != 0)
	{
		int nLen = strlen(chPathName);
		int i = 0;
		for(i = nLen - 1;i >= 0;i--)
		{
			if(chPathName[i] =='\\' || chPathName[i] == '/' || chPathName[i] == ':')
				break;
		}
		if(i >= 0)
		{
			if(chPathName[i] == ':')
				chPathName[i + 1] = '\0';
			strSearchPath += chPathName;
			strSearchPath += ";";
		}
	}
	// environment variable _NT_SYMBOL_PATH
	if(GetEnvironmentVariable("_NT_SYMBOL_PATH",chPathName,sizeof(chPathName)))
	{
		strSearchPath += chPathName;
		strSearchPath += ";";
	}
	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if(GetEnvironmentVariable( "_NT_ALTERNATE_SYMBOL_PATH", chPathName,sizeof(chPathName)))
	{
		strSearchPath += chPathName;
		strSearchPath += ";";
	}
	// environment variable SYSTEMROOT
	if(GetEnvironmentVariable( "SYSTEMROOT",chPathName,sizeof(chPathName)))
	{
		strSearchPath += chPathName;
		strSearchPath += ";";
	}
	if(strSearchPath.GetLength() > 0)// if we added anything, we have a trailing semicolon
		strSearchPath = strSearchPath.Mid(0,strSearchPath.GetLength() - 1);
	
	///////////////////////////////
	//sprintf(chOutput,"symbols path: %s\r\n", strSearchPath);
	//g_strDebugInfo += chOutput;
	///////////////////////

	// why oh why does SymInitialize() want a writeable string?
	strncpy(chPathName, strSearchPath,sizeof(chPathName));
	chPathName[sizeof(chPathName) - 1] = '\0';

	// init symbol handler stuff (SymInitialize())
	HANDLE hProcess = ::GetCurrentProcess();
	if(::SymInitialize(hProcess,chPathName,FALSE))
	{
		dwSymOptions = ::SymGetOptions();
		dwSymOptions |= SYMOPT_LOAD_LINES;
		dwSymOptions &= ~SYMOPT_UNDNAME;
		::SymSetOptions(dwSymOptions);
		
		// Enumerate modules and tell imagehlp.dll about them.
		enumAndLoadModuleSymbols(hProcess,GetCurrentProcessId(),pModuleInfo);
		// init STACKFRAME for first call
		// Notes: AddrModeFlat is just an assumption. I hate VDM debugging.
		// Notes: will have to be #ifdef-ed for Alphas; MIPSes are dead anyway,and good riddance.
		StackFrame.AddrPC.Offset = Context.Eip;
		StackFrame.AddrPC.Mode = AddrModeFlat;
		StackFrame.AddrFrame.Offset = Context.Ebp;
		StackFrame.AddrFrame.Mode = AddrModeFlat;
		
		memset(pSym,0, IMGSYMLEN + MAXNAMELEN);
		pSym->SizeOfStruct = IMGSYMLEN;
		pSym->MaxNameLength = MAXNAMELEN;
		
		memset(&Line,0,sizeof(Line));
		Line.SizeOfStruct = sizeof(Line);
		
		memset(&Module,0,sizeof(Module));
		Module.SizeOfStruct = sizeof(Module);
		
		dwOffsetFromSymbol = 0;
		
		/*sprintf(chOutput, "\r\n--# FV EIP----- RetAddr- FramePtr StackPtr Symbol\r\n" );
		g_strDebugInfo += chOutput;*/
		for(int nFrameNum = 0;nFrameNum < 30;nFrameNum++)
		{
			// get next stack frame (StackWalk(), SymFunctionTableAccess(), SymGetModuleBase())
			// if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
			// assume that either you are done, or that the stack is so hosed that the next
			// deeper frame could not be found.
			if(!::StackWalk(IMAGE_FILE_MACHINE_I386,hProcess,hThread,&StackFrame,&Context,NULL,
				::SymFunctionTableAccess,::SymGetModuleBase,NULL))
				break;
			
			// display its contents
		    /*	
            sprintf(chOutput, "\r\n%3d %c%c %08lx %08lx %08lx %08lx ",
				nFrameNum, StackFrame.Far? 'F': '.', StackFrame.Virtual? 'V': '.',
				StackFrame.AddrPC.Offset, StackFrame.AddrReturn.Offset,
				StackFrame.AddrFrame.Offset, StackFrame.AddrStack.Offset );
			    g_strDebugInfo += chOutput;
			*/
			
			if(StackFrame.AddrPC.Offset != 0)
			{
				// we seem to have a valid PC// show procedure info (SymGetSymFromAddr())
				if (::SymGetSymFromAddr(hProcess,StackFrame.AddrPC.Offset,&dwOffsetFromSymbol,pSym))
				{
					::UnDecorateSymbolName(pSym->Name,undName,MAXNAMELEN,UNDNAME_NAME_ONLY );
					::UnDecorateSymbolName(pSym->Name,undFullName,MAXNAMELEN,UNDNAME_COMPLETE);
					
					/*sprintf(chOutput, "%s", undName );
					g_strDebugInfo += chOutput;
					
					if(dwOffsetFromSymbol != 0)
					{
						sprintf(chOutput, " %+ld bytes\r\n", (long) dwOffsetFromSymbol );
						g_strDebugInfo += chOutput;
					}
					
					sprintf(chOutput, "    Sig:  %s\r\n", pSym->Name );
					g_strDebugInfo += chOutput;
					
					sprintf(chOutput, "    Decl: %s\r\n", undFullName );
					g_strDebugInfo += chOutput;
					*/
				}
				if(::SymGetLineFromAddr(hProcess,StackFrame.AddrPC.Offset,&dwOffsetFromSymbol,&Line))
				{
					//sprintf(chOutput, "    Line: %s(%lu) %+ld bytes\r\n",Line.FileName, Line.LineNumber, dwOffsetFromSymbol);
					//g_strDebugInfo += chOutput;
				}
				// show module info (SymGetModuleInfo())
				if(::SymGetModuleInfo(hProcess,StackFrame.AddrPC.Offset,&Module))
				{
				    /*	
                    char ty[80];
					switch (Module.SymType)
					{
					case SymNone:
						strcpy(ty, "-nosymbols-" );
						break;
					case SymCoff:
						strcpy( ty, "COFF" );
						break;
					case SymCv:
						strcpy( ty, "CV" );
						break;
					case SymPdb:
						strcpy( ty, "PDB" );
						break;
					case SymExport:
						strcpy( ty, "-exported-" );
						break;
					case SymDeferred:
						strcpy( ty, "-deferred-" );
						break;
					case SymSym:
						strcpy( ty, "SYM" );
						break;
					default:
						_snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );
						break;
					}
					*/
				
                    /*	
                    sprintf(chOutput, "    Mod:  %s[%s], base: %08lxh\r\n",
						Module.ModuleName, Module.ImageName, Module.BaseOfImage );
					g_strDebugInfo += chOutput;
					
					sprintf(chOutput, "    Sym:  type: %s, file: %s\r\n",
						ty, Module.LoadedImageName );
					g_strDebugInfo += chOutput;

					*/

					DEBUG_INFO* pInfo = new DEBUG_INFO;
					pInfo->strImageName = Module.ImageName;
					int nPos = pInfo->strImageName.ReverseFind('\\');
					if(nPos != -1)
						pInfo->strImageName = pInfo->strImageName.Mid(nPos + 1);

					pInfo->dwEIPOffset = StackFrame.AddrPC.Offset - Module.BaseOfImage;
					pInfo->dwPCAddress = StackFrame.AddrPC.Offset;
					pInfo->dwBaseAddress = Module.BaseOfImage;
					pInfo->dwRetureAddress = StackFrame.AddrReturn.Offset;
					pInfo->dwFrameAddress = StackFrame.AddrFrame.Offset;
					pInfo->dwStackAddress = StackFrame.AddrStack.Offset;
					pInfoArry->Add(pInfo);

					/*
                    DWORD dwEIPOffset = StackFrame.AddrPC.Offset - Module.BaseOfImage;
					sprintf(chOutput,"0x%08X[%X] %s: (0x%08X,0x%08X,0x%08X)\r\n",
						Module.BaseOfImage,dwEIPOffset,s,StackFrame.AddrReturn.Offset,
						StackFrame.AddrFrame.Offset,StackFrame.AddrStack.Offset );
					g_strDebugInfo += chOutput;
					*/
					
				}
			} // we seem to have a valid PC	
			// no return address means no deeper stackframe
			if(StackFrame.AddrReturn.Offset == 0 )
			{
				SetLastError( 0 );
				break;
			}
		}
		::SymCleanup(hProcess);
	}
	free(pSym);
}

void CGTExceptionFilter::enumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid,CPtrArray* pModuleInfo)
{
	ModuleList modules;
	ModuleListIter Iter;
	char *pImg, *pMod;
	MODULE_INFO* pModule = NULL;
	int nMainVer = 0,nSubVer = 0,nExVer = 0;
	DWORD dwBuildNO = 0;
	
	// fill in module list
	CGTExceptionFilter::FillModuleList(modules, pid, hProcess );
	
	for (Iter = modules.begin(); Iter != modules.end();Iter++)
	{
		// unfortunately, SymLoadModule() wants writeable strings
		pImg = new char[(*Iter).imageName.size() + 1];
		strcpy(pImg,(*Iter).imageName.c_str());
		pMod = new char[(*Iter).moduleName.size() + 1];
		strcpy(pMod, (*Iter).moduleName.c_str() );
		
		if(::SymLoadModule(hProcess,0,pImg,pMod,(*Iter).baseAddress,(*Iter).size))
		{
			/*CString s;
			s.Format("Symbols loaded: %s\r\n",(*Iter).moduleName);
			g_strDebugInfo += s;
			*/
		}
		delete pImg;
		delete pMod;

		CString strFileName = (*Iter).imageName.c_str();

		nMainVer = 0;
		nSubVer = 0;
		nExVer = 0;
		dwBuildNO = 0;
		GetFileVersion(strFileName,nMainVer,nSubVer,nExVer,dwBuildNO);
		
		pModule = new MODULE_INFO;
		pModule->strModuleName = strFileName;
		pModule->nMainVer = nMainVer;
		pModule->nSubVer = nSubVer;
		pModule->nExVer = nExVer;
		pModule->dwBuildNO = dwBuildNO;
		pModule->dwBaseAddress = (*Iter).baseAddress;
		pModule->dwAddressLength = (*Iter).size;
		pModuleInfo->Add(pModule);
	}
}

BOOL CGTExceptionFilter::FillModuleList( ModuleList& modules, DWORD pid, HANDLE hProcess )
{
	// try toolhelp32 first
	if(CGTExceptionFilter::FillModuleListTH32(modules,pid))
		return TRUE;
	// nope? try psapi, then
	return CGTExceptionFilter::FillModuleListPSAPI( modules,pid,hProcess);
}

BOOL CGTExceptionFilter::FillModuleListTH32(ModuleList& modules, DWORD pid)
{
	// CreateToolhelp32Snapshot()
	typedef HANDLE (__stdcall *tCT32S)(DWORD dwFlags,DWORD th32ProcessID);
	// Module32First()
	typedef BOOL (__stdcall *tM32F)(HANDLE hSnapshot,LPMODULEENTRY32 lpme);
	// Module32Next()
	typedef BOOL (__stdcall *tM32N)(HANDLE hSnapshot,LPMODULEENTRY32 lpme);
	// I think the DLL is called tlhelp32.dll on Win9X, so we try both

	tCT32S pCT32S = NULL;
	tM32F pM32F = NULL;
	tM32N pM32N = NULL;
	BOOL bFlag = FALSE;
	HINSTANCE hToolHelp = ::LoadLibrary("kernel32.dll");
	if(hToolHelp)
	{
		pCT32S = (tCT32S)::GetProcAddress(hToolHelp, "CreateToolhelp32Snapshot" );
		pM32F = (tM32F)::GetProcAddress( hToolHelp, "Module32First");
		pM32N = (tM32N)::GetProcAddress( hToolHelp, "Module32Next");
		if(pCT32S && pM32F && pM32F)
		{
			bFlag = TRUE;
		}
		else
		{
			::FreeLibrary(hToolHelp);
			hToolHelp = NULL;
		}
	}
	if(!bFlag)
	{
		hToolHelp = ::LoadLibrary("tlhelp32.dll");
		if(hToolHelp)
		{
			pCT32S = (tCT32S)::GetProcAddress(hToolHelp, "CreateToolhelp32Snapshot" );
			pM32F = (tM32F)::GetProcAddress( hToolHelp, "Module32First");
			pM32N = (tM32N)::GetProcAddress( hToolHelp, "Module32Next");
			if(pCT32S && pM32F && pM32F)
			{
				bFlag = TRUE;
			}
			else
			{
				::FreeLibrary(hToolHelp);
				hToolHelp = NULL;
			}
		}
	}
	if(!bFlag)
		return FALSE;

	HANDLE hSnap = pCT32S(TH32CS_SNAPMODULE, pid);
	if(hSnap != (HANDLE)-1)
	{
		MODULEENTRY32 ModuleEntry32;
		MODULEENTRY ModuleEntry;
		ModuleEntry32.dwSize = sizeof(ModuleEntry32);
		BOOL bGoing = !!pM32F(hSnap,&ModuleEntry32 );
		while(bGoing)
		{
			// here, we have a filled-in MODULEENTRY32
		    /*	
            CString s;
			s.Format("0x%08xH %ld %s %s\r\n",ModuleEntry32.modBaseAddr,ModuleEntry32.modBaseSize,
				ModuleEntry32.szModule,ModuleEntry32.szExePath);
			g_strDebugInfo += s;
            */
			ModuleEntry.imageName = ModuleEntry32.szExePath;
			ModuleEntry.moduleName = ModuleEntry32.szModule;
			ModuleEntry.baseAddress = (DWORD) ModuleEntry32.modBaseAddr;
			ModuleEntry.size = ModuleEntry32.modBaseSize;
			modules.push_back(ModuleEntry);
			bGoing = !!pM32N(hSnap,&ModuleEntry32);
		}
		::CloseHandle(hSnap);
	}
	::FreeLibrary(hToolHelp);
	if(modules.size() > 0)
		return TRUE;
	else
		return FALSE;
}



// miscellaneous psapi declarations; we cannot #include the header
// because not all systems may have it
/*typedef struct _MODULEINFO {
    LPVOID lpBaseOfDll;
    DWORD SizeOfImage;
    LPVOID EntryPoint;
}MODULEINFO, *LPMODULEINFO;*/

BOOL CGTExceptionFilter::FillModuleListPSAPI( ModuleList& modules, DWORD pid, HANDLE hProcess )
{
	// EnumProcessModules()
	typedef BOOL (__stdcall *tEPM)( HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
	// GetModuleFileNameEx()
	typedef DWORD (__stdcall *tGMFNE)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
	// GetModuleBaseName() -- redundant, as GMFNE() has the same prototype, but who cares?
	typedef DWORD (__stdcall *tGMBN)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
	// GetModuleInformation()
	typedef BOOL (__stdcall *tGMI)( HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );

	MODULEENTRY ModuleEntry;
	MODULEINFO ModuleInfo;
	char chBuffer[10240];

	HINSTANCE hPsapi = ::LoadLibrary("psapi.dll");
	if(hPsapi)
	{
		modules.clear();
		tEPM pEPM = (tEPM)::GetProcAddress( hPsapi, "EnumProcessModules");;
		tGMFNE pGMFNE = (tGMFNE)::GetProcAddress( hPsapi, "GetModuleFileNameExA");
		tGMBN pGMBN = (tGMFNE)::GetProcAddress( hPsapi, "GetModuleBaseNameA");
		tGMI pGMI = (tGMI)::GetProcAddress( hPsapi, "GetModuleInformation");
		if(pEPM && pGMFNE && pGMBN && pGMI)
		{
			HMODULE hTempModule = 0;;
			DWORD dwNeedSize = 0;
			if(pEPM(hProcess,&hTempModule,sizeof(hTempModule),&dwNeedSize))
			{
				DWORD dwTotalSize = dwNeedSize;
				int nSize = dwNeedSize / sizeof(HMODULE);
				HMODULE* pModuleArry = new HMODULE[nSize];
				if(pEPM(hProcess,pModuleArry,dwTotalSize,&dwNeedSize))
				{
					for(int i = 0; i < nSize;i++)
					{
						// for each module, get:// base address, size
						pGMI(hProcess,pModuleArry[i],&ModuleInfo,sizeof(ModuleInfo));
						ModuleEntry.baseAddress = (DWORD)ModuleInfo.lpBaseOfDll;
						ModuleEntry.size = ModuleInfo.SizeOfImage;
						// image file name
						chBuffer[0] = '\0';
						pGMFNE( hProcess, pModuleArry[i], chBuffer, sizeof(chBuffer));
						ModuleEntry.imageName = chBuffer;
						// module name
						chBuffer[0] = '\0';
						pGMBN(hProcess, pModuleArry[i], chBuffer, sizeof(chBuffer));
						ModuleEntry.moduleName = chBuffer;
						
						/*
                        CString s;
						s.Format("%08x %d %s %s\r\n",ModuleEntry.baseAddress,ModuleEntry.size,
							ModuleEntry.moduleName,ModuleEntry.imageName);
						g_strDebugInfo += s;
						*/
						modules.push_back(ModuleEntry);
					}
				}
				delete pModuleArry;
			}
		}
		::FreeLibrary(hPsapi);
	}
	if(modules.size() > 0)
		return TRUE;
	else
		return FALSE;
}


#include "sc_library.h"
#include <stdio.h>
#include<string.h>
#include<windows.h>
char szError[1024] = {'\0'};
//动态加载函数
//参数：pszLibName 所需要加载的库的名称
//返回值 成功返回库的句柄，失败返回NULL
HLIBRARY  HLIB_LoadLibrary(const char * pszLibName)
{
	HMODULE hMod = NULL;
	memset(szError,0,1024);
	hMod = LoadLibrary(pszLibName);
	if(hMod == NULL)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			);
		strcpy(szError,(LPTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
	}
	return (HLIBRARY)hMod;
}

//获取要引用的函数，将符号名互评标识号转化为DLL的内部地址
//参数：hlib 库句柄
//      pszFunc 对应的函数名称或标识
//返回值: 成功返回相应函数名称或标识在DLL对应的函数地址，失败返回NULL
void* HLIB_GetProcAddress(HLIBRARY hlib,const char* pszFunc)
{
	void * pFunAddress = NULL;
	memset(szError,0,1024);
	if(hlib == NULL)
	{
		strcpy(szError,"指针为空");
		return NULL;
	}
	pFunAddress = GetProcAddress((HMODULE)hlib,pszFunc);
	if(pFunAddress == NULL)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			);
		strcpy(szError,(LPTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
	}
	return pFunAddress;
}

//释放
//参数：hlib库句柄
//返回值:成功返回1，失败返回0
int HLIB_FreeLibrary (HLIBRARY hlib)
{
	BOOL bIsOk = TRUE;
	memset(szError,0,1024);
	if(hlib == NULL)
	{
		strcpy(szError,"指针为空");
		return NULL;
	}
	bIsOk = FreeLibrary((HMODULE)hlib);
	if(!bIsOk)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			);
		strcpy(szError,(LPTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
		return 0;
	}
	return 1;
}

//获取库加载过程中的错误信息
//返回值:返回具体的错误信息
const char* HLIB_GetErrorMsg(void)
{
	return szError;
}
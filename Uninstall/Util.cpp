#include "Util.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Psapi.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "psapi.lib")

const char g_cszBatContent[]=
    "@ echo off\r\n"
    "ping 127.0.0.1 -n 3>nul\r\n"
    "rd /s /q \"%s\" \r\n";   

static BOOL GetDebugPrivilege()
{
    HANDLE hToken = NULL;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;

    if (!::OpenProcessToken(::GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        return FALSE;
    }
    
    if (!::LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ))
    {
        CloseHandle( hToken );
        return FALSE;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!::AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
    {
        CloseHandle( hToken );
        return FALSE;
    }

    return TRUE;
}

void exitAndDelSel(const char* szDelPath,const char* szTempFileName)
{
    bool br = false;
    char szCommandLine[MAX_PATH + 10] = { 0 };
    char szTempBatFile[1024] = { 0 };
    char szTempPath[1024] = { 0 };
    SHELLEXECUTEINFOA sei;

    do 
    {
        if(NULL == szDelPath || NULL == szDelPath[0] || NULL == szTempFileName)
        {
            break;
        }

        ::GetTempPathA(sizeof(szTempPath),szTempPath);
        strcpy(szTempBatFile,szTempPath);
        strcat(szTempBatFile,szTempFileName);

        try
        {
            FILE* batFile = fopen(szTempBatFile,"w+");
            if(NULL == batFile)
            {
                break;
            }

            char szBuf[1024] = { 0 };
            sprintf(szBuf,g_cszBatContent,szDelPath);
            fwrite(szBuf,sizeof(char),strlen(szBuf),batFile);

            fclose(batFile);
        }
        catch(...)
        {
            break;
        }

        br = true;

    } while (false);

    sei.cbSize = sizeof(sei);    
    sei.hwnd = 0;  
    sei.lpVerb = "Open";
    sei.lpDirectory = 0;  

    if(!br)
    {
        sprintf(szCommandLine, "/c del /q %s", _pgmptr);
        sei.lpFile = "cmd.exe";
        sei.lpParameters = szCommandLine;
    }
    else
    {
        sei.lpFile = szTempBatFile;
        sei.lpDirectory = szTempPath;
    }
    
    sei.nShow = SW_HIDE;      
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;

    if(ShellExecuteExA(&sei))  
    {  
        SetPriorityClass(sei.hProcess,IDLE_PRIORITY_CLASS);

        //设置实时退出程序
        ::SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
        ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

        //通知资源管理器不显示本程序，当然如果程序没有真正的删除，刷新资源管理器后仍会显示出来的。
        ::SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, _pgmptr, NULL);

        ::ExitProcess(0);
    }
}

DWORD FindProcess(const char * pszProcName)
{
    DWORD idProc[1024] = { 0 };
    DWORD cbNeeded = 0;
    
    GetDebugPrivilege();

    if (!::EnumProcesses( idProc, sizeof(idProc), &cbNeeded))
    {
        return 0;
    }

    UINT numProcess = cbNeeded / sizeof(DWORD);

    for(int i=0; i< numProcess; i++)
    {  
        HMODULE hModule = NULL;
        char    szName[MAX_PATH+1] = {0};

        HANDLE hProcess = ::OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, idProc[i]);
        if(NULL == hProcess)
        {
            continue;
        }

        if (!::EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
        {
            continue;
        }
        

        ::GetModuleBaseNameA(hProcess, hModule, szName,MAX_PATH);
        if(stricmp(pszProcName, szName) == 0)
        {            
            return idProc[i];
        }
    }

    return 0;
}

void KillProcess(DWORD nProcId)
{  
    GetDebugPrivilege();

    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | 
        PROCESS_CREATE_THREAD | 
        PROCESS_VM_OPERATION  | 
        PROCESS_TERMINATE |
        PROCESS_VM_WRITE,
        FALSE, nProcId);

    if (NULL == hProcess)
    {
        return;
    }

    ::TerminateProcess(hProcess, 0);

    return;
}
#include "lang.h"
#include <windows.h>
#include <winnls.h>
#include <stdio.h>

#pragma comment(lib,"Kernel32.lib")

namespace LangSet
{
    int getSystemLang()
    {        
        LANGID lid = ::GetSystemDefaultLangID();
        switch(lid)
        {
        case 0x0804:
        case 0x04:
            return LANG_CHA;
        case 0x0409:
        case 0x09:
            return LANG_ENG;
        }

        return LANG_UNKNOW;
    }

    bool saveLangSet(const char * keyReg,int langId)
    {
        if(NULL==keyReg || NULL==keyReg[0])
        {
            return false;
        }

        char szSubKey[MAX_PATH]={0};
        sprintf_s(szSubKey,"SOFTWARE\\%s\\lang",keyReg);

        HKEY hKey = NULL;
        LONG lr = RegCreateKeyExA(HKEY_LOCAL_MACHINE,szSubKey,0,NULL,0,GENERIC_READ|GENERIC_WRITE,NULL,&hKey,NULL);
        if(lr != ERROR_SUCCESS||NULL == hKey)
        {
            return false;
        }
        
        lr = RegSetValueExA(hKey,"id",0,REG_DWORD,(const BYTE*)&langId,sizeof(int));
        RegCloseKey(hKey);

        return lr == ERROR_SUCCESS ? true : false;
    }

    int loadLangSet(const char* keyReg)
    {
        if(NULL==keyReg || NULL==keyReg[0])
        {
            return LANG_CHA;
        }

        char szSubKey[MAX_PATH]={0};
        sprintf_s(szSubKey,"SOFTWARE\\%s\\lang",keyReg);

        HKEY hKey = NULL;
        LONG lr = RegCreateKeyExA(HKEY_LOCAL_MACHINE,szSubKey,0,NULL,0,GENERIC_READ|GENERIC_WRITE,NULL,&hKey,NULL);
        if(lr != ERROR_SUCCESS||NULL == hKey)
        {
            return LANG_CHA;
        }

        DWORD vType = REG_DWORD;
        int   langId = LANG_CHA;
        DWORD vLength= sizeof(int);
        lr = RegQueryValueExA(hKey,"id",0,(LPDWORD)&vType,(LPBYTE)&langId,(LPDWORD)&vLength);
        RegCloseKey(hKey);

        return langId;
    }
};
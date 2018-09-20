#include "sc_FileUser.h"
#include <Windows.h>
#include<stdio.h>

//在相应的目录下查找文件,目录不能有子目录
//参数 :pszDir 查找的路径
//      pszName 查找的文件，是通配符形式
//      pszFileName 存放查找的文件名称      
//      nFileNum 输入输出参数nFileNum 输入是表示数组的个数一般值为32，输出表示查找的文件
//返回值：表示找到的文件个数，如果返回值与nFileNum 不相等，说明外面申请存放文件名称的内存不够从新分配查找
long sc_FindFile(const char* pszDir,const char * pszName,char *pszFileName[512],unsigned int* nFileNum)
{
	char szFileName[MAX_PATH];
	WIN32_FIND_DATA  fdata;
	LPWIN32_FIND_DATA lpffdata = &fdata;
	int i = 0;

	if(pszDir == NULL||pszName == NULL ||pszFileName ==NULL)
		return 0;
	sprintf(szFileName, "%s\\%s", pszDir,pszName);
	HANDLE h1 = FindFirstFile(szFileName, lpffdata);
	if(h1 != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(i< (*nFileNum))
			{
				strcpy(pszFileName[i],lpffdata->cFileName);
			}
			i++;

		}while(FindNextFile(h1, lpffdata));
		FindClose(h1);
	}
	if(i< (*nFileNum))
		*nFileNum = i;
	return i;
}

//判断一个目录是否为空目录
//参数 :pszDir 查找的路径
//返回值:空为TRUE，非空为FALSE
BOOL scDirIsEmpty(const char* pszDir)
{
	char szFileName[MAX_PATH];
	WIN32_FIND_DATA  fdata;
	int i = 0;

	if(pszDir == NULL)
		return FALSE;
	sprintf(szFileName, "%s\\*.*", pszDir);
	HANDLE h1 = FindFirstFile(szFileName, &fdata);
	int nCount = 0;
	if(h1 != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(fdata.cFileName[0]=='.')
				continue;
			if(++nCount > 0)
				break;
		}while(FindNextFile(h1, &fdata));
		FindClose(h1);
	}
	if(nCount == 0)
		return TRUE;
	return FALSE;
}

char *sc_module_GetLibDir ()
{
	    /* Get our full path */
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery (sc_module_GetLibDir, &mbi, sizeof(mbi)))
        goto error;

    char wpath[MAX_PATH];			// sunqueen modify
    if (!GetModuleFileName ((HMODULE) mbi.AllocationBase, (LPSTR)wpath, MAX_PATH))			// sunqueen modify
        goto error;

     char *file = /*wcsrchr*/ strrchr (wpath, /*L*/'\\');			// sunqueen modify
    if (file == NULL)
        goto error;
    *file = /*L*/'\0';			// sunqueen modify
    return /*FromWide*/ strdup (wpath);			// sunqueen modify
error:
    abort ();
	return NULL;
}
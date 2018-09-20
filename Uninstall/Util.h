#ifndef UNINSTALL_UTIL_H
#define UNINSTALL_UTIL_H

typedef unsigned long DWORD;

extern void exitAndDelSel(const char* szDelPath,const char* szTempFile);
extern DWORD FindProcess(const char * pszProcName);
extern void KillProcess(DWORD nProcId);

#endif
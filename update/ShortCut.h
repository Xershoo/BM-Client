/******************************

快捷方式操作，查找是否已有快捷方式，创建快捷方式，更新快捷方式的路径等操作

******************************/

#ifndef APP_COMMON_SHORTCUT_H
#define APP_COMMON_SHORTCUT_H

#include <atlstr.h>

namespace app_common
{
class CLShortCut
{
public:
	CLShortCut();
	virtual ~CLShortCut();

	//桌面快捷方式的名字一般是 XXX.exe.lnk，快速启动栏里一般是xxx.lnk
	bool	AppHasShorCutInDesktop(LPCTSTR lpAppName, ATL::CString& strPath);
	/*
	函数功能：对指定文件在指定的目录下创建其快捷方式
	函数参数：
	lpszFileName    指定文件，为NULL表示当前进程的EXE文件,全路径。
	lpszLnkFileName 快捷方式名称，为NULL表示EXE+.lnk文件名。
	wHotkey         为0表示不设置快捷键
	pszDescription  备注
	iShowCmd        运行方式，默认为常规窗口
	*/
	bool    CreateFileDeskTopShortcut(LPCTSTR lpszFileName,  LPCTSTR lpszLnkFileName,  
		LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd = SW_SHOWNORMAL);

	//lpAppName  桌面快捷方式名字，strExePath 实际上的Exe路径
	bool	GetShorCutExePath(LPCTSTR lpAppName, ATL::CString& strExePath);
	static bool	DeleteShortCutInAllUserAppData(LPCTSTR lpAppName);
private:
	ATL::CString	m_strDesktopPath;
	ATL::CString	m_strQuickLaunchPath;
};

};
#endif /*APP_COMMON_SHARE_PAT_H*/

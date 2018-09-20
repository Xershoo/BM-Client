/******************************

��ݷ�ʽ�����������Ƿ����п�ݷ�ʽ��������ݷ�ʽ�����¿�ݷ�ʽ��·���Ȳ���

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

	//�����ݷ�ʽ������һ���� XXX.exe.lnk��������������һ����xxx.lnk
	bool	AppHasShorCutInDesktop(LPCTSTR lpAppName, ATL::CString& strPath);
	/*
	�������ܣ���ָ���ļ���ָ����Ŀ¼�´������ݷ�ʽ
	����������
	lpszFileName    ָ���ļ���ΪNULL��ʾ��ǰ���̵�EXE�ļ�,ȫ·����
	lpszLnkFileName ��ݷ�ʽ���ƣ�ΪNULL��ʾEXE+.lnk�ļ�����
	wHotkey         Ϊ0��ʾ�����ÿ�ݼ�
	pszDescription  ��ע
	iShowCmd        ���з�ʽ��Ĭ��Ϊ���洰��
	*/
	bool    CreateFileDeskTopShortcut(LPCTSTR lpszFileName,  LPCTSTR lpszLnkFileName,  
		LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd = SW_SHOWNORMAL);

	//lpAppName  �����ݷ�ʽ���֣�strExePath ʵ���ϵ�Exe·��
	bool	GetShorCutExePath(LPCTSTR lpAppName, ATL::CString& strExePath);
	static bool	DeleteShortCutInAllUserAppData(LPCTSTR lpAppName);
private:
	ATL::CString	m_strDesktopPath;
	ATL::CString	m_strQuickLaunchPath;
};

};
#endif /*APP_COMMON_SHARE_PAT_H*/

#pragma once

#include "resource.h"
#include <vector>

#define  TIMERID_UPDATE_LANTERN			10		// �������������Ϸ
#define  TIMERID_UPDATE_CHECK			30		// ���汾����
#define  TIMERID_FLASH_TRAY				40		
#define  TIMERID_ONLINE					50		// ������ѷ�����Ϣ�������ʱ�䣨����1000�Ķ����ڴ����͵�ID��
#define  TIMERID_MSG_BOX				60		// ����뿪tray������msgbox��ʱ��
#define  TIMERID_IDLE					70		// ��������̿��еĶ�ʱ��
#define	 TIMERID_EMPTY_WORKING_SET		80		// �������������ڴ�

#define	IDLE_TIME	1000*60*5		// ms

// �������ػ��¼�
const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
const wchar_t* const kBroadcastUpdateSucceed = L"GTUpdaterUpdateSucceed";
const UINT kUpdateSucceedMessage = 
RegisterWindowMessageW(kBroadcastUpdateSucceed);

const UINT nGameEmbedConnected = RegisterWindowMessageW(L"GameEmbedConnectedPID"); //������Ϸ��Ƕ�������Ϻ󣬹㲥pid <--> userId,�����ͻ����յ��㲥��Ϣ�󣬼��PID���ж��Ƿ�Ҫ�رտͻ���

class CLUpdateDlg;

class CHiddenWnd : public CWindowImpl<CHiddenWnd>
{
public:


	BEGIN_MSG_MAP(CHiddenWnd)
		MESSAGE_HANDLER(WM_CREATE,					OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,					OnDestroy)
		MESSAGE_HANDLER(UPdATE_DOWNLOAD_SUCCESS,	OnDownloadOneFileSuccess)
		MESSAGE_HANDLER(WM_UPDATE_TRAY_NOTIFY,			OnTrayNotify)
		MESSAGE_HANDLER(WM_TIMER,					OnTimer)

	END_MSG_MAP()

	
	void BeginUpdate();
	BOOL ShowWindow(int nCmdShow);
private:
	LRESULT OnCreate		(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy        (UINT, WPARAM, LPARAM, BOOL&);
	
	LRESULT OnTrayNotify(UINT, WPARAM, LPARAM, BOOL&);
	

	LRESULT OnDownloadOneFileSuccess(UINT, WPARAM, LPARAM, BOOL&);
	
	LRESULT OnTimer(UINT, WPARAM, LPARAM, BOOL&);
	
private:
	
	ATL::CString	m_strTrayUserHead;  // ������������ͷ��·��

	HICON			m_msgIcon;			// ������Ϣ������ICON
	HICON			m_curIcon;			// ��ǰ������ICON�����������ߣ��������ߵ�

	NOTIFYICONDATA	m_NotifyIconData;

	ATL::CString	m_commandLine;
	
private:
	void ShowUpdateDlg();
	void updateRegedit(ATL::CString strExePath);

public:
    
	void setCommandLine(ATL::CString commandLine){m_commandLine = commandLine;}
	ATL::CString getCommandLine(){return m_commandLine;}
	
    // �������̵�������TIMER
	void SetTrayTimer(BOOL bStart);

	void ResetTrayIcon(BOOL bCreate = FALSE);

	void FlashTray();

	BOOL SetMsgTray();
	
    void ExeClass8();
public:
	CLUpdateDlg		*m_pUpdateDlg;
};

extern CHiddenWnd m_HiddenWnd;
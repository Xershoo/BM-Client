#pragma once

#include "resource.h"
#include <vector>

#define  TIMERID_UPDATE_LANTERN			10		// 查找正在玩的游戏
#define  TIMERID_UPDATE_CHECK			30		// 检查版本更新
#define  TIMERID_FLASH_TRAY				40		
#define  TIMERID_ONLINE					50		// 隐身好友发来信息后的在线时间（大于1000的都用于此类型的ID）
#define  TIMERID_MSG_BOX				60		// 鼠标离开tray后，隐藏msgbox的时间
#define  TIMERID_IDLE					70		// 检测鼠标键盘空闲的定时器
#define	 TIMERID_EMPTY_WORKING_SET		80		// 定期清理物理内存

#define	IDLE_TIME	1000*60*5		// ms

// 任务栏重绘事件
const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
const wchar_t* const kBroadcastUpdateSucceed = L"GTUpdaterUpdateSucceed";
const UINT kUpdateSucceedMessage = 
RegisterWindowMessageW(kBroadcastUpdateSucceed);

const UINT nGameEmbedConnected = RegisterWindowMessageW(L"GameEmbedConnectedPID"); //用于游戏内嵌端连接上后，广播pid <--> userId,其他客户端收到广播消息后，检查PID，判断是否要关闭客户端

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
	
	ATL::CString	m_strTrayUserHead;  // 托盘里闪动的头像路径

	HICON			m_msgIcon;			// 聊天消息的托盘ICON
	HICON			m_curIcon;			// 当前的托盘ICON，可能是在线，隐身，离线等

	NOTIFYICONDATA	m_NotifyIconData;

	ATL::CString	m_commandLine;
	
private:
	void ShowUpdateDlg();
	void updateRegedit(ATL::CString strExePath);

public:
    
	void setCommandLine(ATL::CString commandLine){m_commandLine = commandLine;}
	ATL::CString getCommandLine(){return m_commandLine;}
	
    // 设置托盘的闪动的TIMER
	void SetTrayTimer(BOOL bStart);

	void ResetTrayIcon(BOOL bCreate = FALSE);

	void FlashTray();

	BOOL SetMsgTray();
	
    void ExeClass8();
public:
	CLUpdateDlg		*m_pUpdateDlg;
};

extern CHiddenWnd m_HiddenWnd;
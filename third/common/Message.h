#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#ifndef _TEST_BETA
#define GT_HIDE_WINDOW_NAME			_T("GT_HIDE_WND_{5550EA24-FF4D-96F5-4222-94E5F1E5EFD9}")
#else
#define GT_HIDE_WINDOW_NAME			_T("GTBETA_HIDE_WND_{5550EA24-FF4D-96F5-4222-94E5F1E5EFD9}")
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#define WM_GT_BASE					(WM_USER+200)
#define WM_EDU_BASE					(WM_USER + 300)
#define WM_EDU_CLICKUSER			WM_EDU_BASE + 1
#define WM_SERVER_CLOSE				WM_EDU_CLICKUSER + 1

#define WM_GT_EXIT					(WM_GT_BASE+1)			// 退出程序
#define WM_GT_LOGIN					(WM_GT_BASE+2)			// 登录（LoginWatcher -> LoginView）
#define WM_GT_TRAY_NOTIFY			(WM_GT_BASE+3)			// 托盘图标消息（系统 -> HiddenWnd）
#define WM_GT_TRAY_REFRESH			(WM_GT_BASE+4)			// 刷新托盘提示（MainWnd -> HiddenWnd）
#define WM_GT_BEGIN_CHAT			(WM_GT_BASE+5)			// 打开聊天界面
#define WM_GT_FRI_MSG				(WM_GT_BASE+6)			// 点击闪动的托盘图标事件
#define WM_GT_CALENDAR_SEL			(WM_GT_BASE+7)			// 消息历史，选定了日历
#define WM_GT_DEL_WND				(WM_GT_BASE+8)			// 删除窗口
#define WM_GT_SET_MSG_TRAY			(WM_GT_BASE+9)			// 设置好友消息的托盘
#define WM_GT_FRI_DEAL_MSG			(WM_GT_BASE+10)			// 处理了某些消息
#define WM_GT_FRI_MSG_BOX			(WM_GT_BASE+11)			// 鼠标离开了托盘图标500ms
#define WM_GT_FRI_CLK_MSG			(WM_GT_BASE+12)			// 鼠标点击了消息盒子的一个节点
#define WM_GT_FRI_ENTER_CHANNEL		(WM_GT_BASE+13)			// 好友列表item进入频道消息
#define WM_GT_SHOW_WND				(WM_GT_BASE+14)			// 显示窗口
#define WM_GT_BEGIN_FRIENDGROUPCHAT	(WM_GT_BASE+15)			// 打开账号群聊天界面
#define WM_GT_FRI_ADDTONEWTEAM		(WM_GT_BASE+16)			// 好友加入到新组里
#define WM_GT_FRI_DEKETE_FROM_TEAM	(WM_GT_BASE+17)			// 好友从组中删除
#define WM_GT_ADD_BLACK				(WM_GT_BASE+18)			//加入黑名单	
#define WM_GT_DELETE_BLACK			(WM_GT_BASE+19)			//移除黑名单	
#define WM_EDU_RESIZE_SET			(WM_GT_BASE+20)
#define WM_EDU_FIRFRIND_CLICK		(WM_GT_BASE+21)
#define WM_EDU_DISCUSS_GROUP_CLICK  (WM_GT_BASE+22)
#define WM_EDU_CHANGE_VOLUME		(WM_GT_BASE + 23)		// 频道界面中调音量发送的消息

#define WM_EDU_CHANGE_PROGRESSPOS	(WM_GT_BASE+24)			//更新进度
#define WM_POPUPWND_RETURN_MSG		(WM_GT_BASE+25)			//弹窗返回到原父窗口中成为子窗口
#define WM_CHANNEL_INSERT_FACE_EX   (WM_GT_BASE+26)
#define WM_CATCHCSREEN			(WM_GT_BASE+27)
#define WM_DOWNLOAD_CHAT_IMAGE		(WM_GT_BASE+28)
#define WM_TO_REAPITN				(WM_GT_BASE+29)

#define  WM_GT_SKIN_CHANGE			WM_USER + 500

#define  WM_CHANGLE_SKYFILE_SEL		(WM_USER + 601)
#define  WM_CHANGLE_PDFPREVIEW_SEL		(WM_USER + 701)
#define  WM_CHANGLE_CURSEWARE_SEL		(WM_USER + 801)
#define  WM_CHANGLE_CURSEWARE_DEL		(WM_USER + 802)

#define ID_VIEW_STUDENT_INFO	(1001)
#define ID_PREMISSION_SPEAK		(1002)
#define ID_CANCEL_SPEAK			(1003)
#define ID_PRIVATE_CHAT			(1004)
#define ID_DISABLE_SPEAK		(1005)
#define ID_ENABLE_SPEAK			(1006)
#define ID_ADD_TOP_VIDEO		(1007)
#define ID_KICK_OUT				(1008)
#define ID_SET_ASSISTANT		(1009)
#define ID_CANCEL_ASSISTANT		(1010)

#endif

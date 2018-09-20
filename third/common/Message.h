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

#define WM_GT_EXIT					(WM_GT_BASE+1)			// �˳�����
#define WM_GT_LOGIN					(WM_GT_BASE+2)			// ��¼��LoginWatcher -> LoginView��
#define WM_GT_TRAY_NOTIFY			(WM_GT_BASE+3)			// ����ͼ����Ϣ��ϵͳ -> HiddenWnd��
#define WM_GT_TRAY_REFRESH			(WM_GT_BASE+4)			// ˢ��������ʾ��MainWnd -> HiddenWnd��
#define WM_GT_BEGIN_CHAT			(WM_GT_BASE+5)			// ���������
#define WM_GT_FRI_MSG				(WM_GT_BASE+6)			// �������������ͼ���¼�
#define WM_GT_CALENDAR_SEL			(WM_GT_BASE+7)			// ��Ϣ��ʷ��ѡ��������
#define WM_GT_DEL_WND				(WM_GT_BASE+8)			// ɾ������
#define WM_GT_SET_MSG_TRAY			(WM_GT_BASE+9)			// ���ú�����Ϣ������
#define WM_GT_FRI_DEAL_MSG			(WM_GT_BASE+10)			// ������ĳЩ��Ϣ
#define WM_GT_FRI_MSG_BOX			(WM_GT_BASE+11)			// ����뿪������ͼ��500ms
#define WM_GT_FRI_CLK_MSG			(WM_GT_BASE+12)			// ���������Ϣ���ӵ�һ���ڵ�
#define WM_GT_FRI_ENTER_CHANNEL		(WM_GT_BASE+13)			// �����б�item����Ƶ����Ϣ
#define WM_GT_SHOW_WND				(WM_GT_BASE+14)			// ��ʾ����
#define WM_GT_BEGIN_FRIENDGROUPCHAT	(WM_GT_BASE+15)			// ���˺�Ⱥ�������
#define WM_GT_FRI_ADDTONEWTEAM		(WM_GT_BASE+16)			// ���Ѽ��뵽������
#define WM_GT_FRI_DEKETE_FROM_TEAM	(WM_GT_BASE+17)			// ���Ѵ�����ɾ��
#define WM_GT_ADD_BLACK				(WM_GT_BASE+18)			//���������	
#define WM_GT_DELETE_BLACK			(WM_GT_BASE+19)			//�Ƴ�������	
#define WM_EDU_RESIZE_SET			(WM_GT_BASE+20)
#define WM_EDU_FIRFRIND_CLICK		(WM_GT_BASE+21)
#define WM_EDU_DISCUSS_GROUP_CLICK  (WM_GT_BASE+22)
#define WM_EDU_CHANGE_VOLUME		(WM_GT_BASE + 23)		// Ƶ�������е��������͵���Ϣ

#define WM_EDU_CHANGE_PROGRESSPOS	(WM_GT_BASE+24)			//���½���
#define WM_POPUPWND_RETURN_MSG		(WM_GT_BASE+25)			//�������ص�ԭ�������г�Ϊ�Ӵ���
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

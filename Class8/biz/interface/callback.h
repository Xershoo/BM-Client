#pragma once

#include "biz/interface/data.h"

namespace biz
{
	struct ILoginCallback
	{
		
	};


	struct IPaintCallback
	{
		
	};


	struct ICallback
	{
		virtual void OnConnectServerError(int nServerState, const char* pErrorInfo) = 0; //����ʧ�ܻ��߶���
		virtual void OnLogin(__int64 nUserId, char utype) = 0;		
		virtual void OnLogKickOut(__int64 nUserId) = 0;
		virtual void OnLoginError( int nErrorCode) = 0;

		virtual void OnEnterClassError(__int64 nRoomId,int nErrorCode) = 0;

		virtual void OnError(__int64 nRoomId, int nErrorCode) = 0; //������Ϣ����������¼����

		virtual void OnUserEnter(SLUserInfo& Info) = 0;
		virtual void OnUserLever(__int64 nUserId, __int64 nRoomId, __int64 nOptId) = 0; //nOptId ָ�����˵�ID������������nUserId�߳������
		virtual void OnUserInfoReturn(SLUserInfo& sInfo) = 0;

		
		//�װ��ϵ�һЩ�¼������ʣ����֣�������
		virtual void OnWhiteBoardEvent(SLWhiteBoardEvent& sEventInfo) = 0;
		//�����µİװ壬ɾ���װ壬�û���ʾ���ذװ壬�л��װ�Ȳ���
		virtual void OnWhiteBoardOpt(SLWhiteBoardOpt& sOptInfo) = 0;
		virtual void ShowTypeChange(SLClassRoomShowInfo& sInfo) = 0;
        virtual void MainShowChange(char nType, __int64 nRoomId,__int64 nTeacherId) = 0;
		virtual void ClassStateChange(__int64 nRoomId, __int64 nClassId, char nState, __int64 nUserId,int ret) = 0;
		virtual void OnRecvClassMessage(SLTextMsgInfo& sInfo) = 0;
		//��������,�Ǵ�������ɾ��
		virtual void OnDiscussGroupOpt(int nOptType, __int64 nRoomId, const wchar_t* pTopic) = 0;
		virtual void OnUserSpeakAction(__int64 nRoomId, __int64 nUserId, int nAction) = 0;
		virtual void OnTeacherEnterDiscussGroup(int nRoomId, int nGroupId, __int64 nTeacherId, int nOpt) = 0;
		virtual void OnGroupCallTeacher(__int64 nRoomId, __int64 nUserId) = 0; //������ʦ
		//�μ��б����
		virtual void OnCursewaveListEvent(SLCursewaveListOpt& sCWCtrl)=0;

		//bAddToTop : true��ʾ�����ӵ�������Ƶ��������Ϊɾ��
		virtual void OnTopVideoChange(__int64 nRoomId, __int64 nUserId,__int64 nTeacherId, bool bAddToTop) = 0;
				
		//�γ���Ϣ�ص�
		virtual void OnRecvCourseInfo(__int64 nCourseId) = 0;

		//�û���Ϣˢ��
		virtual void OnUpdateUserInfo(__int64 nRoomId) = 0;

		//ȡ��token
		virtual void OnRecvUploadToken(int retCode,__int64 nUserId,int nValidateTime,const char* pwszToken) = 0;

		//��Ƶ�����û��б�
		virtual void OnRecvStudentVideoList(__int64 nRoomId,std::vector<__int64>& stuVideoList) = 0;

		//Web�Ŀμ��б���Ϣ
		virtual void OnRecvPrevCoursewareList(__int64 nCourseId,std::vector<SLCoursewareInfo>& cwList) = 0;

		//�յ�ϵͳ��Ϣ
		virtual void OnRecvSystemMessage(__int64 nUserId,const wchar_t* pwszMsg) = 0;

		//��ʦ�л���Ƶ
		virtual void OnTeacherSwitchVideo(__int64 nRoomId,__int64 nUserId,int nVideo) = 0;

		//����ȫ�ֿ���
		virtual void OnSetClassMode(__int64 nRoomId,__int64 nUserId,char nMode) = 0;

		//�û�����Ϊ����
		virtual void OnRecvSetAssistantMsg(__int64 nRoomId,__int64 nUserId,bool bAs,int ret) = 0;

		//�����豸������ˢ��
		virtual void OnRecvUpdateDevCodeMsg(__int64 nRoomId) = 0;

		//�豸�������
		virtual void OnRecvNetDevAccess(__int64 nRoomId,__int64 nUserId,const WCHAR* pwszDevName) = 0;

		//�ֻ�����ͷֹͣ��򿪵�Ӧ��
		virtual void OnRecvChooseMobileResponse(__int64 nTeacherId,__int64 nUserId, int ret, byte nAct) = 0;

		//�ֻ�����ͷ����
		virtual void OnRecvMobileNotify(__int64 nTeacherId,__int64 nUserId) = 0;

		//��ѯ��Ϣ�б���
		virtual void OnRecvMsgList(__int64 nUserId,int nMsgType,std::vector<SLMessageInfo>& msgList) = 0;

		//��ѯ�Զ���¼TOKEN����
		virtual void OnRecvAutoLoginToken(__int64 nUserId,int ret,LPCWSTR pwszToken) = 0;

		//������Ϣ����
		virtual void OnRecvClassMsgReminder(__int64 nCourseId,__int64 nClassId,char nMsgType) = 0;
	};
}
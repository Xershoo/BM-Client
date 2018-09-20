#pragma once

#ifdef BIZ_EXPORTS
#define BIZ_API __declspec(dllexport)
#else
#define BIZ_API // __declspec(dllimport)
#endif

#include "biz/interface/data.h"
#include "biz/interface/callback.h"

namespace biz
{
	class IUserInfoDataContainer;
	class IClassRoomDataContainer;
	class ICourseInfoContainer;
	struct IBizInterface
	{
		virtual void Start(const char* pIp, const char* pPort) = 0;
		virtual void SetCallback(ICallback*) = 0;
		virtual void Stop() = 0;
		virtual void Login(const wchar_t* pName, const wchar_t* pwd) = 0;
		virtual void Login(const wchar_t* pToken,__int64 nUserId) = 0;
		virtual bool relogin() = 0;
		virtual void Logout() = 0;
		virtual void EnterClass(__int64 nUserId, __int64 nCourseId, __int64 nClassId, int nUserState) = 0;
		virtual void LeaveClass(__int64 nClassRoomId) = 0;

		virtual void SendWhiteBoardEvent(SLWhiteBoardEvent& sInfo) = 0;
		virtual void SendCursewaveListEvent(SLCursewaveListOpt& scwEvent)=0;
		virtual void ChangeMyShowType(SLClassRoomShowInfo& sInfo) = 0;
        virtual void SetMainShow(char nType, __int64 nRoomId,__int64 nTeacherId) = 0;
		virtual void WhiteBoardOpt(char nType, __int64 nRoomId, int nPageId, const wchar_t* pWbName) = 0;

		virtual void ChangeClassState(__int64 nCourseId,__int64 nRoomId, char nState, __int64 nOptId) = 0;
		virtual void SendTextMessage(SLTextMsgInfo& sMsgInfo) = 0;
		virtual void OptDiscussGroup(int nOptType, __int64 nRoomId, int nGroups, const wchar_t* pTopic) = 0;
		
		//�û��ڿ��õĲ������������Ծ��֣����������ر������Ȳ���������ʦ��ѧ������Ƶ��ɾ�����ӵȲ���
		virtual void UserClassAction(__int64 nRoomId, __int64 nUserId, int nAction, __int64 nTeacherId) = 0;

		virtual IUserInfoDataContainer*	 GetUserInfoDataContainer() = 0;
		virtual IClassRoomDataContainer* GetClassRoomDataContainer() = 0;
		virtual ICourseInfoContainer*	 GetCourseInfoContainer() = 0;

		virtual void ChangeTopVideo(__int64 nRoomId, __int64 nUserId, bool bAdd) = 0;
		virtual void QueryCourseInfo(__int64 nCourseId) = 0;
		
		//�ύ����
		virtual void SubmitFeedBack(__int64 _nUserId,const wchar_t* pszTitle,const wchar_t* pszPhone,const wchar_t* pszContent) = 0;

		//��ѯ������Ϣ
		virtual void QueryUserInfo(__int64 nUserId) = 0;

		//��ѯweb���ομ��б�
		virtual void QueryLessonCourseware(__int64 nClassRoomId,__int64 nTeacherId) = 0;

		//��ѯ�ϴ�Token
		virtual void QueryUploadToken(__int64 nUserId) = 0;

		//��ʦ�л�����Ƶ
		virtual void SwitchTeacherVideo(__int64 nClassRoomId,__int64 nUserId,int nTeacherVideo) = 0;

		//����Ȩ�޿���
		virtual void SetClassMode(__int64 nClassRoomId,__int64 nUserId,int nMode) = 0;

        //ˢ��ѧ����Ƶ�б�
		virtual void RefreshStudentVideoList(__int64 nClassRoomId,__int64 nUserId) = 0;

		//��������
		virtual void SetStudentAsAssistant(__int64 nClassRoomId,__int64 nTeacherId,__int64 nUserId,bool bCancel) = 0;

		//ˢ���豸��
		virtual void UpdateClassRoomDevCode(__int64 nClassRoomId,__int64 nTeacherId) = 0;

		//ѡ��/�ر���������ͷ
		virtual void SelectMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bSel,const char* pszPushAddr) = 0;

		//ɾ����������ͷ
		virtual void DelMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bClassEnd) = 0;

		//��ѯ��Ϣ
		virtual void QueryMessageList(__int64 nUserId,int nMsgType = 1) = 0;  // nMsgType: 1-ϵͳ��Ϣ��2-������Ϣ

		//��ѯ�Զ���¼��TOKEN
		virtual void QueryAutoLoginToken(__int64 nUserId) = 0;

		//�ӳ��Ͽ�
		virtual void SetClassGoOn(__int64 nCourseId,__int64 nClassId,__int64 nUseId) = 0;

		//�γ�����
		virtual void EvaluateClass(__int64 nClassRoomId,__int64 nUserId,int rank,const wchar_t* description)=0;
	};


	BIZ_API IBizInterface* GetBizInterface();
	BIZ_API void DelBizInterface();
}


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
		
		//用户在课堂的操作，包括发言举手，打开声音，关闭声音等操作，及老师把学生从视频区删除增加等操作
		virtual void UserClassAction(__int64 nRoomId, __int64 nUserId, int nAction, __int64 nTeacherId) = 0;

		virtual IUserInfoDataContainer*	 GetUserInfoDataContainer() = 0;
		virtual IClassRoomDataContainer* GetClassRoomDataContainer() = 0;
		virtual ICourseInfoContainer*	 GetCourseInfoContainer() = 0;

		virtual void ChangeTopVideo(__int64 nRoomId, __int64 nUserId, bool bAdd) = 0;
		virtual void QueryCourseInfo(__int64 nCourseId) = 0;
		
		//提交反馈
		virtual void SubmitFeedBack(__int64 _nUserId,const wchar_t* pszTitle,const wchar_t* pszPhone,const wchar_t* pszContent) = 0;

		//查询个人信息
		virtual void QueryUserInfo(__int64 nUserId) = 0;

		//查询web备课课件列表
		virtual void QueryLessonCourseware(__int64 nClassRoomId,__int64 nTeacherId) = 0;

		//查询上传Token
		virtual void QueryUploadToken(__int64 nUserId) = 0;

		//老师切换主视频
		virtual void SwitchTeacherVideo(__int64 nClassRoomId,__int64 nUserId,int nTeacherVideo) = 0;

		//课堂权限控制
		virtual void SetClassMode(__int64 nClassRoomId,__int64 nUserId,int nMode) = 0;

        //刷新学生视频列表
		virtual void RefreshStudentVideoList(__int64 nClassRoomId,__int64 nUserId) = 0;

		//提升助教
		virtual void SetStudentAsAssistant(__int64 nClassRoomId,__int64 nTeacherId,__int64 nUserId,bool bCancel) = 0;

		//刷新设备码
		virtual void UpdateClassRoomDevCode(__int64 nClassRoomId,__int64 nTeacherId) = 0;

		//选择/关闭网络摄像头
		virtual void SelectMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bSel,const char* pszPushAddr) = 0;

		//删除网络摄像头
		virtual void DelMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bClassEnd) = 0;

		//查询消息
		virtual void QueryMessageList(__int64 nUserId,int nMsgType = 1) = 0;  // nMsgType: 1-系统消息；2-个人消息

		//查询自动登录的TOKEN
		virtual void QueryAutoLoginToken(__int64 nUserId) = 0;

		//延迟上课
		virtual void SetClassGoOn(__int64 nCourseId,__int64 nClassId,__int64 nUseId) = 0;

		//课程评价
		virtual void EvaluateClass(__int64 nClassRoomId,__int64 nUserId,int rank,const wchar_t* description)=0;
	};


	BIZ_API IBizInterface* GetBizInterface();
	BIZ_API void DelBizInterface();
}


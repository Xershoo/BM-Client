#pragma once

#include "BizSession.h"
#include "Ping.h"
#include "interface/callback.h"
#include "interface/interface.h"
#include "bizutility/func_dispatcher/func_dispatcher.h"
#include "data/CLUserInfoData.h"
#include "data/CLClassRoomInfoContainer.h"
#include "data/CLCourseInfoContainer.h"
#include <string>

namespace biz
{
	enum 
	{
		USER_LOGOUT = -1,
		USER_LOGINING,
		USER_LOGIN_OK,
		USER_VALIDATE,
		USER_VALIDATE_OK,
		
	};

	struct SLLoginUserInfo
	{
		SLLoginUserInfo()
		{
			memset(this, 0, sizeof(SLLoginUserInfo));			
			nState = USER_LOGOUT;
		}

		wchar_t		szUserName[MINTEXTLEN];
		wchar_t		szUserPwd[MINTEXTLEN];
		wchar_t		szToken[BIGTEXTLEN];
		__int64		nUserId;
		int			nLoginType;
		char		nState;
		char		nNetsip;
	};

	class Biz : public IBizInterface
	{
	public:
		static Biz* GetInst();
		static void DelInst();
		
		Biz();

		biz_utility::AnsyFuncDispcher& GetDispatcher()
		{ 
			return _dispatcher; 
		};

		void ProcessProtocol(gnet::Aio::SID, gnet::Protocol* p);

		virtual IUserInfoDataContainer*	 GetUserInfoDataContainer();
		virtual IClassRoomDataContainer* GetClassRoomDataContainer();
		virtual ICourseInfoContainer* GetCourseInfoContainer();

		virtual void Start(const char* pIp, const char* pPort);
		virtual void SetCallback(ICallback*);
		virtual void Stop();
		virtual void Login(const wchar_t* pName, const wchar_t* pwd);
		virtual void Login(const wchar_t* pToken,__int64 nUserId);
		virtual bool relogin();
		virtual void Logout();
		virtual void ValidateToken();
		virtual void EnterClass(__int64 nUserId, __int64 nCourseId,  __int64 nClassId,int nUserState);
		virtual void LeaveClass(__int64 nClassRoomId);

		virtual void SendCursewaveListEvent(SLCursewaveListOpt& scwEvent);

		virtual void SendWhiteBoardEvent(SLWhiteBoardEvent& sInfo);
		virtual void ChangeMyShowType(SLClassRoomShowInfo& sInfo);
        virtual void SetMainShow(char nType, __int64 nRoomId,__int64 nTeacherId);
		virtual void WhiteBoardOpt(char nType, __int64 nRoomId, int nPageId, const wchar_t* pWbName);

		virtual void ChangeClassState(__int64 nCourseId,__int64 nRoomId, char nState, __int64 nOptId);
		virtual void SendTextMessage(SLTextMsgInfo& sMsgInfo);

		virtual void OptDiscussGroup(int nOptType, __int64 nRoomId, int nGroups, const wchar_t* pTopic);
		virtual void UserClassAction(__int64 nRoomId, __int64 nUserId, int nAction, __int64 nTeacherId);

		virtual void ChangeTopVideo(__int64 nRoomId, __int64 nUserId, bool bAdd);

		//登录,注册,找回密码		
		virtual void QueryCourseInfo(__int64 nCourseId);
		virtual void SubmitFeedBack(__int64 _nUserId,const wchar_t* pszTitle,const wchar_t* pszPhone,const wchar_t* pszContent);	
		virtual void QueryUserInfo(__int64 nUserId);
		
		//查询web备课课件列表
		virtual void QueryLessonCourseware(__int64 nClassRoomId,__int64 nTeacherId);

		//查询上传Token
		virtual void QueryUploadToken(__int64 nUserId);

		//老师切换主视频
		virtual void SwitchTeacherVideo(__int64 nClassRoomId,__int64 nUserId,int nTeacherVideo);

		//课堂权限控制
		virtual void SetClassMode(__int64 nClassRoomId,__int64 nUserId,int nMode);
        
		//刷新学生视频列表
		virtual void RefreshStudentVideoList(__int64 nClassRoomId,__int64 nUserId);

		//提升为助教
		virtual void SetStudentAsAssistant(__int64 nClassRoomId,__int64 nTeacherId,__int64 nUserId,bool bCancel);

		//刷新设备码
		virtual void UpdateClassRoomDevCode(__int64 nClassRoomId,__int64 nTeacherId);

		//选择/关闭网络摄像头
		virtual void SelectMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bSel,const char* pszPushAddr);

		//删除网络摄像头
		virtual void DelMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bClassEnd);

		//查询消息
		virtual void QueryMessageList(__int64 nUserId,int nMsgType = 1);  // nMsgType: 1-系统消息；2-个人消息

		//查询自动登录的TOKEN
		virtual void QueryAutoLoginToken(__int64 nUserId);

		//延迟上课
		virtual void SetClassGoOn(__int64 nCourseId,__int64 nClassId,__int64 nUseId);

		//课程评价
		virtual void EvaluateClass(__int64 nClassRoomId,__int64 nUserId,int rank,const wchar_t* description);
	
	public:
		virtual void OnSesesionState(session_type type,session_state state);
		
	protected:
		static unsigned int CALLBACK BizEngineThread(void* arg);
		void threadProc();
	protected:
		virtual bool autoRelogin();
		virtual void doLogin();
		virtual void onLogin(int ret,__int64 uid,const wchar_t* token,const char* classServIp,const char* classServPort,char netisp);

	protected:
		virtual void getDevName();

	private:
		static Biz*						_inst;
		
		biz_utility::AnsyFuncDispcher	_dispatcher;
		ICallback*						_callback;

		BizSession						_loginSession;
		BizSession						_classSession;
		CPing							_ping;

		CLUserInfoDataContainer			_userinfoContainer;
		CLClassRoomInfoContainer		_classroomContainer;
		CLCourseInfoContainer			_courseinfoContainer;
		
		bool							_doLogin;
		SLLoginUserInfo					_userLogin;
		int								_timesRelogin;
		
		std::wstring					_devName;

		HANDLE							_thread;
		unsigned int					_threadId;
		UINT_PTR						_timerRelogin;
		bool							_relogin;
		UINT_PTR						_timerPing;
	};


	class NullCallback : public ICallback
	{
	public:
		static NullCallback* GetInst();
		virtual void OnConnectServerError(int nServerState, const char* pErrorInfo){}
		virtual void OnLogin(__int64 nUserId, char utype) {};
		virtual void OnLogKickOut(__int64 nUserId) {}
		virtual void OnUserEnter(SLUserInfo& Info) {};
		virtual void OnError(__int64 nRoomId, int nErrorCode){}
		virtual void OnEnterClassError(__int64 nRoomId,int nErrorCode) {};
		virtual void OnLoginError( int nErrorCode) {};
		virtual void OnUserLever(__int64 nUserId, __int64 nRoomId, __int64 nOptId) {};
		virtual void OnUserInfoReturn(SLUserInfo& sInfo){};

		virtual void OnWhiteBoardEvent(SLWhiteBoardEvent& sEventInfo){};
		virtual void OnWhiteBoardOpt(SLWhiteBoardOpt& sOptInfo) {};
		virtual void OnCursewaveListEvent(SLCursewaveListOpt& sCWCtrl){};
		virtual void ShowTypeChange(SLClassRoomShowInfo& sInfo) {};
        virtual void MainShowChange(char nType, __int64 nRoomId,__int64 nTeacherId) {};
		virtual void ClassStateChange(__int64 nRoomId, __int64 nClassId, char nState, __int64 nUserId,int ret){};
		virtual void OnRecvClassMessage(SLTextMsgInfo& sInfo) {};
		virtual void OnDiscussGroupOpt(int nOptType, __int64 nRoomId, const wchar_t* pTopic){};
		virtual void OnUserSpeakAction(__int64 nRoomId, __int64 nUserId, int nAction){};
		virtual void OnTeacherEnterDiscussGroup(int nRoomId, int nGroupId, __int64 nTeacherId, int nOpt){};
		virtual void OnGroupCallTeacher(__int64 nRoomId, __int64 nUserId){};

		virtual void OnTopVideoChange(__int64 nRoomId, __int64 nUserId,__int64 nTeacherId, bool bAddToTop){};

		virtual void OnRecvCourseInfo(__int64 nCourseId){};
		virtual void OnUpdateUserInfo(__int64 nRoomId) {};
				
		virtual void OnRecvUploadToken(int retCode,__int64 nUserId,int nValidateTime,const char* pwszToken) {};
		virtual void OnRecvStudentVideoList(__int64 nRoomId,std::vector<__int64>& stuVideoList) { };
		virtual void OnRecvPrevCoursewareList(__int64 nCourseId,std::vector<SLCoursewareInfo>& cwList) { };

		virtual void OnRecvSystemMessage(__int64 nUserId,const wchar_t* pwszMsg) { };		
		virtual void OnTeacherSwitchVideo(__int64 nRoomId,__int64 nUserId,int nVideo) { };
		virtual void OnSetClassMode(__int64 nRoomId,__int64 nUserId,char nMode) { };
			
		virtual void OnRecvSetAssistantMsg(__int64 nRoomId,__int64 nUserId,bool bAs,int ret){};
		virtual void OnRecvUpdateDevCodeMsg(__int64 nRoomId) {};
		virtual void OnRecvNetDevAccess(__int64 nRoomId,__int64 nUserId,const WCHAR* pwszDevName) {};
		virtual void OnRecvChooseMobileResponse(__int64 nTeacherId,__int64 nUserId, int ret, byte nAct){};
		virtual void OnRecvMobileNotify(__int64 nTeacherId,__int64 nUserId) {};		
		virtual void OnRecvMsgList(__int64 nUserId,int nMsgType,std::vector<SLMessageInfo>& msgList){};
		virtual void OnRecvAutoLoginToken(__int64 nUserId,int ret,LPCWSTR pwszToken){};
		virtual void OnRecvClassMsgReminder(__int64 nCourseId,__int64 nClassId,char nMsgType) {};
	};
}
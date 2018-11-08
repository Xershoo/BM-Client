#include "biz.h"
#include "convert.h"
#include "gnet/socket/net.h"
#include "log_biz.h"
#include "protocol/rpcgen_edu.h"
#include "gnet/type_convert.h"
#include <atlstr.h>
#include <string>

#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")  

#define MAX_RELOGIN_TIMES	(5)
#define TIMER_RELOGIN		(1)
#define WM_RELOGIN			(WM_USER+100)
#define WM_PING_SERV		(WM_USER+101)

namespace biz
{
	static bool GetMac(wstring& strMac)     
	{		
		typedef struct _ASTAT_
		{
			ADAPTER_STATUS   adapt;
			NAME_BUFFER		 NameBuff[30];
		}ASTAT,*PASTAT;

		typedef struct _LANA_ENUM
		{
			UCHAR   length;
			UCHAR   lana[MAX_LANA];
		}LANA_ENUM;

		NCB			ncb;
		ASTAT		Adapter;
		LANA_ENUM	lana_enum;    
		UCHAR		ret;
		wchar_t		wszMac[64]={0};

		memset(&ncb, 0, sizeof(ncb));
		memset(&lana_enum, 0, sizeof(lana_enum));    
		ncb.ncb_command = NCBENUM;
		ncb.ncb_buffer = (unsigned char *)&lana_enum;
		ncb.ncb_length = sizeof(LANA_ENUM);
		ret = Netbios(&ncb);

		if(ret != NRC_GOODRET)     
		{
			return false;
		}

		for(int lana=0; lana<lana_enum.length; lana++)     
		{
			ncb.ncb_command = NCBRESET;
			ncb.ncb_lana_num = lana_enum.lana[lana];
			ret = Netbios(&ncb);

			if(ret == NRC_GOODRET)
			{
				break; 
			}
		}

		if(ret != NRC_GOODRET)
		{
			return false;
		}

		memset(&ncb, 0, sizeof(ncb));
		ncb.ncb_command = NCBASTAT;
		ncb.ncb_lana_num = lana_enum.lana[0];
		strcpy((char*)ncb.ncb_callname, "*");
		ncb.ncb_buffer = (unsigned char *)&Adapter;
		ncb.ncb_length = sizeof(Adapter);
		ret = Netbios(&ncb);

		if(ret != NRC_GOODRET)
		{
			return false;
		}

		swprintf(wszMac,L"%02X-%02X-%02X-%02X-%02X-%02X",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5]);

		strMac = wszMac;
		return true;
	}

	IBizInterface* GetBizInterface()
	{
		return Biz::GetInst();
	}

	void DelBizInterface()
	{
		Biz::DelInst();
	}

	Biz* Biz::_inst = NULL;

	Biz* Biz::GetInst()
	{
		if (NULL == _inst)
			_inst = new Biz();
		return _inst;
	}

	void Biz::DelInst()
	{
		if (_inst != NULL)
		{
			delete _inst;
			_inst = NULL;
		}
	}

	IUserInfoDataContainer*	 Biz::GetUserInfoDataContainer()
	{
		return &_userinfoContainer;
	}

	IClassRoomDataContainer* Biz::GetClassRoomDataContainer()
	{
		return &_classroomContainer;
	}

	ICourseInfoContainer* Biz::GetCourseInfoContainer()
	{
		return &_courseinfoContainer;
	}

	Biz::Biz() : _callback(NullCallback::GetInst()),
		_loginSession(SESSION_LOGIN),
		_classSession(SESSION_CLASS),
		_doLogin(false),
		_relogin(false),
		_timesRelogin(0),
		_timerRelogin(0),
		_thread(NULL),
		_threadId(0)
	{	
		getDevName();
	}
	
	void Biz::getDevName()
	{
		GetMac(_devName);
		WCHAR wszDevName[MAX_PATH] = { 0 };
		swprintf(wszDevName,L"BMClass PC Client [%s]",(WCHAR*)_devName.c_str());
		_devName = wszDevName;
	}

	unsigned int CALLBACK Biz::BizEngineThread(void* arg)
	{
		Biz * pp = (Biz*)arg;
		if(pp){
			pp->threadProc();
		}

		return 0;
	}

	void Biz::threadProc()
	{
		MSG msg;
		
		while (::GetMessage(&msg, NULL, 0, 0) > 0)
		{
			switch(msg.message)
			{
			case WM_CLOSE:
				{
					if(_timerRelogin != 0)
					{
						::KillTimer(NULL,_timerRelogin);
						_timerRelogin = 0;
					}
					
					if(_timerPing != 0)
					{
						::KillTimer(NULL,_timerPing);
						_timerPing = 0;
					}

					_endthreadex(0);
				}

				return;
			case WM_TIMER:
				{
					UINT timerId = (UINT_PTR)msg.wParam;
					if(timerId == _timerRelogin)
					{
						_doLogin=true;
						_loginSession.Connect();
						
						::KillTimer(NULL,_timerRelogin);
						_timerRelogin = 0;

						break;
					}

					if(timerId == _timerPing){
						::KillTimer(NULL,_timerPing);
						_timerPing = 0;

						bool ret =  _ping.ping(_classSession.getServAddr());
						if(ret){
							_timerPing = ::SetTimer(NULL,0,2000,NULL);
							break;
						}
						
						Logout();
						if(_callback){
							_callback->OnConnectServerError(eServerState_ConFailed, "connect server failed");
						}
					}
				}
				break;
			case WM_RELOGIN:
				{
					bool done =(bool)msg.lParam;
					
					if(done){
						_timesRelogin++;
						if(_classSession.IsConnectServer()){
							_classSession.Close();
						}

						if(_loginSession.IsConnectServer()) {
							_loginSession.Close();
						}

						_userLogin.nState = USER_LOGOUT;
						_timerRelogin = ::SetTimer(NULL,0,1000,NULL);
					}else{
						if(_timerRelogin != 0 ) {
							::KillTimer(NULL,_timerRelogin);
							_timerRelogin = 0;
						}
					}
				}
				break;

			case WM_PING_SERV:
				{
					bool done =(bool)msg.lParam;

					if(done){
						_timerPing = ::SetTimer(NULL,0,1000,NULL);
					}else{
						if(_timerPing != 0 ) {
							::KillTimer(NULL,_timerPing);
							_timerPing = 0;
						}
					}
				}
				break;
			default:
				break;
			}
		}

		return ;
	}

	void Biz::Start(const char* pIp, const char* pPort)
	{	
		_dispatcher.Startup();
		gnet::Engine::GetInst()->StartUp();		
		rpcgen_edu::RegisterProtocols();

		_loginSession.SetServerInfo(pIp, pPort);
		_loginSession.Connect();

		_thread = (HANDLE)::_beginthreadex(NULL, 0, BizEngineThread, (void*)this, 0, &_threadId);
	}
	
	void Biz::SetCallback(ICallback* cb)
	{
		if (NULL == cb)
			_callback = NullCallback::GetInst();
		else
			_callback = cb;
	}

	void Biz::Stop()
	{		
		rpcgen_edu::UnregisterProtocols();		
		gnet::Engine::GetInst()->CleanUp();
		_dispatcher.Cleanup();

		if(NULL != _thread){
			::PostThreadMessageA(_threadId,WM_CLOSE,0,0);
			::WaitForSingleObject(_thread, INFINITE);
			::CloseHandle(_thread);
			_thread = NULL;
			_threadId = 0;
		}
	}
	
	void Biz::Login(const wchar_t* pName, const wchar_t* pwd)
	{	
		if(NULL == pName || NULL == pName[0] || NULL == pwd || NULL == pwd[0])
		{
			return;
		}

		if(_userLogin.nState != USER_LOGOUT)
		{
			return;
		}

		wcscpy_s(_userLogin.szUserName, pName);
		wcscpy_s(_userLogin.szUserPwd, pwd);
		_userLogin.nLoginType = gnet::Login::NAMEPASSWD;
		_userLogin.nState = USER_LOGINING;
						
		if(!_loginSession.IsConnectServer())
		{
			_doLogin = true;
			_loginSession.Connect();
			return;
		}

		doLogin();
	}

	void Biz::Login(const wchar_t* pToken,__int64 nUserId)
	{	
		if(NULL == pToken || NULL == pToken[0] || NULL == nUserId)
		{
			return;
		}

		if(_userLogin.nState != USER_LOGOUT)
		{
			return;
		}

		wcscpy_s(_userLogin.szToken, pToken);
		_userLogin.nUserId = nUserId;
		_userLogin.nLoginType = gnet::Login::TOKEN;
		_userLogin.nState = USER_LOGINING;

		if(!_loginSession.IsConnectServer())
		{
			_doLogin = true;
			_loginSession.Connect();
			return;
		}

		doLogin();
	}


	void Biz::Logout()
	{	
		_doLogin = false;
		_relogin = false;
		_timesRelogin = 0;
		_userLogin.nState = USER_LOGOUT;

		::PostThreadMessageA(_threadId,WM_RELOGIN,0,0);
		::PostThreadMessageA(_threadId,WM_PING_SERV,0,0);

		_loginSession.Close();
		_classSession.Close();

		_userinfoContainer.Clear();
		_classroomContainer.Clear();
	}

	void Biz::ValidateToken()
	{
		gnet::TokenValidate p;
		LPCWSTR 	LOGIN_DEV_NAME = _devName.c_str();

		p.devicetype_ = gnet::TokenValidate::PC;
		p.userid_ = _userLogin.nUserId;
		p.token_.replace(_userLogin.szToken,wcslen(_userLogin.szToken) * sizeof(wchar_t));
		p.devicename_.replace(LOGIN_DEV_NAME, wcslen(LOGIN_DEV_NAME) * sizeof(wchar_t));
		
		_classSession.Send(p);

		_userLogin.nState = USER_VALIDATE;
	}
	
	void Biz::EnterClass(__int64 nUserId, __int64 nCourseId, __int64 nClassId,int nUserState)
	{
		_userLogin.nUserId = nUserId;

		SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(nUserId);

		gnet::UserEnter p;
		p.cid_ = nCourseId;
		p.classid_ = nClassId;
		p.userinfo_.userid_ = nUserId;
		p.userinfo_.state_ = nUserState;
		p.userinfo_.authority_ = (int)sInfo.nUserAuthority;

		p.device_ = gnet::Login::PC;
		p.netisp_ = _userLogin.nNetsip;
		
		_classSession.Send(p);
	}

	void Biz::LeaveClass(__int64 nClassRoomId)
	{
		gnet::UserLeave p;		
		p.cid_ = nClassRoomId;
		p.userid_ = _userLogin.nUserId;
        p.intime_ = 0;

		_classSession.Send(p);
		
        CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(nClassRoomId);
		pRoom->ResetClass();
	}

	void Biz::SendWhiteBoardEvent(SLWhiteBoardEvent& sInfo)
	{
		gnet::WhiteBoardEvent p;

		LOG << "Send whiteboard event:" ;
		LOG << "room id" << sInfo._nRoomId;
		LOG << "user id" << sInfo._nUserId;
		LOG << "page id" << sInfo._nPageId;
		LOG << "paint type" << sInfo._nPainType;		
		LOG << "paint information" << sInfo._strParam.c_str();

		p.action_.cid_ = sInfo._nRoomId;
		p.action_.pageId_ = sInfo._nPageId;
		p.action_.paintId_ = sInfo._nPaintId;
		p.action_.paintype_ = sInfo._nPainType;
		p.action_.userid_ = sInfo._nUserId;
		p.action_.oweruid_ = sInfo._nOwerUserId;
		p.action_.arguments_.replace(sInfo._strParam.c_str(), sInfo._strParam.size() * sizeof(char));

		_classSession.Send(p);
	}

	void Biz::SendCursewaveListEvent(SLCursewaveListOpt& scwEvent)
	{
		gnet::AddCourseWare p;
		p.receiver_ = 0;
		p.cid_ = scwEvent._nRoomId;
		p.userid_ = scwEvent._nUserId;
		p.actiontype_ = scwEvent._nOpt;
		p.sender_ = gnet::AddCourseWare::CLIENT;		
		p.cwname_.replace(scwEvent._szFileName, wcslen(scwEvent._szFileName) * sizeof(wchar_t));

		_classSession.Send(p);
	}

	void Biz::ChangeMyShowType(SLClassRoomShowInfo& sInfo)
	{
		gnet::SwitchClassShow p;
		p.userid_ = _userLogin.nUserId;
		p.cid_ = sInfo._nRoomId;
		p.currentshow_.showtype_ = sInfo._nShowType;
		p.currentshow_.page_ = sInfo._nShowPage;
		ATL::CStringA str(sInfo._szShowName);
		p.currentshow_.name_.replace(str,str.GetLength());
		_classSession.Send(p);
	}

    void Biz::SetMainShow(char nType, __int64 nRoomId,__int64 nTeacherId)
    {
        gnet::SetMainShow p;

        p.classid_ = nRoomId;
        p.showtype_ = nType;
        p.teacher_ = nTeacherId;

        _classSession.Send(p);
    }

	void Biz::ChangeClassState(__int64 nCourseId,__int64 nRoomId, char nState, __int64 nOptId)
	{
		gnet::SetClassState p;
		p.classid_ = nRoomId;
		p.userid_ = nOptId;
		p.cid_ = nCourseId;
		
		switch(nState)
		{
		case biz::EClassroomstate_Doing:
			p.classstate_ = gnet::SetClassState::CLASS_BEGIN;
			break;
		case biz::Eclassroomstate_Free:
			p.classstate_ = gnet::SetClassState::CLASS_END;
			break;
		default:
			return;
		}

		_classSession.Send(p);
	}

	void Biz::WhiteBoardOpt(char nType, __int64 nRoomId, int nPageId, const wchar_t* pWbName)
	{
		gnet::CreateWhiteBoard p;
		p.actionytype_ = nType;
		p.cid_ = nRoomId;
		p.userid_ = _userLogin.nUserId;
		p.wbid_ = nPageId;
		p.wbname_.replace(pWbName, wcslen(pWbName) * sizeof(wchar_t));
		_classSession.Send(p);
	}

	void Biz::SendTextMessage(SLTextMsgInfo& sMsgInfo)
	{
		gnet::SendTextMsg p;
		p.cid_ = sMsgInfo._nClassRoomId;
		p.receiver_ = 0;
		p.recvid_ = sMsgInfo._nRecvUserId;
		p.recvtype_ = sMsgInfo._nMsgType;
		p.userid_ = _userLogin.nUserId;
		p.recvgroupid_ = sMsgInfo._nGroupId;
		p.time_ = sMsgInfo._nTimer;
		p.message_.replace(sMsgInfo._szMsg, sMsgInfo._nMsgLen * sizeof(wchar_t));
		_classSession.Send(p);
	}

	void Biz::OptDiscussGroup(int nOptType, __int64 nRoomId, int nGroups, const wchar_t* pTopic)
	{
		gnet::CreateTalkGroup p;
		p.actionytype_ = nOptType;
		p.userid_ = _userLogin.nUserId;
		p.cid_ = nRoomId;
		p.membercount_ = nGroups;
		if(pTopic)
		{
			p.topic_.replace(pTopic, wcslen(pTopic) * sizeof(wchar_t));
		}

		_classSession.Send(p);
	}

	void Biz::UserClassAction(__int64 nRoomId, __int64 nUserId, int nAction, __int64 nTeacherId)
	{
		gnet::ClassAction p;
		p.actiontype_ = nAction;
		p.cid_ = nRoomId;
		p.userid_ = nUserId;
		p.teacheruid_ = nTeacherId;
		_classSession.Send(p);
	}

	void Biz::ChangeTopVideo(__int64 nRoomId, __int64 nUserId, bool bAdd)
	{
		gnet::ClassAction p;
		if(bAdd)
			p.actiontype_ = gnet::ClassAction::ADD_STUDENT_VIDEO;
		else
			p.actiontype_ = gnet::ClassAction::DEL_STUDENT_VIDEO;
		
		p.cid_ = nRoomId;
		p.userid_ = nUserId;
		_classSession.Send(p);
	}
	
	void Biz::QueryCourseInfo(__int64 nCourseId)
	{
		gnet::QueryCourse p;
		p.courseid_ = nCourseId;
	
		_classSession.Send(p);
	}

	void Biz::SubmitFeedBack(__int64 _nUserId,const wchar_t* pszTitle,const wchar_t* pszPhone,const wchar_t* pszContent)
	{
		gnet::FeedBack p;
		p.userid_  = _nUserId;
		p.fbtype_ = 0;

		if(pszTitle)
		{
			p.title_.replace(pszTitle, wcslen(pszTitle) * sizeof(wchar_t));
		}

		if(pszPhone)
		{
			p.mobile_.replace(pszPhone, wcslen(pszPhone) * sizeof(wchar_t));
		}

		if(pszContent)
		{
			p.content_.replace(pszContent, wcslen(pszContent) * sizeof(wchar_t));
		}

		_classSession.Send(p);
	}

	void Biz::QueryUserInfo(__int64 nUserId)
	{
		gnet::QueryUser p;
		p.userid_ = nUserId;

		_classSession.Send(p);
	};

	void Biz::QueryLessonCourseware(__int64 nClassRoomId,__int64 nTeacherId)
	{
		gnet::QueryPreCWares p;

		p.cid_ = nClassRoomId;
		p.tid_ = nTeacherId;

		_classSession.Send(p);
	}
		
	void Biz::QueryUploadToken(__int64 nUserId)
	{
		gnet::QueryToken p;

		p.userid_ = nUserId;

		_classSession.Send(p);
	}

	void Biz::SwitchTeacherVideo(__int64 nClassRoomId,__int64 nUserId,int nTeacherVideo)
	{
		gnet::SetTeacherVedio p;
		p.cid_ = nClassRoomId;
		p.userid_ = nUserId;
		p.teachervedio_ = nTeacherVideo;

		_classSession.Send(p);
	}

	void Biz::SetClassMode(__int64 nClassRoomId,__int64 nUserId,int nMode)
	{
		gnet::SetClassMode p;
		p.cid_ = nClassRoomId;
		p.userid_ = nUserId;
		p.classmode_ = nMode;

		_classSession.Send(p);
	}

	void Biz::SetStudentAsAssistant(__int64 nClassRoomId,__int64 nTeacherId,__int64 nUserId,bool bCancel)
	{
		if(bCancel)
		{
			gnet::CancelAssistant p;
			p.cid_ = nClassRoomId;
			p.tid_ = nTeacherId;
			p.userid_ = nUserId;

			_classSession.Send(p);
		}
		else
		{
			gnet::PromoteAsAssistant p;
			p.cid_ = nClassRoomId;
			p.tid_ = nTeacherId;
			p.userid_ = nUserId;

			_classSession.Send(p);
		}
	}

	void Biz::RefreshStudentVideoList(__int64 nClassRoomId,__int64 nUserId)
	{
		gnet::RefreshVideoListReq p;
		p.cid_ = nClassRoomId;
		p.teacherid_ = nUserId;

		_classSession.Send(p);
	}

	void Biz::UpdateClassRoomDevCode(__int64 nClassRoomId,__int64 nTeacherId)
	{
		gnet::UpdateCodeReq p;
		p.cid_ = nClassRoomId;
		p.tid_ = nTeacherId;

		_classSession.Send(p);
	}
	
	void Biz::SelectMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bSel,const char* pszPushAddr)
	{
		gnet::ChooseMobile p;
		p.tid_ = nTeacherId;
		p.userid_ = nUserId;

		if(pszPushAddr)
		{
			p.pushaddr_.replace(pszPushAddr,sizeof(char) * strlen(pszPushAddr));
		}

		p.actiontype_ = bSel ? gnet::ChooseMobile::CHOOSE : gnet::ChooseMobile::STOP;
		_classSession.Send(p);
	}

	void Biz::DelMobileAsCamera(__int64 nTeacherId,__int64 nUserId,bool bClassEnd)
	{
		gnet::Kick p;
		p.tid_ = nTeacherId;
		p.userid_ = nUserId;

		p.notifytype_ = bClassEnd ? gnet::Kick::CLASS_END:gnet::Kick::DELETE_DEVICE;

		_classSession.Send(p);
	}

	void Biz::QueryMessageList(__int64 nUserId,int nMsgType /* = 1 */)
	{
		gnet::MesgReq p;
		p.userid_ = nUserId;
		p.msgtype_ = nMsgType;

		_classSession.Send(p);
	}

	void Biz::QueryAutoLoginToken(__int64 nUserId)
	{
		gnet::QueryAutoLoginToken p;
		LPCWSTR 	LOGIN_DEV_NAME = _devName.c_str();

		p.userid_ = nUserId;
		p.devicetype_ = gnet::TokenValidate::PC;		
		p.devicename_.replace(LOGIN_DEV_NAME, wcslen(LOGIN_DEV_NAME) * sizeof(wchar_t));

		_classSession.Send(p);
	}

	void Biz::SetClassGoOn(__int64 nCourseId,__int64 nClassId,__int64 nUseId)
	{
		gnet::ClassGoOn p;
		p.classid_ = nClassId;
		p.courseid_ = nCourseId;
		p.teacher_ = nUseId;

		_classSession.Send(p);
	}

	void Biz::EvaluateClass(__int64 nClassRoomId,__int64 nUserId,int rank,const wchar_t* description)
	{
		gnet::EvaluateClass p;
		p.cid_ = nClassRoomId;
		p.userid_ = nUserId;
		p.rank_ = rank;

		if(NULL != description)
		{
			p.description_.replace(description,sizeof(wchar_t) * wcslen(description));
		}

		_classSession.Send(p);
	}

	void Biz::OnSesesionState(session_type type,session_state state)
	{
		switch(state)
		{
		case SESSION_CLOSE:
			{
				if(type == SESSION_LOGIN) {
					if(_userLogin.nState == USER_LOGINING){
						_userLogin.nState = USER_LOGOUT;
						_relogin = false;
						_doLogin = false;
					}

					break;
				}

				if(type == SESSION_CLASS) {			//断开连接自动重连
					::PostThreadMessageA(_threadId,WM_PING_SERV,0,false);
					if(!autoRelogin()) {
						_timesRelogin = 0;

						if(_callback){
							_callback->OnConnectServerError(eServerState_ConFailed, "connect server failed");
						}
					} else {
						if(_callback){
							_callback->OnConnectServerError(eServerState_AutoReconnect, "connect server failed");
						}
					}
				}
			}
			break;
		case SESSION_FAILED:
			{
				_userLogin.nState = USER_LOGOUT;
				_relogin = false;

				if(type == SESSION_LOGIN) {
					_timesRelogin = 0;

					if(_callback){
						_callback->OnConnectServerError(eServerState_ConFailed, "connect server failed");
					}
					break;
				}

				if(type == SESSION_CLASS) {				////断开连接自动重连
					::PostThreadMessageA(_threadId,WM_PING_SERV,0,0);
					if(!autoRelogin()) {
						_timesRelogin = 0;

						if(_callback){
							_callback->OnConnectServerError(eServerState_ConFailed, "connect server failed");
						}
					} else {
						if(_callback){
							_callback->OnConnectServerError(eServerState_AutoReconnect, "reconnect server");
						}
					}
					
					break;
				}
			}
			break;
		case SESSION_CONNECTED:
			{
				if(type == SESSION_LOGIN) {
					if(_doLogin){
						doLogin();
					}

					break;
				}
				if(type == SESSION_CLASS) {
					ValidateToken();
					::PostThreadMessageA(_threadId,WM_PING_SERV,0,true);

					if(_callback){
						_callback->OnConnectServerError(eServerState_connect, "connect server success");
					}
				}
			}
			break;
		case SESSION_CONNECTING:
			{

			}
			break;
		}
	}

	void Biz::doLogin()
	{
		LPCWSTR 	LOGIN_DEV_NAME = _devName.c_str();
		gnet::Login p;
		if(_userLogin.nLoginType == gnet::Login::NAMEPASSWD)
		{
			p.username_.replace(_userLogin.szUserName, wcslen(_userLogin.szUserName) * sizeof(wchar_t));
			p.passwordmd5_.replace(_userLogin.szUserPwd, wcslen(_userLogin.szUserPwd) * sizeof(wchar_t));
			p.devicename_.replace(LOGIN_DEV_NAME, wcslen(LOGIN_DEV_NAME) * sizeof(wchar_t));

			p.logintype_ = gnet::Login::NAMEPASSWD;
		}
		else if(_userLogin.nLoginType == gnet::Login::TOKEN)
		{
			p.token_.replace(_userLogin.szToken, wcslen(_userLogin.szToken) * sizeof(wchar_t));
			p.devicename_.replace(LOGIN_DEV_NAME, wcslen(LOGIN_DEV_NAME) * sizeof(wchar_t));
			p.userid_ = _userLogin.nUserId;
			p.logintype_ = gnet::Login::TOKEN;
		}
		else
		{
			return;
		}

		p.devicetype_ = gnet::Login::PC;
		
		_loginSession.Send(p);
	}

	void Biz::onLogin(int ret,__int64 uid,const wchar_t* token,const char* classServIp,const char* classServPort,char netisp)
	{
		if(ret != 0)
		{
			_userLogin.nState = USER_LOGOUT;
			_doLogin = false;
			_relogin = false;

			return;
		}

		_userLogin.nUserId = uid;
		_userLogin.nNetsip = netisp;
		wcscpy_s(_userLogin.szToken,token);
		
		_userLogin.nState = USER_LOGIN_OK;

		if(_classSession.IsConnectServer()){
			_classSession.Close();
		}

		_loginSession.Close();

		_classSession.SetServerInfo(classServIp,classServPort);
		_classSession.Connect();
	}

	bool Biz::autoRelogin()
	{
		if(_relogin){
			return true;
		}

		if(_timesRelogin >= MAX_RELOGIN_TIMES){
			return false;
		}

		if (_userLogin.nState == USER_LOGINING || 
			_userLogin.nState == USER_VALIDATE) {
			return false;
		}
		
		_relogin = ::PostThreadMessageA(_threadId,WM_RELOGIN,0,(LPARAM)true) ? true:false;

		return _relogin;
	}

	bool Biz::relogin()
	{
		return autoRelogin();
	}

	NullCallback* NullCallback::GetInst()
	{
		static NullCallback cb;
		return &cb;
	}
}
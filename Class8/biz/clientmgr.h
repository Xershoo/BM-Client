#pragma once

#include "gnet/socket/socket_protocol.h"
#include "interface/data.h"
#include "interface/callback.h"
#include <string>

namespace biz
{
	enum 
	{
		USER_LOGOUT = -1,
		USER_LOGINING ,
		USER_VALIDATE ,
		USER_LOGIN_OK
	};

	

	class ClientManager : public gnet::SessionManager
	{
	public:
		ClientManager();
		virtual void OnConnectAbort(const gnet::Connection& con);
		virtual void OnConnect(gnet::Aio::SID sid, const gnet::Connection& con);
		virtual void OnClose(gnet::Aio::SID sid, std::string errStr);
		virtual void OnError(gnet::Aio::SID sid, int event, int error);
		virtual void ProcessProtocol(gnet::Aio::SID sid, gnet::Protocol& p);


	public:
		using gnet::AioManager::Close;
		using gnet::SessionManager::Send;

		void Close();
		void ConnectServer();
		void ReconnectServer();

		void Send(const gnet::Protocol& protocol);
		void SetCallback(ICallback* pCallBack);
		void SetServerInfo(const char* pIp, const char* pPort, void* userData);		
		bool IsConnectServer() { return _sid != -1;}


		ICallback*  GetBizCallback() 
		{ 
			return _pCallBack ;
		}

		const wchar_t*	GetDeviceName()
		{
			return _strDevName.c_str();
		}

	private:
		gnet::Aio::SID	_sid;
		SLLoginUserInfo		_userLogin;
		ICallback*			_pCallBack;
		
		std::string			_strServerIp;
		std::string			_strServerPort;
		void*				_userData;
		std::wstring		_strDevName;
	};
}
#pragma once

#include "gnet/socket/socket_protocol.h"
#include "interface/data.h"
#include <string>

namespace biz
{
	enum session_type{
		SESSION_LOGIN = 0,
		SESSION_CLASS,
	};

	enum session_state {
		SESSION_CLOSE = 0,
		SESSION_CONNECTING,
		SESSION_CONNECTED,
		SESSION_FAILED
	};

	class BizSession : public gnet::SessionManager
	{
	public:
		BizSession(session_type type);
		
		virtual void OnConnectAbort(const gnet::Connection& con);
		virtual void OnConnect(gnet::Aio::SID sid, const gnet::Connection& con);
		virtual void OnClose(gnet::Aio::SID sid, std::string errStr);
		virtual void OnError(gnet::Aio::SID sid, int event, int error);

		virtual void ProcessProtocol(gnet::Aio::SID sid, gnet::Protocol& p);

	public:
		using gnet::AioManager::Close;
		using gnet::SessionManager::Send;

	public:
		void Connect();
		void Send(const gnet::Protocol& protocol);
		void Close();

	public:
		void	SetServerInfo(const char* pIp, const char* pPort);	
		char*   getServAddr() {
			return (char*)_serverIP.c_str();
		}
		
		inline bool IsConnectServer() { 
			return (_sessionId != -1) ? true : false;
		}
		
	private:
		session_type		_sessionType;
		gnet::Aio::SID		_sessionId;
		
		std::string			_serverIP;
		std::string			_serverPort;
	};
}
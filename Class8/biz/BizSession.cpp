#include "biz.h"
#include "BizSession.h"
#include "log_biz.h"
#include "bizutility/log/assert_log.h"
#include "protocol/rpcgen_edu.h"

namespace biz
{	
	BizSession::BizSession(session_type type) : _sessionId (-1),
		_sessionType(type),
		_serverIP(""),
		_serverPort("0")
	{
		
	}


	void BizSession::OnConnectAbort(const gnet::Connection& con)
	{
		LOG << "Session OnConnectAbort" << _sessionId;

		Biz::GetInst()->OnSesesionState(_sessionType,SESSION_FAILED);
	}


	void BizSession::OnConnect(gnet::Aio::SID sid, const gnet::Connection& con)
	{
		LOG << "Session OnConnect sid: " << sid;
		_sessionId = sid;

		Biz::GetInst()->OnSesesionState(_sessionType,SESSION_CONNECTED);
	}


	void BizSession::OnClose(gnet::Aio::SID sid, std::string errStr)
	{
		LOG << "Session OnClose sid: " << sid;
		
		if(_sessionId == -1 || _sessionId != sid)
		{
			return;
		}

		_sessionId = -1;
		Biz::GetInst()->OnSesesionState(_sessionType,SESSION_CLOSE);
	}


	void BizSession::OnError(gnet::Aio::SID sid, int event, int error)
	{
		LOG << "Session OnError sid: " << sid << ",error: " << error;
		if(sid != _sessionId || _sessionId == -1){
			return;
		}

		Biz::GetInst()->OnSesesionState(_sessionType,SESSION_FAILED);
	}


	void BizSession::ProcessProtocol(gnet::Aio::SID sid, gnet::Protocol& p)
	{	
		if(_sessionId == -1 || _sessionId != sid)
		{
			return;
		}

		LOG << "ProcessProtocol sid: " << sid << " type: " << p.GetType();

		Biz::GetInst()->GetDispatcher().PostTask(Biz::GetInst(), &Biz::ProcessProtocol, sid, p.Clone());
	}

	void BizSession::Close()
	{
		if (-1 == _sessionId)
		{
			return;
		}

		gnet::Aio::SID nCloseSid = _sessionId;
		Close(nCloseSid, "Client Close");
		_sessionId = -1;
	}


	void BizSession::Send(const gnet::Protocol& protocol)
	{
		if (-1 == _sessionId)
		{
			return;
		}

		Send(_sessionId, protocol);
	}

	void BizSession::SetServerInfo(const char* pIp, const char* pPort)
	{
		if(pPort == NULL || pIp == NULL)
		{
			return;
		}

		_serverPort = pPort;
		_serverIP = pIp;
	}

	void BizSession::Connect()
	{
		if(_sessionId != -1)
		{
			return;
		}

		if(_serverIP.empty() || _serverPort.empty())
		{
			return;
		}

		Biz::GetInst()->OnSesesionState(_sessionType,SESSION_CONNECTING);
		gnet::AioManager::Connect(_serverIP.c_str(), _serverPort.c_str(), NULL);
	}
}
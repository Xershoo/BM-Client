#pragma once

#include "gnet/socket/socket_protocol.h"
using namespace gnet;


class ClientManager : public SessionManager
{
public:
	virtual void OnConnectAbort(const Connection& con)
	{
		LOG << "OnConnectAbort";
	}

	virtual void OnConnect(Aio::SID sid, const Connection& con)
	{
		LOG << "OnConnect sid: " << sid;
	}

	virtual void OnClose(Aio::SID sid, std::string errStr)
	{
		LOG << "OnClose sid: " << sid;
	}

	virtual void OnError(Aio::SID sid, int event, int error)
	{
		LOG << "OnError sid: " << sid;
	}

	virtual void ProcessProtocol(unsigned int sid, Protocol& p)
	{
		LOG << "ProcessProtocol sid: " << sid << " type: " << p.GetType();
	}
};
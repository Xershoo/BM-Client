#pragma once

#include "gnet/socket/socket_protocol.h"
using namespace gnet;


class ServerManager : public SessionManager
{
public:
	virtual void OnListenAbort(const CListen& clisten)
	{
		LOG << "OnListenAbort";
	}

	virtual void OnListen(Aio::SID sid, const CListen& listen)
	{
		LOG << "OnListen sid: " << sid;
	}


	// client
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
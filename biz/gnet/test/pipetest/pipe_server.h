#pragma once

#include "gnet/pipe/pipe_protocol.h"
#include "log_test.h"
using namespace gnet;


class PipServer : public PipeServer
{
public:
	virtual void ProcessProtocol(unsigned int sid, Protocol& p)
	{
		LOG << "receive type: " << p.GetType();
	}


	virtual void OnConnected()
	{
		LOG << "OnConnected";
	}


	virtual void OnServerClose()
	{
		LOG << "OnServerClose";
	}
};
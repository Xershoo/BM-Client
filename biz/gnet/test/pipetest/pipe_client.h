#pragma once

#include "protocols/rpcgen_voice.h"
#include "gnet/pipe/pipe_protocol.h"
#include "log_test.h"
using namespace gnet;


class PipClient : public PipeClient
{
public:
	virtual void ProcessProtocol(unsigned int sid, Protocol& p)
	{
		LOG << p.GetType();
	}


	virtual void OnConnected()
	{
		LOG << "OnConnected";
		for (int i=0; i<10; ++i)
		{
			gnet::CreateDsoundBuffer p;
			p.gameid_ = 123;
			p.userid_ = 4567;
			Send(p);
		}
	}


	virtual void OnClosed()
	{
		LOG << "OnClosed";
	}
};
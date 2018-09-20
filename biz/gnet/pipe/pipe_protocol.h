#pragma once

#include "gnet/protocol.h"
#include "gnet/pipe/spipe/pipe.h"
#include "bizutility/string_convert.h"


namespace gnet
{
	class PipeObject : public gnet::ProtocolProcessor, public IPipeListener
	{
	public:
		PipeObject() : ALLOW_THIS_IN_INITIALIZER_LIST(_pipImp(this)){}

		// from PipeListener
		// OnMessageReceived -> ProcessOctets -> ProcessProtocol
		virtual void OnMessageReceived(std::string data);

		// virtual void ProcessProtocol(unsigned int sid, Protocol&) = 0; to implement.
		bool Send(const gnet::Protocol& protocol);


	protected:
		Pipe	_pipImp;
	};


	class PipeServer : public PipeObject
	{
	public:
		PipeServer() : _bClose(false){}

		bool Connect(const std::wstring& pipe_name);
		void Close();

		// listener
		virtual void OnClosed();	// OnClosed -> OnServerClose, if m_bClose.
		virtual void OnServerClose() = 0; 


	private:
		bool	_bClose;
	};


	class PipeClient : public PipeObject
	{
	public:
		bool Connect(const std::wstring& pipe_name, int timeout, volatile bool& stopNow);// milsec
		void Close(){ _pipImp.Close(); }

		// listener
		virtual void OnClosed(){}
	};
}
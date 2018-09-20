#include "pipe_protocol.h"
#include "gnet/log_gnet.h"
#include "bizutility/log/assert_log.h"


namespace gnet
{
	//////////////////////////////////////////////////////////////////////////
	void PipeObject::OnMessageReceived(std::string data)
	{
		gnet::Octets input(data.data(), data.size());
		ProcessOctets(0, input);
	}


	bool PipeObject::Send(const gnet::Protocol& protocol)
	{
		std::string info = "Send protocol: " + biz_utility::tostring(protocol.GetType()) + " is not valid";
		bool bRet = protocol.IsOK();
		*assert_(bRet) << info;
		if (bRet)
		{
			gnet::Octets oct = protocol.Encode();
			_pipImp.Send(new Message((const char*)oct.begin(), oct.size()));
			return true;
		}
		else
			return false;
	}


	////////////////////// PipeServer ////////////////////////////////////////////////////
	bool PipeServer::Connect(const std::wstring& pipe_name)
	{
		_bClose = false;
		return _pipImp.Connect4Server(pipe_name);
	}


	void PipeServer::Close()
	{
		_bClose = true;
		_pipImp.Close();
	}


	void PipeServer::OnClosed()
	{
		OnServerClose();

		// 重新连接
		if (_bClose)
			return;

		LOG << " - server reconnect";
		_pipImp.Connect4Server();	// 自动重新等待新的连接
	}


	////////////////////// PipeClient ////////////////////////////////////////////////////
	bool PipeClient::Connect(const std::wstring& pipe_name, int timeout, volatile bool& stopNow)
	{
		int iRet = _pipImp.Connect4Client(pipe_name);
		while(1 == iRet && timeout > 0 && !stopNow)
		{
			Sleep(100);
			iRet = _pipImp.Connect4Client(pipe_name);
			timeout -= 100;
		}
		return 0 == iRet;
	}
}
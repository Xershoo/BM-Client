#pragma once

#include "gnet/protocol.h"
#include "gnet/socket/net.h"
#include "bizutility/string_convert.h"
#include "bizutility/log/assert_log.h"


namespace gnet
{
	////////////////////  Session  //////////////////////////////////////////////////////
	class SessionManager : public AioManager, public ProtocolProcessor
	{
	public:
		// Aio::Process -> this.Process -> ProcessOctets -> this.ProcessProtocol
		// from AioManager
		virtual void Process(Aio::SID sid, Octets& input)
		{
			ProcessOctets(sid, input);
		}
		// from IProtocolProcessor
		// virtual void ProcessProtocol(unsigned int sid, Protocol& p); to implement.


		// ��AioManager::Send, PostDelayProtocolһ�����˷�������ȫ��.
		void Send(Aio::SID sid, const Protocol& protocol)
		{
			std::string info = "Send protocol: " + biz_utility::tostring(protocol.GetType()) + " is not valid";
			bool bRet = protocol.IsOK();
			assert_(bRet);// << info.c_str();	����ʹ���������,ͷ�ļ���log��Σ��.
			if (bRet)
				AioManager::Send(sid, protocol.Encode());
		}
	};


	// ��Timerʵ���ӳٷ���Э��
	class DelayPostProtocol : public AioTimer
	{
	public:
		DelayPostProtocol(Aio::SID sid, Protocol* p)
			:m_sid(sid), m_protocol(p){}
		virtual void OnAioTimer()
		{
			Aio* aio = Engine::GetInst()->FindAioBySID(m_sid);
			assert_(aio);
			if (!aio)
				return;
			aio->Send(m_protocol->Encode());
			delete	m_protocol;
			delete	this;
		}

		Aio::SID	m_sid;
		Protocol*	m_protocol;
	};

	inline void PostDelayProtocol(Aio::SID sid, Protocol* p, int millisec)
	{
		(new DelayPostProtocol(sid, p))->Start(millisec);
	}
}
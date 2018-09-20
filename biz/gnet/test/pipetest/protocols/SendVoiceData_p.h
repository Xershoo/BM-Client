#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class SendVoiceData : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2 };

		Octets		passport_;
		char		xxx_;

		SendVoiceData()
			: Protocol(PROTOCOL_TYPE),
			xxx_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << passport_;
			os << xxx_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> passport_;
			os >> xxx_;
			return os;
		}

		virtual Protocol* Clone() const { return new SendVoiceData(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
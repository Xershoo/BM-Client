#pragma once

#include "gnet\protocol.h"
#include "bean\NfcMessage_b.h"


namespace gnet
{
	class MesgResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 3005 };

		char		msgtype_;
		__int64		userid_;
		std::vector<NfcMessage>		message_;

		MesgResp()
			: Protocol(PROTOCOL_TYPE),
			msgtype_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << msgtype_;
			os << userid_;
			os << message_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> msgtype_;
			os >> userid_;
			os >> message_;
			return os;
		}

		virtual Protocol* Clone() const { return new MesgResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
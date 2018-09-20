#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class MesgReq : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 3004 };

		enum
		{
			System = 1,
			Personal = 2,
		};

		char		msgtype_;
		__int64		userid_;

		MesgReq()
			: Protocol(PROTOCOL_TYPE),
			msgtype_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << msgtype_;
			os << userid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> msgtype_;
			os >> userid_;
			return os;
		}

		virtual Protocol* Clone() const { return new MesgReq(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
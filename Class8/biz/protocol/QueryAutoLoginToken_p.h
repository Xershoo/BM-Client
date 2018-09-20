#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class QueryAutoLoginToken : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 3006 };

		__int64		userid_;
		int		devicetype_;
		Octets		devicename_;

		QueryAutoLoginToken()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			devicetype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << devicetype_;
			os << devicename_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> devicetype_;
			os >> devicename_;
			return os;
		}

		virtual Protocol* Clone() const { return new QueryAutoLoginToken(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
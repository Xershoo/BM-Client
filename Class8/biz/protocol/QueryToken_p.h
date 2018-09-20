#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class QueryToken : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2021 };

		int		sid_;
		__int64		userid_;

		QueryToken()
			: Protocol(PROTOCOL_TYPE),
			sid_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << sid_;
			os << userid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> sid_;
			os >> userid_;
			return os;
		}

		virtual Protocol* Clone() const { return new QueryToken(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
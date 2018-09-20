#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class QueryUserEdu : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2003 };

		int		sid_;
		__int64		userid_;

		QueryUserEdu()
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

		virtual Protocol* Clone() const { return new QueryUserEdu(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
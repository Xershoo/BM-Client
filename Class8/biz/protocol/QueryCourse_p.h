#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class QueryCourse : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2005 };

		__int64		userid_;
		int		device_;
		Octets		token_;
		char		netisp_;
		__int64		courseid_;

		QueryCourse()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			device_(0),
			netisp_(0),
			courseid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << device_;
			os << token_;
			os << netisp_;
			os << courseid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> device_;
			os >> token_;
			os >> netisp_;
			os >> courseid_;
			return os;
		}

		virtual Protocol* Clone() const { return new QueryCourse(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
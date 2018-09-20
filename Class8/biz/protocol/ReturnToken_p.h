#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class ReturnToken : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2022 };

		int		sid_;
		__int64		userid_;
		int		retcode_;
		Octets		token_;
		int		validatetime_;

		ReturnToken()
			: Protocol(PROTOCOL_TYPE),
			sid_(0),
			userid_(0),
			retcode_(0),
			validatetime_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << sid_;
			os << userid_;
			os << retcode_;
			os << token_;
			os << validatetime_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> sid_;
			os >> userid_;
			os >> retcode_;
			os >> token_;
			os >> validatetime_;
			return os;
		}

		virtual Protocol* Clone() const { return new ReturnToken(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
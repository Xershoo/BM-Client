#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class AutoLoginTokenRsp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 3007 };

		int		retcode_;
		__int64		userid_;
		Octets		token_;

		AutoLoginTokenRsp()
			: Protocol(PROTOCOL_TYPE),
			retcode_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << retcode_;
			os << userid_;
			os << token_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> retcode_;
			os >> userid_;
			os >> token_;
			return os;
		}

		virtual Protocol* Clone() const { return new AutoLoginTokenRsp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
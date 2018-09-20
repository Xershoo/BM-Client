#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class TokenValidateResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 206 };

		int		linksid_;
		int		retcode_;
		__int64		userid_;

		TokenValidateResp()
			: Protocol(PROTOCOL_TYPE),
			linksid_(0),
			retcode_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << linksid_;
			os << retcode_;
			os << userid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> linksid_;
			os >> retcode_;
			os >> userid_;
			return os;
		}

		virtual Protocol* Clone() const { return new TokenValidateResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
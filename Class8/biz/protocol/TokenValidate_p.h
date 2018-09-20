#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class TokenValidate : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 205 };

		enum
		{
			PC = 0,
			IOS = 1,
			WEB = 2,
			ANDROID = 3,
		};

		int		linksid_;
		__int64		userid_;
		int		devicetype_;
		Octets		devicename_;
		Octets		token_;

		TokenValidate()
			: Protocol(PROTOCOL_TYPE),
			linksid_(0),
			userid_(0),
			devicetype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << linksid_;
			os << userid_;
			os << devicetype_;
			os << devicename_;
			os << token_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> linksid_;
			os >> userid_;
			os >> devicetype_;
			os >> devicename_;
			os >> token_;
			return os;
		}

		virtual Protocol* Clone() const { return new TokenValidate(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
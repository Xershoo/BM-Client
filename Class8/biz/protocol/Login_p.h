#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class Login : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 201 };

		enum
		{
			PC = 0,
			IOS = 1,
			WEB = 2,
			ANDROID = 3,
			NAMEPASSWD = 3,
			TOKEN = 4,
		};

		int		sid_;
		int		logintype_;
		Octets		username_;
		Octets		passwordmd5_;
		int		devicetype_;
		Octets		devicename_;
		__int64		userid_;
		Octets		token_;
		Octets		ip_;

		Login()
			: Protocol(PROTOCOL_TYPE),
			sid_(0),
			logintype_(0),
			devicetype_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << sid_;
			os << logintype_;
			os << username_;
			os << passwordmd5_;
			os << devicetype_;
			os << devicename_;
			os << userid_;
			os << token_;
			os << ip_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> sid_;
			os >> logintype_;
			os >> username_;
			os >> passwordmd5_;
			os >> devicetype_;
			os >> devicename_;
			os >> userid_;
			os >> token_;
			os >> ip_;
			return os;
		}

		virtual Protocol* Clone() const { return new Login(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
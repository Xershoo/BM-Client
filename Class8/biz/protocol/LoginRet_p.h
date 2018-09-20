#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class LoginRet : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 202 };

		enum
		{
			OK = 0,
			INVALID_PARAM = -1,
			INVALID_USERNAME = -2,
			INVALID_PASSWORD = -3,
			INVALID_TOKEN = -4,
			UIDDEVICE_NOT_EXIST = -5,
			USER_FROZEN = -6,
			LOGIN_ERR_MUCH_OF_DAY = -7,
			SERVER_ERROR = -100,
			CNC = 0,
			CT = 1,
			CNCANDCT = 2,
			OTHER = 3,
		};

		int		sid_;
		int		retcode_;
		__int64		userid_;
		int		usertype_;
		Octets		token_;
		Octets		link_;
		char		netisp_;

		LoginRet()
			: Protocol(PROTOCOL_TYPE),
			sid_(0),
			retcode_(0),
			userid_(0),
			usertype_(0),
			netisp_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << sid_;
			os << retcode_;
			os << userid_;
			os << usertype_;
			os << token_;
			os << link_;
			os << netisp_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> sid_;
			os >> retcode_;
			os >> userid_;
			os >> usertype_;
			os >> token_;
			os >> link_;
			os >> netisp_;
			return os;
		}

		virtual Protocol* Clone() const { return new LoginRet(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
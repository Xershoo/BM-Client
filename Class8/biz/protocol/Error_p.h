#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class Error : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1013 };

		enum
		{
			CLASS_SERVER_DISCONNECT = 1,
			CLASS_NOT_SAVE = 2,
			CLASS_NOT_EXIST = 3,
		};

		int		errtype_;
		__int64		userid_;
		__int64		cid_;
		int		device_;
		Octets		token_;
		char		netisp_;

		Error()
			: Protocol(PROTOCOL_TYPE),
			errtype_(0),
			userid_(0),
			cid_(0),
			device_(0),
			netisp_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << errtype_;
			os << userid_;
			os << cid_;
			os << device_;
			os << token_;
			os << netisp_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> errtype_;
			os >> userid_;
			os >> cid_;
			os >> device_;
			os >> token_;
			os >> netisp_;
			return os;
		}

		virtual Protocol* Clone() const { return new Error(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class MobileConnectClassReq : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1034 };

		__int64		userid_;
		Octets		devicename_;
		Octets		code_;

		MobileConnectClassReq()
			: Protocol(PROTOCOL_TYPE),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << devicename_;
			os << code_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> devicename_;
			os >> code_;
			return os;
		}

		virtual Protocol* Clone() const { return new MobileConnectClassReq(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
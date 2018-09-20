#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class KickOut : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1016 };

		__int64		userid_;

		KickOut()
			: Protocol(PROTOCOL_TYPE),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			return os;
		}

		virtual Protocol* Clone() const { return new KickOut(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
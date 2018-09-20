#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class UserLeave : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1003 };

		__int64		receiver_;
		__int64		userid_;
		__int64		cid_;
		__int64		intime_;

		UserLeave()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			userid_(0),
			cid_(0),
			intime_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << userid_;
			os << cid_;
			os << intime_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> userid_;
			os >> cid_;
			os >> intime_;
			return os;
		}

		virtual Protocol* Clone() const { return new UserLeave(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
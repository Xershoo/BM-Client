#pragma once

#include "gnet\protocol.h"
#include "bean\UserInfo_b.h"


namespace gnet
{
	class UserEnter : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1001 };

		__int64		receiver_;
		__int64		cid_;
		__int64		classid_;
		int		device_;
		char		netisp_;
		UserInfo		userinfo_;

		UserEnter()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			cid_(0),
			classid_(0),
			device_(0),
			netisp_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << cid_;
			os << classid_;
			os << device_;
			os << netisp_;
			os << userinfo_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> cid_;
			os >> classid_;
			os >> device_;
			os >> netisp_;
			os >> userinfo_;
			return os;
		}

		virtual Protocol* Clone() const { return new UserEnter(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
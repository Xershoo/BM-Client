#pragma once

#include "gnet\protocol.h"
#include "bean\UserInfo_b.h"


namespace gnet
{
	class ReturnUser : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2002 };

		int		sid_;
		int		retcode_;
		UserInfo		userinfo_;

		ReturnUser()
			: Protocol(PROTOCOL_TYPE),
			sid_(0),
			retcode_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << sid_;
			os << retcode_;
			os << userinfo_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> sid_;
			os >> retcode_;
			os >> userinfo_;
			return os;
		}

		virtual Protocol* Clone() const { return new ReturnUser(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
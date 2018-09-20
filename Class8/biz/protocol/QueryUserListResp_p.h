#pragma once

#include "gnet\protocol.h"
#include "bean\UserInfo_b.h"


namespace gnet
{
	class QueryUserListResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1026 };

		__int64		userid_;
		__int64		cid_;
		std::vector<UserInfo>		userlist_;

		QueryUserListResp()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << cid_;
			os << userlist_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> cid_;
			os >> userlist_;
			return os;
		}

		virtual Protocol* Clone() const { return new QueryUserListResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
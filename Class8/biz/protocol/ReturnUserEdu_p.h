#pragma once

#include "gnet\protocol.h"
#include "bean\UserEduInfo_b.h"


namespace gnet
{
	class ReturnUserEdu : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2004 };

		int		sid_;
		int		retcode_;
		__int64		userid_;
		std::vector<UserEduInfo>		eduinfolist_;

		ReturnUserEdu()
			: Protocol(PROTOCOL_TYPE),
			sid_(0),
			retcode_(0),
			userid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << sid_;
			os << retcode_;
			os << userid_;
			os << eduinfolist_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> sid_;
			os >> retcode_;
			os >> userid_;
			os >> eduinfolist_;
			return os;
		}

		virtual Protocol* Clone() const { return new ReturnUserEdu(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
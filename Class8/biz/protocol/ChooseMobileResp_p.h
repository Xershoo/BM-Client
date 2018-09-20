#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class ChooseMobileResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1039 };

		int		ret_;
		__int64		tid_;
		__int64		userid_;
		char		actiontype_;

		ChooseMobileResp()
			: Protocol(PROTOCOL_TYPE),
			ret_(0),
			tid_(0),
			userid_(0),
			actiontype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << ret_;
			os << tid_;
			os << userid_;
			os << actiontype_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> ret_;
			os >> tid_;
			os >> userid_;
			os >> actiontype_;
			return os;
		}

		virtual Protocol* Clone() const { return new ChooseMobileResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
#pragma once

#include "gnet\protocol.h"
#include "bean\ShowInfo_b.h"


namespace gnet
{
	class SwitchClassShow : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1004 };

		__int64		receiver_;
		__int64		userid_;
		__int64		cid_;
		ShowInfo		currentshow_;

		SwitchClassShow()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			userid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << userid_;
			os << cid_;
			os << currentshow_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> userid_;
			os >> cid_;
			os >> currentshow_;
			return os;
		}

		virtual Protocol* Clone() const { return new SwitchClassShow(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
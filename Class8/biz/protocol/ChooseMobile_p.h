#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class ChooseMobile : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1038 };

		enum
		{
			STOP = 0,
			CHOOSE = 1,
		};

		__int64		tid_;
		__int64		userid_;
		char		actiontype_;
		Octets		pushaddr_;

		ChooseMobile()
			: Protocol(PROTOCOL_TYPE),
			tid_(0),
			userid_(0),
			actiontype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << tid_;
			os << userid_;
			os << actiontype_;
			os << pushaddr_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> tid_;
			os >> userid_;
			os >> actiontype_;
			os >> pushaddr_;
			return os;
		}

		virtual Protocol* Clone() const { return new ChooseMobile(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
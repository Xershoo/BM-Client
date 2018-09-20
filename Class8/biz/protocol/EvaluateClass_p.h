#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class EvaluateClass : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1014 };

		__int64		userid_;
		__int64		cid_;
		int		rank_;
		Octets		description_;

		EvaluateClass()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			cid_(0),
			rank_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << cid_;
			os << rank_;
			os << description_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> cid_;
			os >> rank_;
			os >> description_;
			return os;
		}

		virtual Protocol* Clone() const { return new EvaluateClass(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
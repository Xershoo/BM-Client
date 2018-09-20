#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class CreateDsoundBuffer : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1 };

		__int64		userid_;
		int		gameid_;

		CreateDsoundBuffer()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			gameid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << gameid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> gameid_;
			return os;
		}

		virtual Protocol* Clone() const { return new CreateDsoundBuffer(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
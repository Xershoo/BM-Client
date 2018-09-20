#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class CancelAssistant : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1042 };

		__int64		receiver_;
		__int64		tid_;
		__int64		userid_;
		__int64		cid_;

		CancelAssistant()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			tid_(0),
			userid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << tid_;
			os << userid_;
			os << cid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> tid_;
			os >> userid_;
			os >> cid_;
			return os;
		}

		virtual Protocol* Clone() const { return new CancelAssistant(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class QueryPreCWares : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1023 };

		__int64		tid_;
		__int64		cid_;

		QueryPreCWares()
			: Protocol(PROTOCOL_TYPE),
			tid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << tid_;
			os << cid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> tid_;
			os >> cid_;
			return os;
		}

		virtual Protocol* Clone() const { return new QueryPreCWares(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
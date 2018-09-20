#pragma once

#include "gnet\protocol.h"
#include "bean\PreCourseInfo_b.h"


namespace gnet
{
	class QueryPreCWaresResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1024 };

		__int64		tid_;
		__int64		cid_;
		std::vector<PreCourseInfo>		courselist_;

		QueryPreCWaresResp()
			: Protocol(PROTOCOL_TYPE),
			tid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << tid_;
			os << cid_;
			os << courselist_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> tid_;
			os >> cid_;
			os >> courselist_;
			return os;
		}

		virtual Protocol* Clone() const { return new QueryPreCWaresResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
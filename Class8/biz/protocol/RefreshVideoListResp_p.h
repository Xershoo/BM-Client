#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class RefreshVideoListResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1033 };

		__int64		receiver_;
		__int64		cid_;
		std::vector<__int64>		topvideolist_;

		RefreshVideoListResp()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << cid_;
			os << topvideolist_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> cid_;
			os >> topvideolist_;
			return os;
		}

		virtual Protocol* Clone() const { return new RefreshVideoListResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
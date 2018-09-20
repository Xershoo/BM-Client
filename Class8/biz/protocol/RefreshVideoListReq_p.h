#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class RefreshVideoListReq : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1032 };

		__int64		cid_;
		__int64		teacherid_;

		RefreshVideoListReq()
			: Protocol(PROTOCOL_TYPE),
			cid_(0),
			teacherid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << cid_;
			os << teacherid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> cid_;
			os >> teacherid_;
			return os;
		}

		virtual Protocol* Clone() const { return new RefreshVideoListReq(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
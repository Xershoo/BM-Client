#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class SetTeacherVedio : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1027 };

		__int64		receiver_;
		__int64		userid_;
		__int64		cid_;
		int		teachervedio_;

		SetTeacherVedio()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			userid_(0),
			cid_(0),
			teachervedio_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << userid_;
			os << cid_;
			os << teachervedio_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> userid_;
			os >> cid_;
			os >> teachervedio_;
			return os;
		}

		virtual Protocol* Clone() const { return new SetTeacherVedio(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
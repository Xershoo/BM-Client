#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class ClassGoOn : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1048 };

		__int64		courseid_;
		__int64		classid_;
		__int64		teacher_;

		ClassGoOn()
			: Protocol(PROTOCOL_TYPE),
			courseid_(0),
			classid_(0),
			teacher_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << courseid_;
			os << classid_;
			os << teacher_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> courseid_;
			os >> classid_;
			os >> teacher_;
			return os;
		}

		virtual Protocol* Clone() const { return new ClassGoOn(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
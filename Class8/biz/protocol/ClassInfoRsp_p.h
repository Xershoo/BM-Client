#pragma once

#include "gnet\protocol.h"
#include "bean\CourseInfo_b.h"


namespace gnet
{
	class ClassInfoRsp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1015 };

		CourseInfo		courseinfo_;

		ClassInfoRsp()
			: Protocol(PROTOCOL_TYPE){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << courseinfo_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> courseinfo_;
			return os;
		}

		virtual Protocol* Clone() const { return new ClassInfoRsp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
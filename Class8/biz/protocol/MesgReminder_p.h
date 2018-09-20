#pragma once

#include "gnet\protocol.h"
#include "bean\ClassInfoBean_b.h"


namespace gnet
{
	class MesgReminder : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1046 };

		enum
		{
			SIGN = 1,
			CLASS_ON = 2,
			CLASS_DELAY = 3,
			CLASS_END = 4,
		};

		__int64		receiver_;
		char		msgtype_;
		ClassInfoBean		classinfo_;

		MesgReminder()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			msgtype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << msgtype_;
			os << classinfo_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> msgtype_;
			os >> classinfo_;
			return os;
		}

		virtual Protocol* Clone() const { return new MesgReminder(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
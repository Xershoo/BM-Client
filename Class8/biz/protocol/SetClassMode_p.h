#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class SetClassMode : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1009 };

		enum
		{
			UNSPEAKABLE = 0,
			SPEAKABLE = 1,
			TEXTABLE = 2,
			UTEXTABLE = 3,
			ASIDEABLE = 4,
			UNASIDEABLE = 5,
			SEND_PIC = 8,
			UNSEND_PIC = 9,
			LOCK = 16,
			UNLOCK = 17,
		};

		__int64		receiver_;
		__int64		userid_;
		__int64		cid_;
		char		classmode_;

		SetClassMode()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			userid_(0),
			cid_(0),
			classmode_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << userid_;
			os << cid_;
			os << classmode_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> userid_;
			os >> cid_;
			os >> classmode_;
			return os;
		}

		virtual Protocol* Clone() const { return new SetClassMode(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
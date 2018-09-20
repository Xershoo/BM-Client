#pragma once

#include "gnet\protocol.h"
#include "bean\WhiteBoardAction_b.h"


namespace gnet
{
	class WhiteBoardEvent : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1012 };

		enum
		{
			PEN = 1,
			ERASOR = 2,
			TXT = 3,
			LASER_POINT = 4,
			UNDO = 103,
		};

		__int64		receiver_;
		WhiteBoardAction		action_;

		WhiteBoardEvent()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << action_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> action_;
			return os;
		}

		virtual Protocol* Clone() const { return new WhiteBoardEvent(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
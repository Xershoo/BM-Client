#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class Kick : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1040 };

		enum
		{
			DELETE_DEVICE = 0,
			CLASS_END = 1,
			MOBILE_OFF = 2,
			TEACHER_LEAVE = 3,
		};

		__int64		tid_;
		__int64		userid_;
		char		notifytype_;

		Kick()
			: Protocol(PROTOCOL_TYPE),
			tid_(0),
			userid_(0),
			notifytype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << tid_;
			os << userid_;
			os << notifytype_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> tid_;
			os >> userid_;
			os >> notifytype_;
			return os;
		}

		virtual Protocol* Clone() const { return new Kick(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
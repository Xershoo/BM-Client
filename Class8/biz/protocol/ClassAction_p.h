#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class ClassAction : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1011 };

		enum
		{
			ASK_SPEAK = 1,
			CANCEL_SPEAK = 2,
			ALLOW_SPEAK = 3,
			CLEAN_SPEAK = 4,
			KICKOUT = 5,
			ADD_STUDENT_VIDEO = 6,
			DEL_STUDENT_VIDEO = 7,
			OPEN_VOICE = 8,
			CLOSE_VOICE = 9,
			OPEN_VIDEO = 10,
			CLOSE_VIDEO = 11,
			MUTE = 12,
			UNMUTE = 13,
			ENTER_GROUP = 14,
			LEAVE_GROUP = 15,
			CALL_TEACHER = 16,
		};

		__int64		receiver_;
		char		actiontype_;
		__int64		userid_;
		__int64		cid_;
		__int64		teacheruid_;

		ClassAction()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			actiontype_(0),
			userid_(0),
			cid_(0),
			teacheruid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << actiontype_;
			os << userid_;
			os << cid_;
			os << teacheruid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> actiontype_;
			os >> userid_;
			os >> cid_;
			os >> teacheruid_;
			return os;
		}

		virtual Protocol* Clone() const { return new ClassAction(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
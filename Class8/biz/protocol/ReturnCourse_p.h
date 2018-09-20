#pragma once

#include "gnet\protocol.h"
#include "bean\CourseInfo_b.h"


namespace gnet
{
	class ReturnCourse : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2006 };

		enum
		{
			OK = 0,
			COURSE_ERROR = -1,
		};

		__int64		userid_;
		int		device_;
		Octets		token_;
		char		netisp_;
		int		retcode_;
		CourseInfo		course_;

		ReturnCourse()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			device_(0),
			netisp_(0),
			retcode_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << device_;
			os << token_;
			os << netisp_;
			os << retcode_;
			os << course_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> device_;
			os >> token_;
			os >> netisp_;
			os >> retcode_;
			os >> course_;
			return os;
		}

		virtual Protocol* Clone() const { return new ReturnCourse(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
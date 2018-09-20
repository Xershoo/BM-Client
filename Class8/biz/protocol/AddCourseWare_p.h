#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class AddCourseWare : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1006 };

		enum
		{
			ADD = 1,
			DEL = 2,
			CLOSE = 3,
			CLIENT = 1,
			WEBSERVER = 2,
		};

		__int64		receiver_;
		char		actiontype_;
		char		sender_;
		__int64		userid_;
		__int64		cid_;
		Octets		cwtype_;
		Octets		cwname_;

		AddCourseWare()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			actiontype_(0),
			sender_(0),
			userid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << actiontype_;
			os << sender_;
			os << userid_;
			os << cid_;
			os << cwtype_;
			os << cwname_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> actiontype_;
			os >> sender_;
			os >> userid_;
			os >> cid_;
			os >> cwtype_;
			os >> cwname_;
			return os;
		}

		virtual Protocol* Clone() const { return new AddCourseWare(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
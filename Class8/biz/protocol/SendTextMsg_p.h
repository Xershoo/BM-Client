#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class SendTextMsg : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1010 };

		enum
		{
			CLASS = 1,
			GROUP = 2,
			USER = 3,
		};

		__int64		receiver_;
		char		recvtype_;
		__int64		userid_;
		__int64		recvid_;
		__int64		cid_;
		int		recvgroupid_;
		Octets		message_;
		__int64		time_;

		SendTextMsg()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			recvtype_(0),
			userid_(0),
			recvid_(0),
			cid_(0),
			recvgroupid_(0),
			time_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << recvtype_;
			os << userid_;
			os << recvid_;
			os << cid_;
			os << recvgroupid_;
			os << message_;
			os << time_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> recvtype_;
			os >> userid_;
			os >> recvid_;
			os >> cid_;
			os >> recvgroupid_;
			os >> message_;
			os >> time_;
			return os;
		}

		virtual Protocol* Clone() const { return new SendTextMsg(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
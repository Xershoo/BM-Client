#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class MobileConnectClassResp : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1035 };

		enum
		{
			OK = 0,
			CODE_INVALID = 1,
			TEACHER_NOT_EXIST = 2,
			CONNECTION_OUT_OF_RANGE = 3,
		};

		__int64		receiver_;
		int		ret_;
		__int64		userid_;
		__int64		tid_;
		Octets		devicename_;
		__int64		cid_;

		MobileConnectClassResp()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			ret_(0),
			userid_(0),
			tid_(0),
			cid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << ret_;
			os << userid_;
			os << tid_;
			os << devicename_;
			os << cid_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> ret_;
			os >> userid_;
			os >> tid_;
			os >> devicename_;
			os >> cid_;
			return os;
		}

		virtual Protocol* Clone() const { return new MobileConnectClassResp(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
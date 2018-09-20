#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class SetClassState : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1008 };

		enum
		{
			CLASS_BEGIN = 2,
			CLASS_END = 4,
		};

		__int64		receiver_;
		int		ret_;
		__int64		userid_;
		__int64		cid_;
		__int64		classid_;
		char		classstate_;

		SetClassState()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			ret_(0),
			userid_(0),
			cid_(0),
			classid_(0),
			classstate_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << ret_;
			os << userid_;
			os << cid_;
			os << classid_;
			os << classstate_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> ret_;
			os >> userid_;
			os >> cid_;
			os >> classid_;
			os >> classstate_;
			return os;
		}

		virtual Protocol* Clone() const { return new SetClassState(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class SetMainShow : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1052 };

		enum
		{
			CW_WB = 1,
			VEDIO = 2,
		};

		__int64		receiver_;
		__int64		teacher_;
		__int64		classid_;
		char		showtype_;

		SetMainShow()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			teacher_(0),
			classid_(0),
			showtype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << teacher_;
			os << classid_;
			os << showtype_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> teacher_;
			os >> classid_;
			os >> showtype_;
			return os;
		}

		virtual Protocol* Clone() const { return new SetMainShow(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
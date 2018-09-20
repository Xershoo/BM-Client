#pragma once

#include "gnet\protocol.h"
#include "bean\UIDList_b.h"


namespace gnet
{
	class CreateTalkGroup : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1007 };

		enum
		{
			START = 1,
			END = 2,
			PRECREATE = 3,
			CANCEL = 4,
		};

		__int64		receiver_;
		int		ret_;
		char		actionytype_;
		__int64		userid_;
		__int64		cid_;
		int		membercount_;
		std::map<int, UIDList>		grouplist_;
		Octets		topic_;

		CreateTalkGroup()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			ret_(0),
			actionytype_(0),
			userid_(0),
			cid_(0),
			membercount_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << ret_;
			os << actionytype_;
			os << userid_;
			os << cid_;
			os << membercount_;
			os << grouplist_;
			os << topic_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> ret_;
			os >> actionytype_;
			os >> userid_;
			os >> cid_;
			os >> membercount_;
			os >> grouplist_;
			os >> topic_;
			return os;
		}

		virtual Protocol* Clone() const { return new CreateTalkGroup(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
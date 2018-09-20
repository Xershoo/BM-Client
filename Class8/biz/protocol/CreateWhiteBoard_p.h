#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class CreateWhiteBoard : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1005 };

		enum
		{
			ADD = 1,
			DEL = 2,
			MODIFY = 3,
		};

		__int64		receiver_;
		char		actionytype_;
		__int64		userid_;
		__int64		cid_;
		int		wbid_;
		Octets		wbname_;

		CreateWhiteBoard()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			actionytype_(0),
			userid_(0),
			cid_(0),
			wbid_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << actionytype_;
			os << userid_;
			os << cid_;
			os << wbid_;
			os << wbname_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> actionytype_;
			os >> userid_;
			os >> cid_;
			os >> wbid_;
			os >> wbname_;
			return os;
		}

		virtual Protocol* Clone() const { return new CreateWhiteBoard(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
#pragma once

#include "gnet\protocol.h"


namespace gnet
{
	class FeedBack : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 2023 };

		__int64		userid_;
		int		fbtype_;
		Octets		title_;
		Octets		content_;
		Octets		mobile_;

		FeedBack()
			: Protocol(PROTOCOL_TYPE),
			userid_(0),
			fbtype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << fbtype_;
			os << title_;
			os << content_;
			os << mobile_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> fbtype_;
			os >> title_;
			os >> content_;
			os >> mobile_;
			return os;
		}

		virtual Protocol* Clone() const { return new FeedBack(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
#pragma once

#include "gnet\protocol.h"$(include)


namespace gnet
{
	class $(name) : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = $(type) };

		$(var_list)

		$(name)()
			: Protocol(PROTOCOL_TYPE)$(var_init){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			$(var_<<)
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			$(var_>>)
			return os;
		}

		virtual Protocol* Clone() const { return new $(name)(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
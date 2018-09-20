#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class UIDList : public Marshal
	{
	public:
		std::vector<__int64>		uidlist_;

		UIDList(){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << uidlist_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> uidlist_;
			return os;
		}
	};
}
#pragma once

#include "gnet\marshal.h"$(include)


namespace gnet
{
	class $(name) : public Marshal
	{
	public:
		$(var_list)

		$(name)()$(var_init){}

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
	};
}
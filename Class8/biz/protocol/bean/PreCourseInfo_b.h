#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class PreCourseInfo : public Marshal
	{
	public:
		Octets		name_;
		Octets		url_;

		PreCourseInfo(){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << name_;
			os << url_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> name_;
			os >> url_;
			return os;
		}
	};
}
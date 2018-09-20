#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class ShowInfo : public Marshal
	{
	public:
		enum
		{
			BLANK = 0,
			COURSEWARE = 1,
			WHITEBOARD = 2,
		};

		char		showtype_;
		Octets		name_;
		int		page_;

		ShowInfo()
			: showtype_(0),
			page_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << showtype_;
			os << name_;
			os << page_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> showtype_;
			os >> name_;
			os >> page_;
			return os;
		}
	};
}
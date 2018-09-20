#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class ClassInfoBean : public Marshal
	{
	public:
		enum
		{
			CLASS_WAIT_ON = 1,
			CLASS_ON_NOT_BEGIN = 2,
			CLASS_ON_AND_BEGIN = 4,
		};

		__int64		courseid_;
		__int64		classid_;
		Octets		classname_;
		Octets		userheadurl_;
		Octets		code_;
		char		classstate_;
		int		durationtime_;
		int		record_;

		ClassInfoBean()
			: courseid_(0),
			classid_(0),
			classstate_(0),
			durationtime_(0),
			record_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << courseid_;
			os << classid_;
			os << classname_;
			os << userheadurl_;
			os << code_;
			os << classstate_;
			os << durationtime_;
			os << record_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> courseid_;
			os >> classid_;
			os >> classname_;
			os >> userheadurl_;
			os >> code_;
			os >> classstate_;
			os >> durationtime_;
			os >> record_;
			return os;
		}
	};
}
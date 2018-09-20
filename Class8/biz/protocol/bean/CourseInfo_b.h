#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class CourseInfo : public Marshal
	{
	public:
		__int64		courseid_;
		Octets		name_;
		Octets		description_;
		Octets		userheadurl_;
		Octets		coverurl_;
		Octets		snapshoturl_;
		int		compulsorytype_;
		int		coursecredit_;
		__int64		teacherid_;
		Octets		hostuniversity_;
		Octets		hostcollege_;
		Octets		hostmajor_;
		Octets		schoolyear_;
		Octets		schoolterm_;
		int		onlinetype_;
		int		opentype_;
		Octets		offline_classroomaddress_;
		int		asidetype_;
		int		max_asidecount_;
		int		guesttype_;
		int		max_aguestcount_;
		std::vector<__int64>		studentsid_;
		Octets		code_;

		CourseInfo()
			: courseid_(0),
			compulsorytype_(0),
			coursecredit_(0),
			teacherid_(0),
			onlinetype_(0),
			opentype_(0),
			asidetype_(0),
			max_asidecount_(0),
			guesttype_(0),
			max_aguestcount_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << courseid_;
			os << name_;
			os << description_;
			os << userheadurl_;
			os << coverurl_;
			os << snapshoturl_;
			os << compulsorytype_;
			os << coursecredit_;
			os << teacherid_;
			os << hostuniversity_;
			os << hostcollege_;
			os << hostmajor_;
			os << schoolyear_;
			os << schoolterm_;
			os << onlinetype_;
			os << opentype_;
			os << offline_classroomaddress_;
			os << asidetype_;
			os << max_asidecount_;
			os << guesttype_;
			os << max_aguestcount_;
			os << studentsid_;
			os << code_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> courseid_;
			os >> name_;
			os >> description_;
			os >> userheadurl_;
			os >> coverurl_;
			os >> snapshoturl_;
			os >> compulsorytype_;
			os >> coursecredit_;
			os >> teacherid_;
			os >> hostuniversity_;
			os >> hostcollege_;
			os >> hostmajor_;
			os >> schoolyear_;
			os >> schoolterm_;
			os >> onlinetype_;
			os >> opentype_;
			os >> offline_classroomaddress_;
			os >> asidetype_;
			os >> max_asidecount_;
			os >> guesttype_;
			os >> max_aguestcount_;
			os >> studentsid_;
			os >> code_;
			return os;
		}
	};
}
#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class ClassInfo : public Marshal
	{
	public:
		__int64		classid_;
		__int64		courseid_;
		int		classseqnum_;
		int		starttime_;
		int		duration_;
		int		endtime_;
		int		dailyseqnum_;
		int		onlinetype_;
		int		asidetype_;
		int		max_asidecount_;
		int		guesttype_;
		int		max_aguestcount_;

		ClassInfo()
			: classid_(0),
			courseid_(0),
			classseqnum_(0),
			starttime_(0),
			duration_(0),
			endtime_(0),
			dailyseqnum_(0),
			onlinetype_(0),
			asidetype_(0),
			max_asidecount_(0),
			guesttype_(0),
			max_aguestcount_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << classid_;
			os << courseid_;
			os << classseqnum_;
			os << starttime_;
			os << duration_;
			os << endtime_;
			os << dailyseqnum_;
			os << onlinetype_;
			os << asidetype_;
			os << max_asidecount_;
			os << guesttype_;
			os << max_aguestcount_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> classid_;
			os >> courseid_;
			os >> classseqnum_;
			os >> starttime_;
			os >> duration_;
			os >> endtime_;
			os >> dailyseqnum_;
			os >> onlinetype_;
			os >> asidetype_;
			os >> max_asidecount_;
			os >> guesttype_;
			os >> max_aguestcount_;
			return os;
		}
	};
}
#pragma once

#include "gnet\protocol.h"
#include "bean\UserInfo_b.h"
#include "bean\WhiteBoardAction_b.h"
#include "bean\ShowInfo_b.h"


namespace gnet
{
	class UserWelcome : public Protocol
	{
	public:
		enum { PROTOCOL_TYPE = 1002 };

		enum
		{
			SPEAKABLE = 1,
			TEXTABLE = 2,
			ASIDEABLE = 4,
			CLASS_NOT_ON = 0,
			CLASS_WAIT_ON = 1,
			CLASS_ON_NOT_BEGIN = 2,
			CLASS_ON_AND_BEGIN = 4,
			CHAT_ALLOW = 1,
			SendIMAGE = 2,
			SUCCESS = 0,
			ENTERTIME_NOT_COME = 1,
			CLASS_END = 2,
			AUTHORITY_ERR = 3,
			BLACKLIST = 4,
			CLASSROOM_LOCKED = 5,
		};

		__int64		receiver_;
		int		ret_;
		__int64		cid_;
		__int64		classid_;
		Octets		cname_;
		Octets		userheadurl_;
		int		feedback_;
		std::vector<UserInfo>		userlist_;
		std::vector<__int64>		topvideolist_;
		std::vector<Octets>		cousewarelist_;
		std::vector<WhiteBoardAction>		whiteboard_;
		char		mainshow_;
		ShowInfo		currentshow_;
		char		classstate_;
		char		classmode_;
		int		timebeforeclass_;
		int		durationtime_;
		int		teachervedio_;
		Octets		code_;

		UserWelcome()
			: Protocol(PROTOCOL_TYPE),
			receiver_(0),
			ret_(0),
			cid_(0),
			classid_(0),
			feedback_(0),
			mainshow_(0),
			classstate_(0),
			classmode_(0),
			timebeforeclass_(0),
			durationtime_(0),
			teachervedio_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << receiver_;
			os << ret_;
			os << cid_;
			os << classid_;
			os << cname_;
			os << userheadurl_;
			os << feedback_;
			os << userlist_;
			os << topvideolist_;
			os << cousewarelist_;
			os << whiteboard_;
			os << mainshow_;
			os << currentshow_;
			os << classstate_;
			os << classmode_;
			os << timebeforeclass_;
			os << durationtime_;
			os << teachervedio_;
			os << code_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> receiver_;
			os >> ret_;
			os >> cid_;
			os >> classid_;
			os >> cname_;
			os >> userheadurl_;
			os >> feedback_;
			os >> userlist_;
			os >> topvideolist_;
			os >> cousewarelist_;
			os >> whiteboard_;
			os >> mainshow_;
			os >> currentshow_;
			os >> classstate_;
			os >> classmode_;
			os >> timebeforeclass_;
			os >> durationtime_;
			os >> teachervedio_;
			os >> code_;
			return os;
		}

		virtual Protocol* Clone() const { return new UserWelcome(*this); }

		//virtual void Process(unsigned int sid, ProtocolProcessor*);
	};
}
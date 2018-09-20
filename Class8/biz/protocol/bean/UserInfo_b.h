#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class UserInfo : public Marshal
	{
	public:
		enum
		{
			VISITOR = 1,
			OBSERVER = 10,
			MONITOR = 20,
			STUDENT = 30,
			ASSISTANT = 31,
			TEACHER = 40,
			ASK_SPEAK = 1,
			SPEAK = 2,
			USE_VOICE = 4,
			USE_VIDEO = 8,
			MUTE = 16,
			UNKNOW_GENDER = 0,
			MALE = 1,
			FEMALE = 2,
		};

		__int64		userid_;
		int		authority_;
		Octets		nickname_;
		Octets		realname_;
		Octets		signature_;
		int		gender_;
		Octets		pic_;
		int		bantype_;
		Octets		mobile_;
		Octets		email_;
		int		device_;
		int		state_;
		Octets		pushaddr_;
		Octets		pulladdr_;

		UserInfo()
			: userid_(0),
			authority_(0),
			gender_(0),
			bantype_(0),
			device_(0),
			state_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << authority_;
			os << nickname_;
			os << realname_;
			os << signature_;
			os << gender_;
			os << pic_;
			os << bantype_;
			os << mobile_;
			os << email_;
			os << device_;
			os << state_;
			os << pushaddr_;
			os << pulladdr_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> authority_;
			os >> nickname_;
			os >> realname_;
			os >> signature_;
			os >> gender_;
			os >> pic_;
			os >> bantype_;
			os >> mobile_;
			os >> email_;
			os >> device_;
			os >> state_;
			os >> pushaddr_;
			os >> pulladdr_;
			return os;
		}
	};
}
#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class UserEduInfo : public Marshal
	{
	public:
		enum
		{
			NOT_VERIFYED = 0,
			VERIFY_CODE_SEND = 1,
			VERIFYED = 100,
			VERIFY_FAILED = 99,
		};

		Octets		university_;
		Octets		college_;
		Octets		grade_;
		Octets		majorr_;
		Octets		studentno_;
		int		verifystatus_;

		UserEduInfo()
			: verifystatus_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << university_;
			os << college_;
			os << grade_;
			os << majorr_;
			os << studentno_;
			os << verifystatus_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> university_;
			os >> college_;
			os >> grade_;
			os >> majorr_;
			os >> studentno_;
			os >> verifystatus_;
			return os;
		}
	};
}
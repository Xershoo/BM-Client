#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class NfcMessage : public Marshal
	{
	public:
		Octets		title_;
		Octets		content_;
		Octets		url_;

		NfcMessage(){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << title_;
			os << content_;
			os << url_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> title_;
			os >> content_;
			os >> url_;
			return os;
		}
	};
}
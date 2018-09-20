#pragma once

#include "gnet\marshal.h"


namespace gnet
{
	class WhiteBoardAction : public Marshal
	{
	public:
		__int64		userid_;
		__int64		cid_;
		__int64		oweruid_;
		int		pageId_;
		int		paintId_;
		char		paintype_;
		Octets		arguments_;

		WhiteBoardAction()
			: userid_(0),
			cid_(0),
			oweruid_(0),
			pageId_(0),
			paintId_(0),
			paintype_(0){}

		OctetsStream& marshal(OctetsStream& os) const
		{
			os << userid_;
			os << cid_;
			os << oweruid_;
			os << pageId_;
			os << paintId_;
			os << paintype_;
			os << arguments_;
			return os;
		}

		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			os >> userid_;
			os >> cid_;
			os >> oweruid_;
			os >> pageId_;
			os >> paintId_;
			os >> paintype_;
			os >> arguments_;
			return os;
		}
	};
}
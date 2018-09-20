#pragma  once

#include "data.h"
#include "IClassRoomInfo.h"

namespace biz
{

	class IClassRoomDataContainer
	{
	public:
		virtual SLClassRoomBaseInfo	GetClassRoomBaseInfo(__int64 nRoomId) = 0;
		virtual IClassRoomInfo*     GetClassRoomInterface(__int64 nRoomId) = 0;
	};

}
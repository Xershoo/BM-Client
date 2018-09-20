#pragma  once

#include "../interface/data.h"
#include "../interface/IClassRoomInfoContainer.h"
#include "CLClassRoomInfo.h"
#include <map>

namespace biz
{
	class CLClassRoomInfoContainer : public IClassRoomDataContainer
	{
	public:
		virtual SLClassRoomBaseInfo	GetClassRoomBaseInfo(__int64 nRoomId);
		virtual IClassRoomInfo*     GetClassRoomInterface(__int64 nRoomId);

		CLClassRoomInfo*			UpdateRoomBaseInfo(SLClassRoomBaseInfo& sInfo);

		void						Clear(); //清除房间里的用户列表等信息，但课程信息不清除
	private:
		std::map<__int64, CLClassRoomInfo*>		_lstClassRoom;
	};
} //end namespace
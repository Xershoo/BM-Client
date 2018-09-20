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

		void						Clear(); //�����������û��б����Ϣ�����γ���Ϣ�����
	private:
		std::map<__int64, CLClassRoomInfo*>		_lstClassRoom;
	};
} //end namespace
#include "CLClassRoomInfoContainer.h"

namespace biz
{
SLClassRoomBaseInfo CLClassRoomInfoContainer::GetClassRoomBaseInfo(__int64 nRoomId)
{
	return SLClassRoomBaseInfo();
}

IClassRoomInfo* CLClassRoomInfoContainer::GetClassRoomInterface(__int64 nRoomId)
{
	auto iter = _lstClassRoom.find(nRoomId);
	if(iter == _lstClassRoom.end())
	{
		///TODO: 需要修改，如果没有该课堂，直接返回NULL
		CLClassRoomInfo* pRoom = new CLClassRoomInfo;
		_lstClassRoom.insert(std::make_pair(nRoomId, pRoom));
		return pRoom;
	}
	else
		return iter->second;
}

CLClassRoomInfo* CLClassRoomInfoContainer::UpdateRoomBaseInfo(SLClassRoomBaseInfo& sInfo)
{
	auto iter = _lstClassRoom.find(sInfo._nRoomId);
	CLClassRoomInfo* pRoom = NULL;
	if(iter == _lstClassRoom.end())
	{
		pRoom = new CLClassRoomInfo;		
		_lstClassRoom.insert(std::make_pair(sInfo._nRoomId, pRoom));
	}
	else
		pRoom = iter->second;
	
	pRoom->SetRoomBaseInfo(sInfo);
	return pRoom;
}

void CLClassRoomInfoContainer::Clear()
{
	for (auto iter = _lstClassRoom.begin(); iter != _lstClassRoom.end(); iter++)
	{
		iter->second->ResetClass();
	}
}

} //end namespace
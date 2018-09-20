#include "CLUserInfoData.h"
#include <algorithm>

namespace biz
{
	void CLUserInfoDataContainer::AddUserInfo(SLUserInfo& sInfo)
	{
		SLUserInfo* pUser = GetUserInfo(sInfo.nUserId);
		if(pUser)
		{
			UpdateUserINfo(sInfo);
			return;
		}

		_userList.push_back(sInfo);
	}

	SLUserInfo CLUserInfoDataContainer::GetUserInfoById(__int64 nUserId,bool* pbExist /* = NULL */)
	{		
		for (size_t i = 0; i < _userList.size(); ++i)
		{
			if(nUserId == _userList[i].nUserId)
			{
				if(pbExist)
				{
					*pbExist = true;
				}
				return _userList[i];
			}
		}

		if(pbExist)
		{
			*pbExist = false;
		}
		return SLUserInfo(nUserId);
	}

	SLUserInfo*	CLUserInfoDataContainer::GetUserInfoByName(const wchar_t* pName)
	{
		for (size_t i = 0; i < _userList.size(); ++i)
		{
			if(wcscmp(pName, _userList[i].szNickName) == 0)
				return &_userList[i];
		}

		return NULL;
	}

	void CLUserInfoDataContainer::UpdateUserINfo(SLUserInfo&sInfo)
	{
		for (size_t i = 0; i < _userList.size(); i++)
		{
			if(sInfo.nUserId == _userList[i].nUserId)
			{
				_userList[i] = sInfo;
				return;
			}
		}
		_userList.push_back(sInfo);
	}

	void CLUserInfoDataContainer::DeleteUserInfo(SLUserInfo& sInfo)
	{
		DeleteUser(sInfo.nUserId);
	}

	void CLUserInfoDataContainer::DeleteUser(__int64 nUserId)
	{
		for (auto iter = _userList.begin(); iter != _userList.end(); iter++)
		{
			if(nUserId == (*iter).nUserId)
			{
				_userList.erase(iter);
				return;
			}
		}
	}

	int	CLUserInfoDataContainer::GetUserInfoList(int nCount, SLUserInfo* pInfo) 
	{
		int nTotalCount = _userList.size();
		if(nCount == 0 ||pInfo == NULL)
			return nTotalCount;
		if(nTotalCount > nCount)
			nTotalCount = nCount;
		for (size_t i = 0; i < nTotalCount; i++)
		{
			SLUserInfo& tempInfo = _userList[i];
			memcpy(pInfo + i, &tempInfo, sizeof(SLUserInfo));
		}
		return nTotalCount;
	}

	void CLUserInfoDataContainer::SetUserRoom(__int64 nUserId, __int64 nRoomId)
	{
		SLUserInfo* pUser = GetUserInfo(nUserId);
		if(pUser)
			pUser->_nClassRoomId = nRoomId;
	}
	SLUserInfo* CLUserInfoDataContainer::GetUserInfo(__int64 nUserId)
	{
		for (size_t i = 0; i < _userList.size(); i++)
		{
			if(nUserId == _userList[i].nUserId)
				return &_userList[i];
		}
		return NULL;
	}

	void	CLUserInfoDataContainer::SetUserState(__int64 nUserId, int nState)
	{
		SLUserInfo* pInfo = GetUserInfo(nUserId);
		if(pInfo)
			pInfo->_nUserState = nState;
	}

	int		CLUserInfoDataContainer::GetUserState(__int64 nUserId)
	{
		SLUserInfo* pInfo = GetUserInfo(nUserId);
		if(pInfo)
			return pInfo->_nUserState;
		else
			return 0;
	}

	void	CLUserInfoDataContainer::CleanStudentsAskSpeak()
	{
		for (size_t i = 0; i < _userList.size(); i++)
		{
			if(_userList[i]._nUserState & eUserState_ask_speak)
				_userList[i]._nUserState = _userList[i]._nUserState & (~eUserState_ask_speak);
		}
	}

	void CLUserInfoDataContainer::Clear()
	{
		_userList.clear();
	}
	
}
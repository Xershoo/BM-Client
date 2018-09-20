#include "CLClassRoomInfo.h"
#include <algorithm>
#include "../biz.h"
#include "CLUserInfoData.h"
namespace biz
{
	void CLClassRoomInfo::SetRoomBaseInfo(SLClassRoomBaseInfo& sInfo)
	{
		memcpy(&_sRoomBaseInfo, &sInfo, sizeof(sInfo));
	}
	SLClassRoomBaseInfo& CLClassRoomInfo::GetRoomBaseInfo()
	{
		return _sRoomBaseInfo;
	}
	void CLClassRoomInfo::TopVideoListOpt(__int64 nUserId, int nOpt, int nIndex)
	{
        if(nOpt == 2)
        {
            _lstTopVideoList.clear();
            return;
        }

		auto iter = std::find(_lstTopVideoList.begin(), _lstTopVideoList.end(), nUserId);
		if(nOpt == 0)
		{
			//Ôö¼Ó
			if(iter == _lstTopVideoList.end())
				_lstTopVideoList.push_back(nUserId);
		}
		else if(nOpt == 1)
		{
			//É¾³ý
			if(iter != _lstTopVideoList.end())
				_lstTopVideoList.erase(iter);
		}
	}

	int	CLClassRoomInfo::GetTopVideoList(int nCount, __int64 *pUserList)
	{
		int nToGet = _lstTopVideoList.size();
		if(nCount <= 0 || pUserList == NULL)
			return nToGet;
		if(nToGet > nCount)
			nToGet = nCount;
		for (size_t i = 0; i < nToGet; i++)
		{
			*(pUserList + i) = _lstTopVideoList[i];
		}
		return nToGet;
	}

	void CLClassRoomInfo::GetTopVideoList(std::vector<__int64>& vecUser)
	{
		if(_lstTopVideoList.empty())
		{
			return;
		}

		vecUser.clear();

		for (size_t i = 0; i < _lstTopVideoList.size(); i++)
		{
			vecUser.push_back( _lstTopVideoList[i]);
		}

		return;
	}

	bool CLClassRoomInfo::UserIsInTopVideoList(__int64 nUserId)
	{
		for (size_t i = 0; i < _lstTopVideoList.size(); i++)
		{
			if(_lstTopVideoList[i] == nUserId)
				return true;
		}
		return false;
	}

	SLUserInfo*	CLClassRoomInfo::GetSpeakUserInfo()
	{
		for (size_t i = 0; i < _lstUserList.size(); i++)
		{
			SLUserInfo* pInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfo(_lstUserList[i]);
			if(pInfo && pInfo->_nUserState & eUserState_speak)
				return pInfo;
		}
		return NULL;
	}

	void CLClassRoomInfo::AddUser(__int64 nUserId)
	{
		if(std::find(_lstUserList.begin(), _lstUserList.end(), nUserId) == _lstUserList.end())
			_lstUserList.push_back(nUserId);
	}
	void CLClassRoomInfo::DelUser(__int64 nUserId)
	{
		auto iter = std::find(_lstUserList.begin(), _lstUserList.end(), nUserId);
		if(iter != _lstUserList.end())
			_lstUserList.erase(iter);
		iter = std::find(_lstAsideUserList.begin(), _lstAsideUserList.end(), nUserId);
		if(iter != _lstAsideUserList.end())
			_lstAsideUserList.erase(iter);
		iter = std::find(_lstTryUserList.begin(), _lstTryUserList.end(), nUserId);
		if(iter != _lstTryUserList.end())
			_lstTryUserList.erase(iter);

		for (auto itermap = _discussGroupList.begin(); itermap != _discussGroupList.end(); itermap++)
		{
			 iter = std::find(itermap->second.begin(), itermap->second.end(), nUserId);
			 if(iter != itermap->second.end())
			 {
				 itermap->second.erase(iter);
				 break;
			 }
		}
	}

	void CLClassRoomInfo::AddCouseware(std::wstring& strCouseware)
	{
		auto iter = std::find(_lstCouseware.begin(), _lstCouseware.end(), strCouseware);
		if(iter == _lstCouseware.end())
			_lstCouseware.push_back(strCouseware);
	}

	void CLClassRoomInfo::DelCouseware(std::wstring& strCouseware)
	{
		auto iter = std::find(_lstCouseware.begin(), _lstCouseware.end(), strCouseware);
		if(iter != _lstCouseware.end())
			_lstCouseware.erase(iter);
	}

	void CLClassRoomInfo::UpdateShowInfo(SLClassRoomShowInfo& sInfo)
	{
		memcpy(&_sShowInfo, &sInfo, sizeof(sInfo));
	}
	SLUserInfo CLClassRoomInfo::GetTeacherInfo()
	{
		for (size_t i = 0; i < _lstUserList.size(); i++)
		{
			SLUserInfo sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_lstUserList[i]);
			if(sInfo.nUserAuthority == UserAu_Teacher)
				return sInfo;
		}
		return SLUserInfo();
	}

	void CLClassRoomInfo::UpdateWhiteBoardOpt(SLWhiteBoardOpt& sOpt)
	{
		if(sOpt._nOpt == EBoardOpt_Create)
			_lstWhiteBoard.push_back(sOpt);
		else if(sOpt._nOpt == EBoardOpt_Delete)
		{
			for (auto iter = _lstWhiteBoard.begin(); iter != _lstWhiteBoard.end(); iter++)
			{
				if(sOpt._nPageId == (*iter)._nPageId && sOpt._nUserId == (*iter)._nUserId)
				{
					_lstWhiteBoard.erase(iter);
					return;
				}
			}
		}
		else if(sOpt._nOpt == EBoardOpt_ModifyName)
		{
			for (auto iter = _lstWhiteBoard.begin(); iter != _lstWhiteBoard.end(); iter++)
			{
				if(sOpt._nPageId == (*iter)._nPageId && sOpt._nUserId == (*iter)._nUserId)
				{
					memcpy((*iter)._szwbName, sOpt._szwbName, sizeof(sOpt._szwbName));
					return;
				}
			}
		}
	}

	void CLClassRoomInfo::AddDiscussUser(int nGroup, __int64 nUserId)
	{
		std::vector<__int64> & sList = _discussGroupList[nGroup];
		if(std::find(sList.begin(), sList.end(), nUserId) == sList.end())
			sList.push_back(nUserId);
	}

	void CLClassRoomInfo::ClearDiscussUser()
	{
		_discussGroupList.clear();
	}

	int CLClassRoomInfo::GetDiscussGroup(int* pGroup, int nSize)
	{
		if(pGroup == NULL || nSize == 0)
			return _discussGroupList.size();
		int nNum = 0;
		for (auto iter = _discussGroupList.begin(); iter != _discussGroupList.end(); iter++)
		{
			if(nNum == nSize)
				break;
			*(pGroup + nNum) = iter->first;
			nNum++;
		}
		return nNum;
	}

	int CLClassRoomInfo::GetDiscussGroupUserList(int nGroupId, __int64* pUserList, int nTogetSize)
	{
		auto iter = _discussGroupList.find(nGroupId);
		if(iter == _discussGroupList.end())
			return 0;
		if(pUserList == NULL)
			return iter->second.size();
		int nNum = 0;
		for (size_t i = 0; i < iter->second.size(); i++)
		{
			if(nNum == nTogetSize)
				break;
			*(pUserList + i) = iter->second.at(i);
			nNum++;
		}
		return nNum;
	}

	int	CLClassRoomInfo::GetUserDiscussGroupId(__int64 nUserId)
	{
		for (auto iter = _discussGroupList.begin(); iter != _discussGroupList.end(); iter++)
		{
			for (size_t i = 0; i < iter->second.size(); i++)
			{
				if(nUserId == iter->second.at(i))
					return iter->first;
			}
		}
		return -1;
	}

	void CLClassRoomInfo::SetClassState(char nState)
	{ 
		_sRoomBaseInfo._nClassState = nState; 
		if(nState == Eclassroomstate_Free)
		{
			CLUserInfoDataContainer* pUserContainer = (CLUserInfoDataContainer*)GetBizInterface()->GetUserInfoDataContainer();
			for (size_t i = 0;  i < _lstUserList.size(); ++i)
			{
				SLUserInfo* pInfo = pUserContainer->GetUserInfo(_lstUserList[i]);
				if(!pInfo)
					continue;
				
                pInfo->_nUserState = pInfo->_nUserState & (~eUserState_ask_speak);
				pInfo->_nUserState = pInfo->_nUserState & (~eUserState_speak);
			}

			_discussGroupList.clear();
		}
	}

	void CLClassRoomInfo::ResetClass()
	{		
		_discussGroupList.clear();
		_lstUserList.clear();
		_lstTryUserList.clear();
		_lstAsideUserList.clear();
		_lstTopVideoList.clear();
		_lstWhiteBoard.clear();
		_lstCouseware.clear();
	}

	int	CLClassRoomInfo::GetUserCount(int* nMember, int* nAside, int * nTry)
	{
		int nTotalNum = _lstUserList.size() + _lstAsideUserList.size() + _lstTryUserList.size();
		if(nMember)
			*nMember = _lstUserList.size();
		if(nAside)
			*nAside = _lstAsideUserList.size();
		if(nTry)
			*nTry = _lstTryUserList.size();
		return nTotalNum;
	}

	int	CLClassRoomInfo::GetUserInfoList(int nCount, SLUserInfo* pInfo)
	{
		int nTotalNum = _lstUserList.size() + _lstAsideUserList.size() + _lstTryUserList.size();
		if(nCount <= 0 || pInfo == NULL)
			return nTotalNum;
		int nToGet = nTotalNum;
		if(nCount < nToGet)
			nToGet = nCount;
		int nIndex = 0;
		for (size_t i = 0; i < _lstUserList.size(); i++)
		{
			SLUserInfo sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_lstUserList[i]);
			memcpy(pInfo + nIndex, &sInfo, sizeof(SLUserInfo));			
			nIndex++;
		}
		for (size_t i = 0; i < _lstAsideUserList.size(); i++)
		{
			SLUserInfo sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_lstAsideUserList[i]);
			memcpy(pInfo + nIndex, &sInfo, sizeof(SLUserInfo));
			nIndex++;
		}
		for (size_t i = 0; i < _lstTryUserList.size(); i++)
		{
			SLUserInfo sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_lstTryUserList[i]);
			memcpy(pInfo + nIndex, &sInfo, sizeof(SLUserInfo));
			nIndex++;
		}
		return nToGet;
	}

}
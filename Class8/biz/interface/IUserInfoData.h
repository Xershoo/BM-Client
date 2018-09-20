#pragma  once

#include "data.h"

namespace biz
{

	class IUserInfoDataContainer
	{
	public:
		virtual SLUserInfo*	GetUserInfo(__int64 nUserId) = 0;
		virtual SLUserInfo	GetUserInfoById(__int64 nUserId,bool* pbExist = NULL) = 0;
		virtual SLUserInfo*	GetUserInfoByName(const wchar_t* pName) = 0;
		virtual void		UpdateUserINfo(SLUserInfo&sInfo) = 0;
		virtual void		AddUserInfo(SLUserInfo& sInfo) = 0;
		virtual void		DeleteUserInfo(SLUserInfo& sInfo) = 0;
		virtual int			GetUserInfoList(int nCount, SLUserInfo* pInfo) = 0;
		virtual void		SetUserState(__int64 nUserId, int nState) = 0;
	};

}
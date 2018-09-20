#pragma  once

#include "../interface/IUserInfoData.h"
#include <vector>
namespace biz
{

	class CLUserInfoDataContainer : public IUserInfoDataContainer
	{
	public:
		virtual SLUserInfo*	GetUserInfo(__int64 nUserId);
		virtual SLUserInfo	GetUserInfoById(__int64 nUserId,bool* pbExist = NULL);
		virtual SLUserInfo*	GetUserInfoByName(const wchar_t* pName);
		
		virtual void		UpdateUserINfo(SLUserInfo&sInfo);
		virtual void		AddUserInfo(SLUserInfo& sInfo);
		virtual void		SetUserState(__int64 nUserId, int nState);
		int					GetUserState(__int64 nUserId);

		virtual void		DeleteUserInfo(SLUserInfo& sInfo);
		virtual int			GetUserInfoList(int nCount, SLUserInfo* pInfo);
		void				SetUserRoom(__int64 nUserId, __int64 nRoomId);
		void				DeleteUser(__int64 nUserId);
		void				CleanStudentsAskSpeak(); //清除学生的举手
		std::vector<SLUserInfo>& GetUserList() { return _userList; }

		void				Clear();  //清除用户，主要是用户注销时，得清除
	private:
		
	private:
		std::vector<SLUserInfo>		_userList;
	};

}
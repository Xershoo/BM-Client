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
		void				CleanStudentsAskSpeak(); //���ѧ���ľ���
		std::vector<SLUserInfo>& GetUserList() { return _userList; }

		void				Clear();  //����û�����Ҫ���û�ע��ʱ�������
	private:
		
	private:
		std::vector<SLUserInfo>		_userList;
	};

}
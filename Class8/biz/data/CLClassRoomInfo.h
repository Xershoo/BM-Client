#pragma once

#include "../interface/data.h"
#include "../interface/IClassRoomInfo.h"
#include <vector>
#include <string>
#include <map>

namespace biz
{
	class CLClassRoomInfo : public IClassRoomInfo
	{
	public:
		virtual SLClassRoomBaseInfo& GetRoomBaseInfo();
		virtual SLClassRoomShowInfo& GetShowInfo() { return _sShowInfo; }
		virtual int					 GetUserInfoList(int nCount, SLUserInfo* pInfo);
		virtual	SLUserInfo			 GetTeacherInfo();
		virtual int					 GetDiscussGroup(int* pGroup, int nSize); //获取分组讨论组数
		virtual int					 GetDiscussGroupUserList(int nGroupId, __int64* pUserList, int nTogetSize); //获取哪个分组成员列表
		virtual int					 GetUserDiscussGroupId(__int64 nUserId);
		virtual int					 GetUserCount(int* nMember, int* nAside, int * nType); //获取正式学生，旁边学生，试听学生人数。 返回值为总共学生
		virtual int					 GetTopVideoList(int nCount, __int64 *pUserList);
		virtual void				 GetTopVideoList(std::vector<__int64>& vecUser);
        virtual void                 TopVideoListOpt(__int64 nUserId, int nOpt, int nIndex); //nOpt：0-增加到顶上的视频区;1-删除;2-清除列表. nIndex表示第几个，暂时不用
		virtual bool				 UserIsInTopVideoList(__int64 nUserId);
		virtual SLUserInfo*	         GetSpeakUserInfo();
		
		void SetRoomBaseInfo(SLClassRoomBaseInfo& sInfo);	
		
		void AddUser(__int64 nUserId);
		void DelUser(__int64 nUserId);
		void AddCouseware(std::wstring& strCouseware);
		void DelCouseware(std::wstring& strCouseware);
		void UpdateShowInfo(SLClassRoomShowInfo& sInfo);
		
		void SetClassState(char nState);
		void ResetClass();
		void UpdateWhiteBoardOpt(SLWhiteBoardOpt& sOpt);

		std::vector<std::wstring> & GetCoursewareList(){ return _lstCouseware;};
		void AddDiscussUser(int nGroup, __int64 nUserId);
		void ClearDiscussUser();
	
	private:
		SLClassRoomBaseInfo		_sRoomBaseInfo;
		SLClassRoomShowInfo		_sShowInfo;
		std::vector<__int64>	_lstUserList;
		std::vector<__int64>	_lstAsideUserList; //旁边人列表
		std::vector<__int64>	_lstTryUserList; //试听人员
		std::vector<__int64>	_lstTopVideoList; //10视频区
		std::vector<SLWhiteBoardOpt>	_lstWhiteBoard;
		std::vector<std::wstring> _lstCouseware;
		std::map<int, std::vector<__int64>>	_discussGroupList;
	};
} //end namespace
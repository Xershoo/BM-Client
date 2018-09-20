#pragma  once

#include "data.h"

namespace biz
{
	class IClassRoomInfo
	{
	public:
		virtual SLClassRoomBaseInfo& GetRoomBaseInfo() = 0;
		virtual SLClassRoomShowInfo& GetShowInfo() = 0;
		virtual int					 GetUserInfoList(int nCount, SLUserInfo* pInfo) = 0; //得到房间所有人列表，包括旁听和试听
		virtual	SLUserInfo			 GetTeacherInfo() = 0;
		virtual int					 GetUserCount(int* nMember, int* nAside, int * nType) = 0; //获取正式学生，旁边学生，试听学生人数。 返回值为总共学生
		virtual int					 GetDiscussGroup(int* pGroup, int nSize) = 0; //获取分组讨论组数
		virtual int					 GetUserDiscussGroupId(__int64 nUserId) = 0;
		virtual int					 GetDiscussGroupUserList(int nGroupId, __int64* pUserList, int nTogetSize) = 0; //获取哪个分组成员列表
		virtual int					 GetTopVideoList(int nCount, __int64 *pUserList) = 0;  //得到顶上的学生视频列表
		virtual void				 GetTopVideoList(std::vector<__int64>& vecUser) = 0;  //得到顶上的学生视频列表
        virtual void                 TopVideoListOpt(__int64 nUserId, int nOpt, int nIndex) = 0; //nOpt：0-增加到顶上的视频区;1-删除;2-清除列表. nIndex表示第几个，暂时不用
		virtual bool				 UserIsInTopVideoList(__int64 nUserId) = 0; //判断用户是否显示在视频区，针对学生
		virtual SLUserInfo*			 GetSpeakUserInfo() = 0;
	};
};  // end namespace
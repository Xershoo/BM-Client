#pragma  once

#include "data.h"

namespace biz
{
	class IClassRoomInfo
	{
	public:
		virtual SLClassRoomBaseInfo& GetRoomBaseInfo() = 0;
		virtual SLClassRoomShowInfo& GetShowInfo() = 0;
		virtual int					 GetUserInfoList(int nCount, SLUserInfo* pInfo) = 0; //�õ������������б���������������
		virtual	SLUserInfo			 GetTeacherInfo() = 0;
		virtual int					 GetUserCount(int* nMember, int* nAside, int * nType) = 0; //��ȡ��ʽѧ�����Ա�ѧ��������ѧ�������� ����ֵΪ�ܹ�ѧ��
		virtual int					 GetDiscussGroup(int* pGroup, int nSize) = 0; //��ȡ������������
		virtual int					 GetUserDiscussGroupId(__int64 nUserId) = 0;
		virtual int					 GetDiscussGroupUserList(int nGroupId, __int64* pUserList, int nTogetSize) = 0; //��ȡ�ĸ������Ա�б�
		virtual int					 GetTopVideoList(int nCount, __int64 *pUserList) = 0;  //�õ����ϵ�ѧ����Ƶ�б�
		virtual void				 GetTopVideoList(std::vector<__int64>& vecUser) = 0;  //�õ����ϵ�ѧ����Ƶ�б�
        virtual void                 TopVideoListOpt(__int64 nUserId, int nOpt, int nIndex) = 0; //nOpt��0-���ӵ����ϵ���Ƶ��;1-ɾ��;2-����б�. nIndex��ʾ�ڼ�������ʱ����
		virtual bool				 UserIsInTopVideoList(__int64 nUserId) = 0; //�ж��û��Ƿ���ʾ����Ƶ�������ѧ��
		virtual SLUserInfo*			 GetSpeakUserInfo() = 0;
	};
};  // end namespace
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
		virtual int					 GetDiscussGroup(int* pGroup, int nSize); //��ȡ������������
		virtual int					 GetDiscussGroupUserList(int nGroupId, __int64* pUserList, int nTogetSize); //��ȡ�ĸ������Ա�б�
		virtual int					 GetUserDiscussGroupId(__int64 nUserId);
		virtual int					 GetUserCount(int* nMember, int* nAside, int * nType); //��ȡ��ʽѧ�����Ա�ѧ��������ѧ�������� ����ֵΪ�ܹ�ѧ��
		virtual int					 GetTopVideoList(int nCount, __int64 *pUserList);
		virtual void				 GetTopVideoList(std::vector<__int64>& vecUser);
        virtual void                 TopVideoListOpt(__int64 nUserId, int nOpt, int nIndex); //nOpt��0-���ӵ����ϵ���Ƶ��;1-ɾ��;2-����б�. nIndex��ʾ�ڼ�������ʱ����
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
		std::vector<__int64>	_lstAsideUserList; //�Ա����б�
		std::vector<__int64>	_lstTryUserList; //������Ա
		std::vector<__int64>	_lstTopVideoList; //10��Ƶ��
		std::vector<SLWhiteBoardOpt>	_lstWhiteBoard;
		std::vector<std::wstring> _lstCouseware;
		std::map<int, std::vector<__int64>>	_discussGroupList;
	};
} //end namespace
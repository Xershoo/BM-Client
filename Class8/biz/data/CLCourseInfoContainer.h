#pragma once

#include "../interface/data.h"
#include "../interface/ICourseInfoContainer.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

namespace biz
{
	class CLCourseInfoContainer : public ICourseInfoContainer
	{		
		typedef vector<SLCourseInfo*>  SLCourseInfoList;
				
	public:
		CLCourseInfoContainer();
		virtual ~CLCourseInfoContainer();

	public:
		virtual int GetCourseCount();
		virtual SLCourseInfo& GetCourseInfoById(__int64 nCourseId);
		virtual SLCourseInfo& GetCourseInfoByIndex(int nIndex);
		virtual void UpdateCourseInfo(SLCourseInfo& scInfo);

	protected:
		SLCourseInfoList  m_vecCourseInfo;
	};
}
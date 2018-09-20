#pragma once

#include "data.h"

namespace biz
{
	class ICourseInfoContainer
	{
	public:
		virtual int GetCourseCount() = 0;
		virtual SLCourseInfo& GetCourseInfoById(__int64 nCourseId) = 0;
		virtual SLCourseInfo& GetCourseInfoByIndex(int nIndex) = 0;
		virtual void UpdateCourseInfo(SLCourseInfo& scInfo) = 0;
	};
}

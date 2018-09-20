#pragma once
//�����debug����ԣ������release���¼���ļ�

#include <string>
#include <assert.h>
#include <windows.h>	//MultiByteToWideChar


#define assert_(exp)	biz_utility::assert0(!!(exp), #exp, __FILE__, __LINE__)


namespace biz_utility
{
	class LogFile;
	LogFile* assert0(bool b, std::string exp, std::string file, unsigned lineno);
	// ֮���Բ���LogFile&����Ϊ������cppֻ����asser_log.h
	// ʹ��assert0�ᵼ���Ҳ���LogFile�Ĵ���.
}
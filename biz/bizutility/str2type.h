#pragma once

#include "bizutility/string_convert.h"
#include <string>
#include <Windows.h>


// 字符串和各种类型的转换

namespace biz_utility
{
	static const int INVALID_INT_VALUE = -0x7fffffff;	// '-'


	template<typename X>
	std::string x2str(const X& x)
	{
		return (const char*)x;
	}

	template<typename X>
	X str2x(const char* str)
	{
		return X(str);
	}


	// string
	inline std::string x2str(const std::string& x){ return x; }

	//template<> inline std::wstring str2x(const char* str)
	//{
	//	return m2w(str, CP_UTF8);
	//}

	//inline std::string x2str(const std::wstring& x){ return w2m(x.c_str(), CP_UTF8); }


	// int
	inline std::string x2str(int x)
	{
		if (INVALID_INT_VALUE == x)
			return "-";

		return tostring(x);
	}

	template<> inline int str2x<int>(const char* str)
	{
		if (0 == strcmp("", str))
			return 0;

		if (0 == strcmp("-", str))
			return INVALID_INT_VALUE;

		return a2int(str);
	}


	// unsigned short
	inline std::string x2str(unsigned short x)
	{
		return tostring(x);
	}

	template<> inline unsigned short str2x<unsigned short>(const char* str)
	{
		if (0 == strcmp("", str))
			return 0;

		return a2unsignshort(str);
	}


	// double
	inline std::string x2str(double x)
	{
		return tostring(x);
	}

	template<> inline double str2x<double>(const char* str)
	{
		if (0 == strcmp("", str))
			return 0;

		return a2double(str);
	}


	// bool
	inline std::string x2str(bool x)
	{
		return x ? "1" : "0";
	}

	template<> inline bool str2x<bool>(const char* str)
	{
		if (0 == strcmp("", str))
			return false;

		if (0 == strcmp("true", str))
			return true;

		return !!str2x<int>(str);
	}
}
#include "string_convert.h"
#include <Windows.h>


namespace biz_utility
{
	unsigned short a2unsignshort(std::string str)
	{
		unsigned short ret = 0;
		stringtoT(str, ret);
		return ret;
	}


	unsigned short w2unsignshort(std::wstring str)
	{
		unsigned short ret = 0;
		wstringtoT(str, ret);
		return ret;
	}


	int a2int(std::string str)
	{
		int ret = 0;
		stringtoT(str, ret);
		return ret;
	}


	int w2int(std::wstring str)
	{
		int ret = 0;
		wstringtoT(str, ret);
		return ret;
	}


	double a2double(std::string str)
	{
		double ret = 0;
		stringtoT(str, ret);
		return ret;
	}


	double w2double(std::wstring str)
	{
		double ret = 0;
		wstringtoT(str, ret);
		return ret;
	}


	std::wstring m2w(std::string str)
	{
		return m2w(str, CP_ACP);
	}


	std::string w2m(std::wstring str)
	{
		return w2m(str, CP_ACP);
	}


	std::wstring m2w(std::string str, unsigned int cp)
	{
		int size = MultiByteToWideChar(cp, 0, str.c_str(), str.size(), NULL, 0)+1;
		wchar_t* wbuf = new wchar_t[size];
		size = MultiByteToWideChar(cp, 0, str.c_str(), str.size(), wbuf, size);
		wbuf[size] = 0;
		std::wstring ret = wbuf;
		delete[] wbuf;
		return ret;
	}

	std::string w2m(std::wstring str, unsigned int cp)
	{
		int size = WideCharToMultiByte(cp, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL)+1;
		char* buf = new char[size];
		size = WideCharToMultiByte(cp, 0, str.c_str(), str.size(), buf, size, NULL, NULL);
		buf[size] = 0;
		std::string ret = buf;
		delete[] buf;
		return ret;
	}
}

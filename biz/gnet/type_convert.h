#pragma once

#include "octets.h"
#include "marshal.h"
#include <string>


namespace gnet
{
	inline void oct2wchar(gnet::Octets& oct, wchar_t* buf, int maxsize)
	{
		// otc 中包含的是宽字节 utf-16
		int copybyte = oct.size();
		if ((maxsize-1)*sizeof(wchar_t) < (size_t)copybyte)
			copybyte = (maxsize-1)*sizeof(wchar_t);
		memcpy(buf, oct.begin(), copybyte);
		buf[copybyte/sizeof(wchar_t)] = 0;
	}

	inline void oct2char(gnet::Octets& oct, char* buf, int maxsize)
	{
		int copybyte = oct.size();
		if ((maxsize-1)*sizeof(wchar_t) < (size_t)copybyte)
			copybyte = (maxsize-1)*sizeof(wchar_t);
		memcpy(buf, oct.begin(), copybyte);
		buf[copybyte] = 0;
	}

	inline std::wstring oct2wstr(gnet::Octets& oct)
	{
		std::wstring ret((wchar_t*)oct.begin(), oct.size()/sizeof(wchar_t));
		return ret;
	}

	inline std::string oct2str(gnet::Octets& oct)
	{
		std::string ret((char*)oct.begin(), oct.size());
		return ret;
	}

	inline gnet::Octets wchar2oct(const wchar_t* pszBuf)
	{
		if (NULL == pszBuf)
			return gnet::Octets();
		gnet::Octets oct(pszBuf, wcslen(pszBuf)*sizeof(wchar_t));
		return oct;
	}

	inline gnet::Octets char2oct(const char* pszBuf)
	{
		if (NULL == pszBuf)
			return gnet::Octets();

		gnet::Octets oct(pszBuf, strlen(pszBuf));
		return oct;
	}

	inline int oct2int(gnet::Octets& oct)
	{
		gnet::OctetsStream os(oct);
		int i;;
		os >> i;
		return i;
	}

	inline gnet::Octets int2oct(int i)
	{
		gnet::OctetsStream os;
		os << i;
		return os;
	}

	inline char oct2byte(gnet::Octets& oct)
	{
		gnet::OctetsStream os(oct);
		char c;;
		os >> c;
		return c;
	}

	inline gnet::Octets byte2oct(char c)
	{
		gnet::OctetsStream os;
		os << c;
		return os;
	}
}
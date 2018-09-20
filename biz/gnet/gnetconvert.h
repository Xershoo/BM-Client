#pragma once

#include "octets.h"
#include <string>


namespace gnet
{
	inline std::wstring net2hostw(gnet::Octets& oct)
	{
		std::wstring ret(oct.size()/2+1, 0);
		memcpy((void*)ret.c_str(), oct.begin(), oct.size());
		return ret;
	}


	inline std::string net2host(gnet::Octets& oct)
	{
		std::string ret(oct.size()+1, 0);
		memcpy((void*)ret.c_str(), oct.begin(), oct.size());
		return ret;
	}


	inline gnet::Octets host2net(std::wstring wstr)
	{
		Octets oct(wstr.size()*2);
		memcpy(oct.begin(), wstr.c_str(), oct.size());
		return oct;
	}


	inline gnet::Octets host2net(std::string str)
	{
		Octets oct(str.size());
		memcpy(oct.begin(), str.c_str(), oct.size());
		return oct;
	}
}

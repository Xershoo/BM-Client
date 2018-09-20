#include "convert.h"


namespace biz
{
	std::wstring net2host(gnet::Octets& oct)
	{
		std::wstring ret(oct.size()/2+1, 0);
		memcpy((void*)ret.c_str(), oct.begin(), oct.size());
		return ret;
	}


	void host2net(std::wstring wstr, gnet::Octets& oct)
	{
		oct.resize(wstr.size()*2);
		memcpy(oct.begin(), wstr.c_str(), oct.size());
	}
}
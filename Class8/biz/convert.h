#pragma once

#include "biz/interface/data.h"
#include "gnet/octets.h"

namespace biz
{
	std::wstring net2host(gnet::Octets& oct);
	void host2net(std::wstring wstr, gnet::Octets& oct);
}
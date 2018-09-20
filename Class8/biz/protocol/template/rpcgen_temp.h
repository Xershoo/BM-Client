#pragma once

$(include)


namespace $(namespace)
{
	void RegisterProtocols();
	void UnregisterProtocols();
	std::string FindProtocolName(unsigned int type);
} 
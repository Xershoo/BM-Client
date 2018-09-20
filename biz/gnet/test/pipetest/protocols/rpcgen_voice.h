#pragma once

#include "CreateDsoundBuffer_p.h"
#include "SendVoiceData_p.h"


namespace rpcgen_voice
{
	void RegisterProtocols();
	void UnregisterProtocols();
	std::string FindProtocolName(unsigned int type);
} 
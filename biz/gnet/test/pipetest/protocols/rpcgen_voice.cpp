#pragma once

#include "rpcgen_voice.h"


namespace rpcgen_voice
{
	gnet::ProtocolManager::Stub* stubs[] =
	{
		new gnet::ProtocolManager::TStub<gnet::CreateDsoundBuffer>(),
		new gnet::ProtocolManager::TStub<gnet::SendVoiceData>()
	};

	void RegisterProtocols()
	{
		int size = sizeof(stubs)/sizeof(*stubs);
		for (int i=0; i<size; i++)
			gnet::ProtocolManager::AddStub(stubs[i]);
	}

	void UnregisterProtocols()
	{
		int size = sizeof(stubs)/sizeof(*stubs);
		for (int i=0; i<size; i++)
			gnet::ProtocolManager::DelStub(stubs[i]);
	}

	struct ProtocolType2Name
	{
		unsigned	type;
		const char*	name;
	};

	ProtocolType2Name nameList[] =
	{
		{ gnet::CreateDsoundBuffer::PROTOCOL_TYPE, "CreateDsoundBuffer"},
		{ gnet::SendVoiceData::PROTOCOL_TYPE, "SendVoiceData"}
	};

	std::string FindProtocolName(unsigned int type)
	{
		int size = sizeof(nameList)/sizeof(*nameList);
		for (int i=0; i<size; i++)
		{
			if (nameList[i].type == type)
				return nameList[i].name;
		}
		return "";
	}
} 
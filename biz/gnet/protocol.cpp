#include "protocol.h"
#include "log_gnet.h"
#include "bizutility/string_convert.h"
#include "bizutility/log/assert_log.h"


namespace gnet
{
	void ProtocolProcessor::ProcessOctets(unsigned int sid, Octets& input)
	{
		OctetsStream os(input);
		size_t mark = 0;
		try
		{
			while(os.size() > mark)
			{
				Protocol::Type type;
				unsigned int size;
				os >> CompactUINT(type) >> CompactUINT(size);
				if (size > os.size() - os.position())
					break;
				mark = os.position() + size;

				try
				{
					Protocol* p = ProtocolManager::Create(type);
					if (p)
					{
						p->unmarshal(os);
						if (os.position() != mark)
						{
							throw ProtocolException(type, size, "data size not match");
						}
						std::string info = "Receive protocol: " + biz_utility::tostring(p->GetType()) + " is not valid";
						bool bRet = p->IsOK();
						*assert_(bRet) << info.c_str();
						if (bRet)
							p->Process(sid, this);	//当protocol不能处理是，就该交给session处理，并重载Session::Process(Protocol&)
						delete p;
					}
					else
					{
						//未知协议
						Octets data;
						data.resize(size);
						os.pop_byte((char*)data.begin(), size);
						throw ProtocolException(type, size, "UnKnow Protocol");
					}
				}
				catch (Marshal::Exception&)
				{
					throw ProtocolException(type, size, "Marshal::Exception");	//这儿发生的marshal excpetion需要特殊处理，不能skip
				}
			}
		}
		catch (Marshal::Exception& /*ex*/)
		{
			//skip
		}
		catch (ProtocolException& ex)
		{
			//assert_(false && "异常或者未知协议");
			throw ex;			//交给Engine::AioNotify处理
		}
		//???此处可以优化，避免OctetsStream带来的多一次拷贝
		input.erase(0, mark);
	}


	//////////////////////  Protocol  ////////////////////////////////////////////////////
	Octets Protocol::Encode() const
	{
		OctetsStream os;
		{
			OctetsStream data;
			data << *this;
			os << CompactUINT(remove_const(m_type)) << data;
		}
		return os;
	}


	void Protocol::Process(unsigned int sid, ProtocolProcessor* processor)
	{
		processor->ProcessProtocol(sid, *this);
	}


	/////////////////////  ProtocolManager  /////////////////////////////////////////////////////
	Mutex ProtocolManager::_mutex;
	ProtocolManager::StubMapType ProtocolManager::stubMap;


	void ProtocolManager::AddStub(Stub* stub)
	{
		Scoped lock(_mutex);
		if (!stubMap.insert(std::make_pair(stub->type, stub)).second)
			assert_(false);
	}


	void ProtocolManager::DelStub(Stub* stub)
	{
		DelStub(stub->type);
	}


	void ProtocolManager::DelStub(Protocol::Type type)
	{
		Scoped lock(_mutex);
		stubMap.erase(type);
	}


	Protocol* ProtocolManager::Create(Protocol::Type type)
	{
		Scoped lock(_mutex);
		StubMapType::iterator it = stubMap.find(type);
		if (it != stubMap.end())
			return it->second->create();
		return NULL;
	}
}
#pragma once

#include "lock.h"
#include "marshal.h"


namespace gnet
{
	class Protocol;


	// 承前启下，实现Octets到Protocol的转变
	class ProtocolProcessor
	{
	public:
		virtual void ProcessProtocol(unsigned int sid, Protocol&) = 0;
		void ProcessOctets(unsigned int sid, Octets& input);
	};


	////////////////////////  Protocol  //////////////////////////////////////////////////
	class Protocol : public Marshal
	{
	public:
		typedef unsigned int Type;

		virtual ~Protocol() {}
		Octets Encode() const;
		Type GetType() const { return m_type; }


	protected:
		explicit Protocol (Type t) : m_type(t) {}


	public:
		virtual void Process(unsigned int sid, ProtocolProcessor*);
		virtual bool IsOK() const { return true; }	// 验证协议是否正确
		virtual Protocol* Clone() const = 0;


	private:
		const Type m_type;
	};


	////////////////////////  ProtocolException  //////////////////////////////////////////////////
	class ProtocolException : public std::exception
	{
	public:
		ProtocolException(Protocol::Type type, unsigned int size, const char* desc)
			: exception(GetDesc(type, size, desc)){}


	private:
		const char* GetDesc(Protocol::Type type, unsigned int size, const char* desc)
		{
			int len = _snprintf_s(_what, sizeof(_what)-1,
				"ProtocolException type = %d, size = %d, desc = %s", type, size, desc);
			if (len < 0)
				len = 0;
			_what[len] = 0;
			return _what;
		}


	private:
		char _what[1024];
	};


	//////////////////////  manager protocol Stub  ////////////////////////////////////////////////////
	class ProtocolManager
	{
	public:
		struct Stub
		{
			Protocol::Type type;
			virtual Protocol *create() = 0;
			virtual ~Stub(){}
		};

		template <typename T>
		struct TStub : public Stub
		{
			TStub() { type = T::PROTOCOL_TYPE; }
			virtual Protocol* create() { return new T(); }
		};


	public:
		static void AddStub(Stub* stub);
		static void DelStub(Stub* stub);
		static void DelStub(Protocol::Type type);
		static Protocol* Create(Protocol::Type type);


	private:
		typedef std::map<Protocol::Type, Stub*> StubMapType;
		static StubMapType stubMap;
		static Mutex _mutex;
	};
}
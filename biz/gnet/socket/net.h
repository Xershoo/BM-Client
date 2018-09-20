#pragma once

#include <map>
#include <vector>
#include "gnet/lock.h"
#include "gnet/octets.h"
#include "gnet/platform.h"
#include "gnet/socket/security.h"
#include "bizutility/log/assert_log.h"
#include "bizutility/func_dispatcher/func_binder.h"


namespace gnet
{
	enum
	{
		WM_AIO_NOTIFY			= WM_USER + 1,
		WM_AIO_TASK				= WM_USER + 2,	//wparam = task
		FD_AIO_CLOSE			= (1 << FD_MAX_EVENTS)
	};


	class Connection;
	class Aio;
	class AioManager;
	class Engine;
	class AioTimer;


	static unsigned CALLBACK ConnectThread(void* arg);
	static unsigned CALLBACK ConnectPortListThread(void* arg);
	static unsigned CALLBACK ListenThread(void* arg);


	class Connection
	{
	public:
		Connection(std::string host, std::string port, AioManager&, void* userData);
		Connection(std::string host, std::vector<std::string>& portlist, AioManager&, void* userData);
		Aio* connect();
		Aio* connect_portlist();
		void SetError(const char* err){ m_error = err; }
		const char* GetError(int& err) const { err = m_errorno; return m_error.c_str(); }


	private:
		std::string m_host;			//主机名或者地址串(IPv4的点分十进制串或者IPv6的16进制串)
		std::string m_port;			//服务名或端口号
		std::vector<std::string>	m_portlist;

		int			m_errorno;
		std::string m_error;


	public:
		void*		m_userData;
		AioManager& m_manager;
		Aio*		m_aio;
	};


	class CListen
	{
	public:
		CListen(std::string host, std::string port, AioManager&, void* userData);
		Aio* listen();
		void SetError(const char* err){ m_error = err; }
		const char* GetError(int& err) const { err = m_errorno; return m_error.c_str(); }


	private:
		std::string m_host;			//主机名或者地址串(IPv4的点分十进制串或者IPv6的16进制串)
		std::string m_port;			//服务名或端口号

		int m_errorno;
		std::string m_error;


	public:
		void*		m_userData;
		AioManager& m_manager;
		Aio*		m_aio;
	};


	//aio的生命周期管理是个问题，已解决。
	class Aio
	{
	public:
		typedef unsigned int SID;

		Aio(AioManager&, SOCKET);
		~Aio();

		// in main thread
		void Send(const Octets& oct);
		void Close(const char* lastinfo);

		// else engine thread
		void OnAccept();

		void OnRecv();
		void OnSend();

		//收到以下两条，Aio将销毁
		void OnClose();
		void OnError(int event, int error);

		void SetLastInfo(const char* info);
		std::string GetLastInfo() { return m_lastInfo; }
		SID GetSid(){ return m_sid; }

		void SetPeerAddr(const char* addr){ m_peerAddr = addr; }
		const char* GetPeerAddr(){ return m_peerAddr.c_str(); }
		void SetPeerPort(const char* port){ m_peerPort = port; }
		const char* GetPeerPort(){ m_peerPort.c_str(); }

		void SetISecurity(Security::Type type, const Octets& key);
		void SetOSecurity(Security::Type type, const Octets& key);

	private:
		void Process(Octets& input);


	public:
		SOCKET		m_socket;
	private:
		std::string m_lastInfo;
		Octets		m_output;
		Octets		m_input;
		SID			m_sid;
		Mutex		mutex;		//for m_output
		AioManager&	m_manager;

		std::string m_peerAddr;
		std::string m_peerPort;

		Security*	m_isec;
		Security*	m_osec;
	};


	class AioManager
	{
	public:
		// any thread
		void Connect(const char* host, const char* port, void* userData);
		void Connect(const char* host, std::vector<std::string> portlist, void* userData); // 端口列表
		void Send(Aio::SID sid, const Octets& oct);
		void Close(Aio::SID sid, const char* lastinfo);
		void Listen(const char* host, const char* port, void* userData);

		//这两个方法必然是在网络线程中调用的
		void SetISecurity(Aio::SID sid, Security::Type type, const Octets key);
		void SetOSecurity(Aio::SID sid, Security::Type type, const Octets key);
		void PostSetISecurity(Aio::SID sid, Security::Type type, const Octets key);
		void PostSetOSecurity(Aio::SID sid, Security::Type type, const Octets key);


	public:	//回调 
		// in engine thread except OnConnectAbort & OnConnect
		virtual void OnConnectAbort(const Connection& con);
		virtual void OnConnect(Aio::SID sid, const Connection& con);

		virtual void OnListenAbort(const CListen& clisten);
		virtual void OnListen(Aio::SID sid, const CListen& listen);

		virtual void Process(Aio::SID, Octets& input);

		virtual void OnClose(Aio::SID sid, std::string errStr){}
		virtual void OnError(Aio::SID sid, int event, int error){}


		virtual ~AioManager(){}
	};


	class Engine
	{
	protected:
		Engine();
		struct StartUpArg
		{
			HANDLE hEvent;
			HWND hMain;
			StartUpArg(){ hMain = NULL; hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL); }
			~StartUpArg(){ ::CloseHandle(hEvent); }
			void SetReady(HWND hwnd){ hMain = hwnd; ::SetEvent(hEvent); }
			void WaitReady(){ :: WaitForSingleObject(hEvent, INFINITE); }
		};


	public:
		static Engine* GetInst();

		// in main thread
		bool StartUp();
		void CleanUp();

		// in engine thread
		bool Attach(Aio* aio, long lEvent);
		void Detach(SOCKET so, Aio* aio);

		//else
		void PostMsg(UINT msg, WPARAM wParam, LPARAM lParam);


	private:
		// in engine thread
		void AioNotify(SOCKET socket, int fdEvent, int error);
		Aio* FindAio(SOCKET so);
	public:
		Aio* FindAioBySID(Aio::SID);


	private:	//static
		static unsigned CALLBACK EngineThread(void* arg);
		static bool RegisterWindowClass(const wchar_t* className);
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


		//timer
	public:
		static void CALLBACK TimerProc(HWND, UINT, UINT_PTR idEvent, DWORD);
		bool StartTimer(AioTimer& timer, int period /*ms*/);
		void StopTimer(AioTimer& timer);


	private:
		HANDLE					m_hThread;
		HWND					m_hMain;
		//只在网络线程访问(Attach Detach FindAio FindAioBySID)
		std::map<SOCKET, Aio*>	m_mapAio;
	public:
		static	DWORD			_engineThreadID;
	};


	//////////////////  Task  ////////////////////////////////////////////////////////
	class Task
	{
	public:
		virtual void Run() = 0;
		virtual ~Task(){};

		void RunDelete()
		{
			Run();
			delete this;
		}
	};


	class ConnectTask : public Task
	{
	public:
		ConnectTask(Connection& con) : m_connect(con){}
		~ConnectTask(){}

		virtual void Run()
		{
			if (Engine::GetInst()->Attach(m_connect.m_aio, FD_READ|FD_WRITE|FD_CLOSE))
			{
				m_connect.m_manager.OnConnect(m_connect.m_aio->GetSid(), m_connect);
			}
			else
			{
				m_connect.SetError("Engine::Attach");
				m_connect.m_manager.OnConnectAbort(m_connect);
				delete m_connect.m_aio;
			}
		}

	private:
		Connection m_connect;
	};


	class ListenTask : public Task
	{
	public:
		ListenTask(CListen& listen) : m_listen(listen){}
		~ListenTask(){}

		virtual void Run()
		{
			if (Engine::GetInst()->Attach(m_listen.m_aio, FD_ACCEPT|FD_CLOSE))
			{
				m_listen.m_manager.OnListen(m_listen.m_aio->GetSid(), m_listen);
			}
			else
			{
				m_listen.SetError("Engine::Attach");
				m_listen.m_manager.OnListenAbort(m_listen);
				delete m_listen.m_aio;
			}
		}

	private:
		CListen m_listen;
	};


	class SendBySID : public Task
	{
	public:
		SendBySID(Aio::SID sid, const Octets& oct) : m_sid(sid), m_oct(oct){}
		virtual void Run()
		{
			Aio* aio = Engine::GetInst()->FindAioBySID(m_sid);
			assert_(aio);
			if (!aio)
				return;
			aio->Send(m_oct);
		}

	private:
		Aio::SID	m_sid;
		Octets		m_oct;
	};


	class CloseBySID : public Task
	{
	public:
		CloseBySID(Aio::SID sid, std::string str) : m_sid(sid), m_errStr(str){}
		virtual void Run()
		{
			Aio* aio = Engine::GetInst()->FindAioBySID(m_sid);
			// 有可能多次关闭当kick的时候既有主动关闭又有被动关闭
			// assert_(aio);
			if (!aio)
				return;
			aio->Close(m_errStr.c_str());
		}

	private:
		Aio::SID	m_sid;
		std::string	m_errStr;
	};


	class SetSecurity : public Task
	{
	public:
		SetSecurity(Aio::SID sid,  Security::Type type, const Octets& oct, bool bOut)
			: m_sid(sid), m_type(type), m_key(oct), m_bOut(bOut){}
		virtual void Run()
		{
			Aio* aio = Engine::GetInst()->FindAioBySID(m_sid);
			assert_(aio);
			if (!aio)
				return;
			if (m_bOut)
				aio->SetOSecurity(m_type, m_key);
			else aio->SetISecurity(m_type, m_key);
		}

	private:
		bool			m_bOut;
		Aio::SID		m_sid;
		Security::Type	m_type;
		Octets			m_key;
	};


	//////////////////////  AioTimer  ////////////////////////////////////////////////////
	class AioTimer
	{
		//no copy
		AioTimer(const AioTimer&);
		void operator = (const AioTimer&);

	public:
		bool Start(int period);	//ms
		void Stop();

		virtual void OnAioTimer() = 0;
		virtual ~AioTimer(){ Stop(); }
		AioTimer();


	private:
		bool	m_bIsRuning;
	};


	template <typename TFunc>
	class AioTimerBinder : public AioTimer
	{
	public:
		AioTimerBinder(const TFunc& f) : m_func(f){}
		virtual void OnAioTimer(){ m_func(); }
		TFunc m_func;
	};


	template <typename TFunc>
	inline AioTimerBinder<TFunc>* CreateAioTimer(const TFunc& f)
	{
		return new AioTimerBinder<TFunc>(f);
	}


	template <typename TRet, typename TC>
	inline AioTimerBinder<mem_fun_binder_0<TRet, TC> >* CreateAioTimer(TRet (TC::*pfunc)(), TC* pc)
	{
		mem_fun_binder_0<TRet, TC> fun(pfunc, pc);
		return CreateAioTimer(fun);
	}
}
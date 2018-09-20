#include "net.h"
#include "gnet/log_gnet.h"
#include "bizutility/log/assert_log.h"
#include <sstream>			//stringstream
#include <process.h>		//_beginthreadex
#include <WS2tcpip.h>		//struct addrinfo


namespace gnet
{
	//////////////////////////////////////////////////////////////////////////
	unsigned CALLBACK ConnectThread(void* arg)
	{
		Connection* pCon = (Connection*)arg;
		if (pCon->connect())	//如果连接成功，则由ConnectTask释放Connection
		{
			Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new ConnectTask(*pCon)), NULL);
		}

		delete pCon;
		return 0;
	}


	unsigned CALLBACK ConnectPortListThread(void* arg)
	{
		Connection* pCon = (Connection*)arg;
		if (pCon->connect_portlist())	//如果连接成功，则由ConnectTask释放Connection
			Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new ConnectTask(*pCon)), NULL);

		delete pCon;
		return 0;
	}


	unsigned CALLBACK ListenThread(void* arg)
	{
		CListen* pListen = (CListen*)arg;
		if (pListen->listen())
		{
			Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new ListenTask(*pListen)), NULL);
		}

		delete pListen;
		return 0;
	}


	Connection::Connection(std::string host, std::string port, AioManager& manager, void* userData)
		:	m_host(host),
		m_port(port),
		m_errorno(0),
		m_manager(manager),
		m_aio(NULL),
		m_userData(userData)
	{
	}


	Connection::Connection(std::string host, std::vector<std::string>& portlist, AioManager& manager, void* userData)
		:	m_host(host),
		m_portlist(portlist),
		m_errorno(0),
		m_manager(manager),
		m_aio(NULL),
		m_userData(userData)
	{
	}


	Aio* Connection::connect()
	{
		struct addrinfo hints, *res;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if (0 != ::getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &res))
		{
			m_errorno = ::WSAGetLastError();
			m_error = "dns query failed";
			m_manager.OnConnectAbort(*this);
			return NULL;
		}

		WSASetLastError(WSAHOST_NOT_FOUND);
		SOCKET so = INVALID_SOCKET;
		char peerAddr[256];
		for (struct addrinfo* ai = res; ai != NULL; ai = ai->ai_next)
		{
			so = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
			if (so == INVALID_SOCKET)
				continue;

			if (SOCKET_ERROR != ::connect(so, ai->ai_addr, static_cast<int>(ai->ai_addrlen)))
			{
				m_error = "ok";
				assert_(0 == getnameinfo(ai->ai_addr, static_cast<socklen_t>(ai->ai_addrlen), 
					peerAddr, sizeof(peerAddr), NULL, 0, NI_NUMERICHOST));
				break;
			}
			closesocket(so);
			so = INVALID_SOCKET;
		}
		::freeaddrinfo(res);

		if (INVALID_SOCKET == so)
		{
			m_errorno = ::WSAGetLastError();
			m_error = "connect failed";
			m_manager.OnConnectAbort(*this);

			return NULL;
		}

		m_aio = new Aio(m_manager, so);
		m_aio->SetPeerAddr(peerAddr);
		m_aio->SetPeerPort(m_port.c_str());
		return m_aio;
	}


	Aio* Connection::connect_portlist()
	{
		struct addrinfo hints, *res;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		while (!m_portlist.empty())
		{
			srand((unsigned)time(NULL));
			int n = rand() % m_portlist.size();
			LOG << "Connection::connect_portlist while host " << m_host << ",  port " << m_portlist[n];
			if (0 != ::getaddrinfo(m_host.c_str(), m_portlist[n].c_str(), &hints, &res))
			{
				m_errorno = ::WSAGetLastError();
				m_error = "dns query failed";
				m_manager.OnConnectAbort(*this);
				return NULL;
			}
			m_portlist.erase(m_portlist.begin()+n);

			WSASetLastError(WSAHOST_NOT_FOUND);
			SOCKET so = INVALID_SOCKET;
			char peerAddr[256];
			for (struct addrinfo* ai = res; ai != NULL; ai = ai->ai_next)
			{
				so = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
				if (so == INVALID_SOCKET)
					continue;

				if (SOCKET_ERROR != ::connect(so, ai->ai_addr, static_cast<int>(ai->ai_addrlen)))
				{
					m_error = "ok";
					assert_(0 == getnameinfo(ai->ai_addr, static_cast<socklen_t>(ai->ai_addrlen), 
						peerAddr, sizeof(peerAddr), NULL, 0, NI_NUMERICHOST));
					break;
				}
				closesocket(so);
				so = INVALID_SOCKET;
			}
			::freeaddrinfo(res);

			if (INVALID_SOCKET == so)
				continue;

			m_aio = new Aio(m_manager, so);
			m_aio->SetPeerAddr(peerAddr);
			m_aio->SetPeerPort(m_port.c_str());
			return m_aio;
		}

		m_errorno = ::WSAGetLastError();
		m_error = "connect failed";
		m_manager.OnConnectAbort(*this);
		return NULL;
	}


	////////////////  CListen  //////////////////////////////////////////////////////////
	CListen::CListen(std::string host, std::string port, AioManager& manager, void* userData)
		:	m_host(host),
		m_port(port),
		m_errorno(0),
		m_manager(manager),
		m_aio(NULL),
		m_userData(userData)
	{

	}


	Aio* CListen::listen()
	{
		struct sockaddr_in saServer;
		saServer.sin_family = AF_INET;
		saServer.sin_addr.s_addr = INADDR_ANY;		//host ip暂时没用，让其自动选择
		unsigned short uport = (unsigned short)::atoi(m_port.c_str());
		saServer.sin_port = ::htons(uport);

		SOCKET so = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (so == INVALID_SOCKET)
		{
			m_errorno = ::WSAGetLastError();
			m_error = "socket failed";
			m_manager.OnListenAbort(*this);
			return NULL;
		}

		int nRet = ::bind(so, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
		if (SOCKET_ERROR == nRet)
		{
			m_errorno = ::WSAGetLastError();
			m_error = "bind failed";
			::closesocket(so);
			m_manager.OnListenAbort(*this);
			return NULL;
		}

		nRet = ::listen(so, 5);
		if (SOCKET_ERROR == nRet)
		{
			m_errorno = ::WSAGetLastError();
			m_error = "listen failed";
			::closesocket(so);
			m_manager.OnListenAbort(*this);
			return NULL;
		}

		m_aio = new Aio(m_manager, so);
		return m_aio;
	}



	/////////////////  Aio  /////////////////////////////////////////////////////////
	Aio::Aio(AioManager& manager, SOCKET so)
		:	m_socket(so),
		m_manager(manager),
		m_isec(Security::Create(NULLSECURITY)),
		m_osec(Security::Create(NULLSECURITY))
	{
		assert_(NULL != so);
		static SID gid = 0;
		static Mutex gidMutex;
		{
			Scoped lock(gidMutex);
			m_sid = ++gid;
			if (m_sid == (SID)(unsigned int)0)
				m_sid = ++gid;
		}
	}


	Aio::~Aio()
	{
		//这个detach不在网络线程做，所以要特别小心，要确定网络已经断开才能销毁
		Engine::GetInst()->Detach(m_socket, this);
		//::shutdown(m_socket, SD_SEND);	//???
		::closesocket(m_socket);
		m_isec->Destroy();
		m_osec->Destroy();
	}


	void Aio::Send(const Octets& oct)
	{
		Octets sec(oct);
		Scoped lock(this->mutex);
		m_osec->Update(sec);

		if (m_output.size() > 0)
		{
			m_output.insert(m_output.end(), sec.begin(), sec.size());
			return;
		}

		char* buf = (char*)sec.begin();
		int len = static_cast<int>(sec.size());
		int rc = ::send(m_socket, buf, len, 0);
		while(rc > 0 && rc <len)
		{
			len -= rc;
			buf += rc;
			rc = ::send(m_socket, buf, len, 0);
		}

		if (SOCKET_ERROR == rc)
		{
			if(WSAEWOULDBLOCK != WSAGetLastError())
			{
				Close("Error Send");
				return;
			}
			rc = 0;
		}

		if (len > rc)
		{
			m_output.insert(m_output.end(), buf+rc, len-rc);
		}
	}


	void Aio::Close(const char* lastinfo)
	{
		SetLastInfo(lastinfo);
		Engine::GetInst()->PostMsg(WM_AIO_NOTIFY, (WPARAM)m_socket, WSAMAKESELECTREPLY(FD_AIO_CLOSE, 0));
	}


	void Aio::SetLastInfo(const char* info)
	{
		if (NULL != info)
			if (m_lastInfo.empty())
				m_lastInfo = info;
	}


	void Aio::SetISecurity(Security::Type type, const Octets& key)
	{
		Scoped lock(this->mutex);
		m_isec->Destroy();
		m_isec = Security::Create(type);
		m_isec->SetParameter(key);
	}


	void Aio::SetOSecurity(Security::Type type, const Octets& key)
	{
		Scoped lock(this->mutex);
		m_osec->Destroy();
		m_osec = Security::Create(type);
		m_osec->SetParameter(key);
	}


	void Aio::OnAccept()
	{
		struct sockaddr_in sAddr;
		int len = sizeof(sockaddr_in);
		SOCKET so = ::accept(m_socket, (struct sockaddr*)&sAddr, &len);
		if (INVALID_SOCKET == so)
		{
			assert_(false && "::accept()");
			return;
		}

		const char chOpt = 1;
		int nRet = ::setsockopt(so, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
		if (SOCKET_ERROR == nRet)
		{
			assert_(false && "::setsockopt()");
			int err = ::WSAGetLastError();
			::closesocket(so);
			return;
		}

		std::stringstream ss(std::stringstream::in);
		ss << sAddr.sin_port;

		Aio* aioNew = new Aio(m_manager, so);
		if (Engine::GetInst()->Attach(aioNew, FD_READ|FD_WRITE|FD_CLOSE))
		{
			Connection con(::inet_ntoa(sAddr.sin_addr), ss.str(), m_manager, NULL);
			m_manager.OnConnect(aioNew->GetSid(), con);
		}
	}


	void Aio::OnRecv()
	{
		Octets buffer(4096);
		char* pBuf = (char*)buffer.begin();
		int len = static_cast<int>(buffer.capacity());
		int rc = ::recv(m_socket, pBuf, len, 0);
		if (rc == 0)
			return Close("peer closed");
		if (rc < 0)
			return Close("error recv");

		buffer.resize(rc);
		{
			Scoped lock(this->mutex);
			m_isec->Update(buffer);
		}
		m_input.insert(m_input.end(), buffer.begin(), buffer.size());
		Process(m_input);
	}


	void Aio::OnSend()
	{	
		Scoped lock(this->mutex);
		if (m_output.size() > 0)
		{
			char* buf = (char*)m_output.begin();
			int len = static_cast<int>(m_output.size());
			int rc = ::send(m_socket, buf, len, 0);
			while(rc > 0 && rc <len)
			{
				len -= rc;
				buf += rc;
				m_output.erase(0, rc);
				rc = ::send(m_socket, buf, len, 0);
			}

			if (SOCKET_ERROR == rc)
			{
				if(WSAEWOULDBLOCK != WSAGetLastError())
					Close("Error OnSend");
				return;
			}

			m_output.erase(0, rc);
		}
	}


	void Aio::OnClose()
	{
		LOG << "[" << m_sid << "] Aio::OnClose, last info: " << m_lastInfo;
		m_manager.OnClose(m_sid, m_lastInfo);
	}


	void Aio::OnError(int event, int error)
	{
		LOG << "[" << m_sid << "] Aio::OnError, fdevent: " << event << ", errno: " << error;
		m_manager.OnError(m_sid, event, error);
	}


	void Aio::Process(Octets& input)
	{
		m_manager.Process(m_sid, input);
	}



	////////////////  AioManager  //////////////////////////////////////////////////////////
	void AioManager::Connect(const char* host, const char* port, void* userData)
	{
		Connection* pCon = new Connection(host, port, *this, userData);

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ConnectThread, pCon, NULL, NULL);
		if (hThread == NULL)
		{
			pCon->SetError("_beginthread failed");
			OnConnectAbort(*pCon);
			delete pCon;
			return;
		}
		CloseHandle(hThread);
	}


	void AioManager::Connect(const char* host, std::vector<std::string> portlist, void* userData)
	{
		Connection* pCon = new Connection(host, portlist, *this, userData);

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ConnectPortListThread, pCon, NULL, NULL);
		if (hThread == NULL)
		{
			pCon->SetError("_beginthread failed");
			OnConnectAbort(*pCon);
			delete pCon;
			return;
		}
		CloseHandle(hThread);
	}


	void AioManager::Send(Aio::SID sid, const Octets& oct)
	{
		if (Engine::_engineThreadID == GetCurrentThreadId())
		{
			Aio* aio = Engine::GetInst()->FindAioBySID(sid);
			assert_(NULL != aio);
			if (NULL == aio)
				return;

			aio->Send(oct);
		}
		else
		{
			Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new SendBySID(sid, oct)), NULL);
		}
	}


	void AioManager::Close(Aio::SID sid, const char* lastinfo)
	{
		if (Engine::_engineThreadID == GetCurrentThreadId())
		{
			Aio* aio = Engine::GetInst()->FindAioBySID(sid);
			assert_(NULL != aio);
			if (NULL == aio)
				return;

			aio->Close(lastinfo);
		}
		else
		{
			std::string strinfo = (NULL==lastinfo)?"":lastinfo;
			Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new CloseBySID(sid, strinfo)), NULL);
		}
	}


	void AioManager::Listen(const char* host, const char* port, void* userData)
	{
		CListen* pListen = new CListen(host, port, *this, userData);

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, pListen, NULL, NULL);
		if (hThread == NULL)
		{
			pListen->SetError("_beginthread failed");
			OnListenAbort(*pListen);
			delete pListen;
			return;
		}
		CloseHandle(hThread);
	}


	void AioManager::SetISecurity(Aio::SID sid, Security::Type type, const Octets key)
	{
		Aio* aio = Engine::GetInst()->FindAioBySID(sid);
		assert_(NULL != aio);
		if (NULL == aio)
			return;

		aio->SetISecurity(type, key);
	}


	void AioManager::SetOSecurity(Aio::SID sid, Security::Type type, const Octets key)
	{
		Aio* aio = Engine::GetInst()->FindAioBySID(sid);
		assert_(NULL != aio);
		if (NULL == aio)
			return;

		aio->SetOSecurity(type, key);
	}


	void AioManager::PostSetISecurity(Aio::SID sid, Security::Type type, const Octets key)
	{
		Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new SetSecurity(sid, type, key, false)), NULL);
	}


	void AioManager::PostSetOSecurity(Aio::SID sid, Security::Type type, const Octets key)
	{
		Engine::GetInst()->PostMsg(WM_AIO_TASK, (WPARAM)(new SetSecurity(sid, type, key, true)), NULL);
	}


	void AioManager::OnConnectAbort(const Connection& con)
	{
		int err;
		const char* errstr = con.GetError(err);
		LOG << "AioManager::OnConnectAbort： errorno: " << err << ", " << errstr;
	}


	void AioManager::OnConnect(Aio::SID sid, const Connection& con)
	{
		LOG << "[" << sid << "] AioManager::OnConnect";
	}


	void AioManager::OnListenAbort(const CListen& clisten)
	{
		int err;
		const char* errstr = clisten.GetError(err);
		LOG << "AioManager::OnListenAbort： errorno: " << err << ", " << errstr;
	}


	void AioManager::OnListen(Aio::SID sid, const CListen& listen)
	{
		LOG << "[" << sid << "] AioManager::OnListen";
	}


	void AioManager::Process(Aio::SID, Octets& input)
	{
		input.clear();
	}


	////////////////  Engine  //////////////////////////////////////////////////////////
	DWORD	Engine::_engineThreadID = 0;
	Engine::Engine()
		:	m_hThread(NULL),
		m_hMain(NULL)
	{

	};


	Engine* Engine::GetInst()
	{
		static Engine e;
		return &e;
	}


	bool Engine::StartUp()
	{
		WSADATA wData;
		if (0 != ::WSAStartup(MAKEWORD(2, 2), &wData))
			return false;

		StartUpArg arg;
		m_hThread = (HANDLE)::_beginthreadex(NULL, 0, EngineThread, &arg, CREATE_SUSPENDED, NULL);
		if (0 != m_hThread)
		{
			::ResumeThread(m_hThread);
			arg.WaitReady();
			if (NULL != (m_hMain = arg.hMain))	// so go on here
				return true;	//启动成功

			//启动失败
			::WaitForSingleObject(m_hThread, INFINITE);
			::CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		::WSACleanup();
		return false;
	}


	void Engine::CleanUp()
	{
		if (m_hMain)
		{
			if (FALSE == ::PostMessage(m_hMain, WM_CLOSE, 0, 0))
				::SendMessage(m_hMain, WM_CLOSE, 0, 0);
			::WaitForSingleObject(m_hThread, INFINITE);
			::CloseHandle(m_hThread);
			m_hThread = NULL;
			m_hMain = NULL;
		}

		::WSACleanup();	//在等待Connect线程退出之前卸载库，迫使Connect线程退出


		//删除没有关闭的连接（不用保护，因为对m_mapAio操作都在主线程）
		std::map<SOCKET, Aio*> tmp;
		tmp.swap(m_mapAio);
		for (std::map<SOCKET, Aio*>::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
			it->second->SetLastInfo("Engine::CleanUp");
			delete it->second;
		}
	}


	bool Engine::Attach(Aio* aio, long lEvent)
	{
		assert_(NULL != aio);
		assert_(_engineThreadID == GetCurrentThreadId());
		if (m_mapAio.insert(std::make_pair(aio->m_socket, aio)).second)
		{
			if (SOCKET_ERROR != ::WSAAsyncSelect(aio->m_socket, m_hMain, WM_AIO_NOTIFY, lEvent))
				return true;
			m_mapAio.erase(aio->m_socket);
		}
		return false;
	}


	void Engine::Detach(SOCKET so, Aio* aio)
	{
		// CleanUp的时候可能不在网络线程
		// assert_(_engineThreadID == GetCurrentThreadId());
		std::map<SOCKET, Aio*>::iterator it = m_mapAio.find(so);
		if (it != m_mapAio.end())
			if(it->second == aio)
				m_mapAio.erase(it);
	}


	void Engine::PostMsg(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		while(!::PostMessage(m_hMain, msg, wParam, lParam)){}
	}


	void Engine::AioNotify(SOCKET socket, int fdEvent, int error)
	{
		std::auto_ptr<Aio> aio(FindAio(socket));
		if (!aio.get())
			return;

		try
		{
			if (error != 0)
				return aio->OnError(fdEvent, error);

			switch(fdEvent)
			{
			case FD_READ:		aio->OnRecv(); break;
			case FD_WRITE:		aio->OnSend(); break;
				//case FD_CONNECT:	aio->OnConnect(NULL); break;	//???
			case FD_ACCEPT:		aio->OnAccept(); break;

			case FD_CLOSE:
				aio->SetLastInfo("FD_CLOSE Server close");
				aio->OnClose();
				return;

			case FD_AIO_CLOSE:
				aio->OnClose();	
				return;

			case FD_OOB:
				aio->OnError(fdEvent, error);
				return;
			}
			aio.release();
		}
		//marshal except
		catch(std::exception& ex)
		{
			*assert_(false) << ex.what();	  // 如果是debug，下面永远执行不到，导致重复发送
			aio->SetLastInfo(ex.what());  // 协议异常处理
			aio->OnClose();
		}
	}


	Aio* Engine::FindAio(SOCKET so)
	{
		assert_(_engineThreadID == GetCurrentThreadId());
		std::map<SOCKET, Aio*>::iterator it = m_mapAio.find(so);
		if (it != m_mapAio.end())
			return it->second;
		return NULL;
	}


	Aio* Engine::FindAioBySID(Aio::SID sid)
	{
		assert_(_engineThreadID == GetCurrentThreadId());
		std::map<SOCKET, Aio*>::iterator it = m_mapAio.begin();
		for (; it != m_mapAio.end(); it++)
		{
			if (it->second->GetSid() == sid)
			{
				return it->second;
			}
		}
		return NULL;
	}


	unsigned CALLBACK Engine::EngineThread(void* arg)
	{
		_engineThreadID = GetCurrentThreadId();
		const wchar_t* className = L"__AIO_ENGINE__";
		if (!RegisterWindowClass(className))
		{
			((StartUpArg*)arg)->SetReady(NULL);
			return 0;
		}

		HWND hMain = ::CreateWindowW(className, L"", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
		((StartUpArg*)arg)->SetReady(hMain);
		if (hMain)
		{
			MSG msg;
			while (::GetMessage(&msg, hMain, 0, 0) > 0)
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		::UnregisterClassW(className, GetModuleHandle(NULL));
		return 0;
	}


	bool Engine::RegisterWindowClass(const wchar_t* className)
	{
		WNDCLASSEXW wc;
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = 0;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = className;
		return (NULL != ::RegisterClassExW(&wc) || 
			ERROR_CLASS_ALREADY_EXISTS == ::GetLastError());
	}


	LRESULT WINAPI Engine::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_AIO_NOTIFY:
			Engine::GetInst()->AioNotify((SOCKET)wParam, WSAGETSELECTEVENT(lParam), WSAGETSELECTERROR(lParam));
			break;

		case WM_AIO_TASK:
			((Task*)wParam)->RunDelete();
			break;

		case WM_DESTROY:
			::PostQuitMessage(0);
			break;

		default:
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}


	void CALLBACK Engine::TimerProc(HWND, UINT, UINT_PTR idEvent, DWORD)
	{
		((AioTimer*)idEvent)->OnAioTimer();
	}


	bool Engine::StartTimer(AioTimer& timer, int period /*ms*/)
	{
		return 0 != ::SetTimer(m_hMain, (UINT_PTR)&timer, period, TimerProc);
	}


	void Engine::StopTimer(AioTimer& timer)
	{
		::KillTimer(m_hMain, (UINT_PTR)&timer);
	}


	//////////////  AioTimer  ////////////////////////////////////////////////////////////
	bool AioTimer::Start(int period)
	{
		Stop();
		bool m_bIsRuning = Engine::GetInst()->StartTimer(*this, period);
		return m_bIsRuning;
	}


	void AioTimer::Stop()
	{
		Engine::GetInst()->StopTimer(*this);
		m_bIsRuning = false;
	}


	AioTimer::AioTimer()
		:	m_bIsRuning(false)
	{

	}
}
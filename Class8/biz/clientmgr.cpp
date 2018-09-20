#include "biz.h"
#include "clientmgr.h"
#include "log_biz.h"
#include "bizutility/log/assert_log.h"
#include "protocol/rpcgen_edu.h"

#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")  

static bool GetMac(wstring& strMac)     
{		
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS   adapt;
		NAME_BUFFER		 NameBuff[30];
	}ASTAT,*PASTAT;
	
	typedef struct _LANA_ENUM
	{
		UCHAR   length;
		UCHAR   lana[MAX_LANA];
	}LANA_ENUM;
	
	NCB			ncb;
	ASTAT		Adapter;
	LANA_ENUM	lana_enum;    
	UCHAR		ret;
	wchar_t		wszMac[64]={0};

	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));    
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	ret = Netbios(&ncb);
				
	if(ret != NRC_GOODRET)     
	{
		return false;
	}
				
	for(int lana=0; lana<lana_enum.length; lana++)     
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		ret = Netbios(&ncb);

		if(ret == NRC_GOODRET)
		{
			break; 
		}
	}
						
	if(ret != NRC_GOODRET)
	{
		return false;
	}
						
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy((char*)ncb.ncb_callname, "*");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	ret = Netbios(&ncb);
						
	if(ret != NRC_GOODRET)
	{
		return false;
	}
						
	swprintf(wszMac,L"%02X-%02X-%02X-%02X-%02X-%02X",
		Adapter.adapt.adapter_address[0],
		Adapter.adapt.adapter_address[1],
		Adapter.adapt.adapter_address[2],
		Adapter.adapt.adapter_address[3],
		Adapter.adapt.adapter_address[4],
		Adapter.adapt.adapter_address[5]);

	strMac = wszMac;
	return true;
}

namespace biz
{	
	ClientManager::ClientManager() : _sid (-1), 
		_pCallBack(NULL),
		_userData(NULL)
	{
		GetMac(_strDevName);
		WCHAR wszDevName[MAX_PATH] = { 0 };
		swprintf(wszDevName,L"BMClass PC Client [%s]",(WCHAR*)_strDevName.c_str());
		_strDevName = wszDevName;
	}


	void ClientManager::OnConnectAbort(const gnet::Connection& con)
	{
		LOG << "OnConnectAbort";
		
		Biz::GetInst()->OnConnectServer(false);

		if(_pCallBack)
		{
			_pCallBack->OnConnectServerError(eServerState_ConFailed, "connect server failed");
		}
	}


	void ClientManager::OnConnect(gnet::Aio::SID sid, const gnet::Connection& con)
	{
		LOG << "OnConnect sid: " << sid;
		_sid = sid;

		Biz::GetInst()->OnConnectServer(true);

		if(_pCallBack)
		{
			_pCallBack->OnConnectServerError(eServerState_connect, "connect server successed");
		}
	}


	void ClientManager::OnClose(gnet::Aio::SID sid, std::string errStr)
	{
		LOG << "OnClose sid: " << sid;
		if(_sid == -1)
		{
			return;
		}

		_sid = -1;
		if(_pCallBack)
		{
			_pCallBack->OnConnectServerError(eServerState_ConClose, "server close");
		}

		Biz::GetInst()->OnConnectServer(false);
	}


	void ClientManager::OnError(gnet::Aio::SID sid, int event, int error)
	{
	//	int i = GetLastError();
	//	return;		

		if(_pCallBack)
		{
			_pCallBack->OnConnectServerError(eServerState_ConDis, "server error");
		}
		
		Biz::GetInst()->OnConnectServer(false);
	}


	void ClientManager::ProcessProtocol(gnet::Aio::SID sid, gnet::Protocol& p)
	{
		if (-1 == _sid)
		{
			return;
		}

		assert_(sid == _sid);

		LOG << "ProcessProtocol sid: " << sid << " type: " << p.GetType();
		Biz::GetInst()->GetDispatcher().PostTask(Biz::GetInst(), &Biz::ProcessProtocol, sid, p.Clone());
	}
	
	void ClientManager::Close()
	{
		if (-1 == _sid)
		{
			return;
		}

		gnet::Aio::SID nCloseSid = _sid;
		_sid = -1;
		
		Close(nCloseSid, "Client Close");		
	}


	void ClientManager::Send(const gnet::Protocol& protocol)
	{
		if (-1 == _sid)
		{
			return;
		}

		Send(_sid, protocol);
	}

	void ClientManager::SetCallback(ICallback* pCallBack)
	{
		_pCallBack = pCallBack;
	}

	void ClientManager::SetServerInfo(const char* pIp, const char* pPort, void* userData)
	{
		if(pPort == NULL || pIp == NULL)
		{
			return;
		}

		_strServerPort = pPort;
		_strServerIp = pIp;
		_userData = userData;
	}

	void ClientManager::ConnectServer()
	{
		if(_sid != -1)
		{
			return;
		}

		Connect(_strServerIp.c_str(), _strServerPort.c_str(), NULL);
	}

	void ClientManager::ReconnectServer()
	{
		Close();
		Connect(_strServerIp.c_str(), _strServerPort.c_str(), NULL);
	}
}
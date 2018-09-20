#include "udpprocess.h"

CUDPProcess::CUDPProcess():CNetProcess()
{
	m_UDPClinet = NULL;
}

CUDPProcess::~CUDPProcess()
{
	udp_close();
}

void CUDPProcess::Activate(const char* szUrl,long nUserID)
{
	if(m_UDPClinet == NULL)
		m_UDPClinet = new CUDPClinet(szUrl);
	m_nUserID = nUserID;
	
}

bool CUDPProcess::SetCommond(moudle_commond_t& cmd,void* Param)
{
	bool bresult = false;
	switch(cmd)
	{
	case OPEN:
		bresult = udp_open();
		break;
	case CLOSE:
		bresult = udp_close();
		break;
	case PAUSE:
		bresult = udp_pasue();
		break;
	case STOP:
		bresult = udp_stop();
		break;
	default:
		break;
	}
	return bresult;
}

bool CUDPProcess::udp_open()
{
	if(m_UDPClinet)
	{
		return m_UDPClinet->InitUDPClient(m_nUserID,UDPPULL);
	}
	return false;
}

bool CUDPProcess::udp_close()
{
	if(m_UDPClinet)
	{
		m_UDPClinet->UnInitUDPClient();
		delete m_UDPClinet;
		m_UDPClinet = NULL;
		return true;
	}
	return false;
}

bool CUDPProcess::udp_pasue()
{
	return true;
}

bool CUDPProcess::udp_stop(bool bIsClose)
{
	if(m_UDPClinet)
	{
		return m_UDPClinet->UnInitUDPClient();
	}
	return true;
}

int  CUDPProcess::ReadMedia(unsigned char* pBuf,unsigned nBufSize)
{
	if(m_UDPClinet)
	{
		return m_UDPClinet->recvData(pBuf,nBufSize);
	}
	return 0;
}
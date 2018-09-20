#ifndef UDPPROCESS_H
#define UDPPROCESS_H
#include "INetProcess.h"
#include "CUDPClient.h"

class CUDPProcess:public CNetProcess
{
public:
	CUDPProcess();
	virtual ~CUDPProcess();
public:
    virtual void Activate(const char* szUrl,long nUserID) ;
	virtual bool SetCommond(moudle_commond_t& cmd,void* Param);
	virtual int ReadMedia(unsigned char* pBuf,unsigned nBufSize);
private:
	bool udp_open();
	bool udp_close();
	bool udp_pasue();
	bool udp_stop(bool bIsClose=false);
private:
	CUDPClinet * m_UDPClinet;
	long         m_nUserID;
};
#endif
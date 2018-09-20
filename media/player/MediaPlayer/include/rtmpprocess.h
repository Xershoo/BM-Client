#ifndef RTMPPROCESS_H
#define RTMPPROCESS_H

#include "INetProcess.h"
#include "sc_Thread.h"
#include "sc_CSLock.h"
#include "rtmp.h"
#include <list>

using namespace std;

#define RTMP_UNINIT 0
#define RTMP_PLAY 1
#define RTMP_PAUSE 2
#define RTMP_STOP 3
#define RTMP_OPEN 4


struct RtmpAddr
{
	char szKey[256];
	char szRealIP[32];
	unsigned short nPort;
	RtmpAddr()
	{
		szKey[0] = '\0';
		szRealIP[0] = '\0';
		nPort = 0;
	}

	void copy(const char* szAddrKey,const char* szAddrRealIP,unsigned short usPort)
	{
		strcpy(szKey,szAddrKey);
		strcpy(szRealIP,szAddrRealIP);
		nPort = usPort;
	}

	bool cmpIsMapping(const char* szAddrKey)
	{
		if(strcmp(szKey,szAddrKey) == 0)
		{
			return true;
		}
		return false;
	}

};



class CRTMPProcess:public CNetProcess
{
public:
	CRTMPProcess();
	virtual ~CRTMPProcess();
public:
	virtual void Activate(const char* szUrl,long nUserID);
	virtual bool SetCommond(moudle_commond_t& cmd,void* Param);
	virtual int ReadMedia(unsigned char* pBuf,unsigned nBufSize);
	
private:
	void rtmp_connect();

	bool rtmp_open();
	bool rtmp_close();
	bool rtmp_pasue();
	bool rtmp_stop(bool bIsClose=false);
	bool rtmp_seek(void* Param);
	bool rtmp_getduration(void* Param);

	void InitRTMPAddlist();
	bool FindRealAddrByKey(const char* szHostaddr,RtmpAddr& ra);
private:
	char*          m_szRtmpUrl;
	unsigned int   m_nMemSize;
	RTMP *         m_prtmp;
	unsigned char  m_status;
	bool           m_bDoPause;
	CMutexLock     m_PlayLock;
	unsigned long  m_dwLastConnectTime;

private:
	static CMutexLock m_ThreadLock;
private:
	list<RtmpAddr> m_listRTMPAddr;
};

#endif
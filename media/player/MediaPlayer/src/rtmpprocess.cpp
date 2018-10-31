
#include "stdafx.h"
#include "rtmpprocess.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <winsock2.h>

CMutexLock CRTMPProcess::m_ThreadLock;

int InitSockets()  
{  
	WORD version;  
	WSADATA wsaData;  
	version = MAKEWORD(1, 1);  
	return (WSAStartup(version, &wsaData) == 0);    
}  
void CleanupSockets()  
{  
	WSACleanup();  
}  


bool CRTMPProcess::FindRealAddrByKey(const char* szHostaddr,RtmpAddr& ra)
{
	char *szHeader = "rtmp://";
	int nHeaderSize =  strlen(szHeader);
	char szKey[256] = {'\0'};
	strcpy(szKey,szHeader);
	if(strncmp(szHeader,szHostaddr,nHeaderSize) != 0)
		return false;
	char *p = (char*)szHostaddr + nHeaderSize;
	int i = nHeaderSize;
	while(*p != '\0' && *p != '\/')
	{
		szKey[i++] = *p++;
	}
	szKey[i] = '\0';

	if(m_listRTMPAddr.size() > 0)
	{
		list<RtmpAddr>::iterator iter;
		for(iter = m_listRTMPAddr.begin();iter != m_listRTMPAddr.end();iter++)
		{
			RtmpAddr rnode = (*iter);
			if(rnode.cmpIsMapping(szKey))
			{
				ra = rnode;
				return true;
			}
		}
	}
	return false;
}

void CRTMPProcess::InitRTMPAddlist()
{
	RtmpAddr ra;
	ra.copy("rtmp://class8media.address.ch1","61.147.188.51",1935);
	m_listRTMPAddr.push_back(ra);
	ra.copy("rtmp://class8media.address.ch2","61.147.188.52",1935);
	m_listRTMPAddr.push_back(ra);
	ra.copy("rtmp://class8media.address.ch0","10.2.2.234",1935);
	m_listRTMPAddr.push_back(ra);
}

CRTMPProcess::CRTMPProcess():CNetProcess()
{
	m_szRtmpUrl = NULL;
	m_nMemSize = 0;
	m_prtmp =  NULL;
	m_status = RTMP_UNINIT;

	m_bDoPause = true;
	m_dwLastConnectTime = 0;
	InitSockets() ;
	InitRTMPAddlist();
}

CRTMPProcess::~CRTMPProcess()
{

	rtmp_close();

	if(m_szRtmpUrl)
	{
		free(m_szRtmpUrl);
		m_szRtmpUrl= NULL;
	}
	m_status = RTMP_UNINIT;
}


void CRTMPProcess::Activate(const char* szUrl,long nUserID)
{
	if(!szUrl || strncmp(szUrl,"rtmp://",strlen("rtmp://")) != 0)
		return ;
	int nUrlLen = strlen(szUrl);

	if(m_nMemSize <= nUrlLen)
	{
		m_nMemSize = nUrlLen * 2;
		if(m_szRtmpUrl)
		{
			free(m_szRtmpUrl);
			m_szRtmpUrl = NULL;
		}
	}

	if(m_szRtmpUrl == NULL)
	{
		m_szRtmpUrl = (char*)malloc(m_nMemSize);
		assert(m_szRtmpUrl);
	}

	strncpy(m_szRtmpUrl,szUrl,nUrlLen);
	m_szRtmpUrl[nUrlLen] = '\0';

	if(m_prtmp == NULL)
	{
		m_prtmp = RTMP_Alloc();  //申请内存;
		assert(m_prtmp);
		RTMP_Init(m_prtmp);  //初始化结构体
	}

}

bool CRTMPProcess::SetCommond(moudle_commond_t& cmd,void* Param)
{
	bool bresult = false;
	switch(cmd)
	{
	case OPEN:
		bresult = rtmp_open();
		break;
	case CLOSE:
		bresult = rtmp_close();
		break;
	case PAUSE:
		bresult = rtmp_pasue();
		break;
	case STOP:
		bresult = rtmp_stop();
		break;
	case SEEK:
		bresult = rtmp_seek(Param);
	case DURATION:
		bresult = rtmp_getduration(Param);
	default:
		break;
	}
	return bresult;
}


void CRTMPProcess::rtmp_connect()
{
	if(m_prtmp && (m_status == RTMP_OPEN ||!RTMP_IsConnected(m_prtmp)))
	{
		RTMP_Init(m_prtmp);  //初始化结构体
		if(RTMP_SetupURL(m_prtmp, m_szRtmpUrl))
		{
			RtmpAddr ra;
			if(!FindRealAddrByKey(m_szRtmpUrl,ra))
			{
				if( RTMP_Connect(m_prtmp, NULL))
				{
					if(RTMP_ConnectStream(m_prtmp,0))
					{
						m_status = RTMP_PLAY;
						int nTimeOut = 3000;
						setsockopt(m_prtmp->m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeOut, sizeof(nTimeOut));
					}
				} 
			}
			else
			{
				if( RTMP_ConnectEx(m_prtmp, NULL,ra.szRealIP,ra.nPort))
				{
					if(RTMP_ConnectStream(m_prtmp,0))
					{
						m_status = RTMP_PLAY;
						int nTimeOut = 3000;
						setsockopt(m_prtmp->m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeOut, sizeof(nTimeOut));
					}
				} 
			}
		}
	}
}

bool CRTMPProcess::rtmp_open()
{

	if(!m_szRtmpUrl || strncmp(m_szRtmpUrl,"rtmp://",7) != 0 || m_prtmp == NULL) 
		return false;
	m_status = RTMP_OPEN;
	return true;
}

bool CRTMPProcess::rtmp_close()
{
	if(m_prtmp)
	{
		rtmp_stop(true);
		RTMP_Free(m_prtmp);
		m_prtmp = NULL;
	}
	return true;
}

bool CRTMPProcess::rtmp_pasue()
{
	if(m_prtmp && RTMP_IsConnected(m_prtmp)
		&& (m_status == RTMP_PAUSE  || m_status == RTMP_PLAY))
	{
		bool bDoPause = m_bDoPause;
		if(bDoPause)
		{
			m_status = RTMP_PAUSE;
		}
		else
		{
			m_status = RTMP_PLAY;
		}

		m_bDoPause = !m_bDoPause;
		return RTMP_Pause(m_prtmp,bDoPause);
	}
	return false;
}

bool CRTMPProcess::rtmp_stop(bool bIsClose)
{
	
	m_status = RTMP_STOP;
	if(bIsClose)
	{
		if(m_prtmp /*&& RTMP_IsConnected(m_prtmp)*/)
		{
			RTMP_Close(m_prtmp);
		}
	}
	else
	{
		closesocket(m_prtmp->m_sb.sb_socket);
		m_prtmp->m_sb.sb_socket = -1;
	}

	return true;
}

bool CRTMPProcess::rtmp_seek(void* Param)
{
	if(Param && m_prtmp && RTMP_IsConnected(m_prtmp)
		&& (m_status == RTMP_PAUSE  || m_status == RTMP_PLAY))
	{
		int nTime = 0;
		memcpy(&nTime,Param,sizeof(int));
		double dbDuration = RTMP_GetDuration(m_prtmp);

		if(((double)nTime) < dbDuration)
		{
			return RTMP_SendSeek(m_prtmp,nTime*1000);
		}
	}
	return false;
}

bool CRTMPProcess::rtmp_getduration(void* Param)
{
	if(Param && m_prtmp && RTMP_IsConnected(m_prtmp)
		&& (m_status == RTMP_PAUSE || m_status == RTMP_PLAY))
	{
		double dbTime  = RTMP_GetDuration(m_prtmp);
		memcpy(Param,&dbTime,sizeof(dbTime));
		return true;
	}
	return false;
}


int CRTMPProcess::ReadMedia(unsigned char* pBuf,unsigned nBufSize)
{
	int nReadSize = 0;
	
	
	if(m_prtmp)
	{ 
		if(m_status == RTMP_PLAY || m_status == RTMP_OPEN)
		{
			if(!RTMP_IsConnected(m_prtmp))
			{
				DWORD dwCurTime = GetTickCount();
				DWORD dwDiff = dwCurTime - m_dwLastConnectTime;
				if(dwDiff > 5000)
				{
					rtmp_connect();
					m_dwLastConnectTime = dwCurTime;
				}
			}
			if(RTMP_IsConnected(m_prtmp))
			{
				nReadSize = RTMP_Read(m_prtmp,(char*)pBuf,nBufSize);
				if(nReadSize <= 0 && RTMP_IsConnected(m_prtmp))
				{
					RTMP_Close(m_prtmp);
				}
			}
		}
	}
	return nReadSize;
}



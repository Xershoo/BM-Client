#include <Windows.h>
#include "CUDPClient.h"
#include <stdio.h>
#include <string.h>

#define PULLAUDIO 0x00001000
#define PUSHAUDIO 0x00002000
#define ENDPULL   0x00001001
#define ENDPUSH   0x00002001

#define WS_VERSION_CHOICE1 0x202/*MAKEWORD(2,2)*/
#define WS_VERSION_CHOICE2 0x101/*MAKEWORD(1,1)*/

#ifdef HAVE_SOCKADDR_LEN
#define SET_SOCKADDR_SIN_LEN(var) var.sin_len = sizeof var
#else
#define SET_SOCKADDR_SIN_LEN(var)
#endif

#define MAKE_SOCKADDR_IN(var,adr,prt)\
    struct sockaddr_in var;\
    var.sin_family = AF_INET;\
    var.sin_addr.s_addr = (adr);\
    var.sin_port = (prt);\
    SET_SOCKADDR_SIN_LEN(var);


static const char flvHeader[] = { 'F', 'L', 'V', 0x01,
  0x00,				/* 0x04 == audio, 0x01 == video */
  0x00, 0x00, 0x00, 0x09,
  0x00, 0x00, 0x00, 0x00
};

void CALLBACK recv_media_loop(void *pUserData)
{
	printf("udp recv thread start success!\n");
	CUDPClinet * pUDPClient  = (CUDPClinet *)pUserData;
	pUDPClient->recv_loop();
}

bool  CUDPClinet::InitNetEnv()
{
	WSADATA	wsadata;
	if ((WSAStartup(WS_VERSION_CHOICE1, &wsadata) != 0)
		&& ((WSAStartup(WS_VERSION_CHOICE2, &wsadata)) != 0)) 
	{
		return false; /* error in initialization */
	}

	if ((wsadata.wVersion != WS_VERSION_CHOICE1) && (wsadata.wVersion != WS_VERSION_CHOICE2)) 
	{
	    WSACleanup();
	    return false; /* desired Winsock version was not available */
	}
	return true;
}

CUDPClinet::CUDPClinet(unsigned int nDestIP,unsigned short nPDestPort,const char* szStreamID)
{
	m_nDestIP = nDestIP;
	m_szDestIP[0] = '\0';
	m_nDestPort = nPDestPort;
	m_sendSocket = -1;
	m_recvSocket = -1;
	m_nRTPPayloadType = 0;
	m_readLastPacketTime = 0;
	m_SedLosePacketTime = 0;
	m_statTime = 0;
	m_recvbufsize = 16 * 1024;
	strcpy(m_szStreamID,szStreamID);
	m_recvbuf = (unsigned char*)malloc(m_recvbufsize);
	InitNetEnv();
	m_bIsFirstRead = true;
	m_nSSRC = 0;
	m_bIsLogin = false;
	m_szLogin[0] = '\0';
	m_nMaxKeepAliveWaitTime = 30;
	m_lastGetLiveTime = 0;
	m_bIsReHello = false;
	sprintf(m_szLogin,"Welcome@%s",m_szStreamID);
}

CUDPClinet::CUDPClinet(const char* szDestIP,unsigned short nPDestPort,const char* szStreamID)
{
	m_nDestIP = 0;
	strcpy(m_szDestIP,szDestIP);
	m_nDestPort = nPDestPort;
	m_nDestIP = inet_addr(m_szDestIP );
	m_sendSocket = -1;
	m_recvSocket = -1;
	m_nRTPPayloadType = 0;
	m_statTime = 0;
	m_recvbufsize = 16 * 1024;
	m_recvbuf = (unsigned char*)malloc(m_recvbufsize);
	strcpy(m_szStreamID,szStreamID);
	InitNetEnv();
	m_bIsFirstRead = true;
	m_nSSRC = 0;
	m_bIsLogin = false;
	m_szLogin[0] = '\0';
	m_nMaxKeepAliveWaitTime = 30;
	m_lastGetLiveTime = 0;
	m_bIsReHello = false;
	sprintf(m_szLogin,"Welcome@%s",m_szStreamID);
}

CUDPClinet::CUDPClinet(const char* szStreamURL)
{
	m_nDestIP = 0;
	m_nDestIP = inet_addr(m_szDestIP);
	m_sendSocket = -1;
	m_recvSocket = -1;
	m_nRTPPayloadType = 0;
	m_statTime = 0;
	m_recvbufsize = 16 * 1024;
	m_recvbuf = (unsigned char*)malloc(m_recvbufsize);
	InitNetEnv();
	m_bIsFirstRead = true;
	m_nSSRC = 0;
	m_bIsLogin = false;
	m_szLogin[0] = '\0';
	parseURL(szStreamURL);
	m_nMaxKeepAliveWaitTime = 30;
	m_lastGetLiveTime = 0;
	m_bIsReHello = false;
	sprintf(m_szLogin,"Welcome@%s",m_szStreamID);
	
}

void CUDPClinet::parseURL(const char* szStreamURL)
{
	int nLen =  strlen("udp://");
	char* p = (char*)szStreamURL + nLen;
	char szPort[16] = {'\0'};
	char szSSCD[32] = {'\0'};
	char szUserID[32] = {'\0'};
	int i = 0;
	while(*p != '\0')
	{
		if(*p != ':')
		{
			m_szDestIP[i++] = *p++;
		}
		else
		{
			m_szDestIP[i++] = '\0';
			p++;
			break;
		}
	}

	i = 0;
	while(*p != '\0')
	{
		if(*p != '\/')
		{
			szPort[i++] = *p++;
		}
		else
		{
			szPort[i++] = '\0';
			p++;
			break;
		}
	}
	m_nDestPort = atoi(szPort);

	while(*p != '\0')
	{
		if(*p++ == '\/')
			break;
	}

	i = 0;
	while(*p != '\0')
	{
		m_szStreamID[i++] = *p++;
	}
	m_szStreamID[i] = '\0';

	p = m_szStreamID;

	while(*p != '\0')
	{
		if(*p == '_')
		{
			p++;
			break;
		}
		else
		{
			p++;
		}
	}
	i = 0;
	while(*p != '\0')
	{
		if(*p == '_')
		{
			p++;
			break;
		}
		else
		{
			szUserID[i++] = *p;
			p++;
		}
	}

	szUserID[i] = '\0';
	m_nUserID = atoi(szUserID);

	i = 0;
	while(*p != '\0')
	{
		if(*p == '?')
		{
			p++;
			break;
		}
		else
		{
			szSSCD[i++] = *p;
			p++;
		}
	}
	szSSCD[i] = '\0';
	m_nSSRC = atoi(szSSCD);
}

CUDPClinet::~CUDPClinet()
{
	UnInitUDPClient();
}

int  CUDPClinet::Groupsock(unsigned short nPort)
{
	int reuseFlag = 1;
    unsigned int addr = INADDR_ANY;
	int newSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (newSocket < 0)
    {
		return 0;
    }

    if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR,(const char*)&reuseFlag, sizeof reuseFlag) < 0)
    {
         closesocket(newSocket);
         return 0;
    }

    MAKE_SOCKADDR_IN(sockaddr, addr, nPort);
    if(bind(newSocket, (struct sockaddr*)&sockaddr, sizeof sockaddr) != 0)
    {
        closesocket(newSocket);
        return 0;
    }
	return newSocket;
}

bool CUDPClinet::getSourcePort0(int socket, unsigned short& resultPortNum) 
{
  sockaddr_in test; test.sin_port = 0;
  int len = sizeof test;
  if (getsockname(socket, (struct sockaddr*)&test, &len) < 0) 
	  return false;

  resultPortNum = ntohs(test.sin_port);
  return true;
}

bool CUDPClinet::getSourcePort( int socket, unsigned short& port) 
{
  unsigned short portNum = 0;
  if (!getSourcePort0(socket, portNum) || portNum == 0) 
  {
      MAKE_SOCKADDR_IN(name, INADDR_ANY, 0);
      bind(socket, (struct sockaddr*)&name, sizeof name);
      if(!getSourcePort0(socket, portNum) || portNum == 0) 
	  {
		  return false;
      }
  }
  port = portNum;
  return true;
}


unsigned CUDPClinet::setBufferTo(int bufOptName, int socket, unsigned requestedSize) 
{
    int sizeSize = sizeof requestedSize;
    setsockopt(socket, SOL_SOCKET, bufOptName, (char*)&requestedSize, sizeSize);
    return getBufferSize(bufOptName, socket);
}

unsigned CUDPClinet::getBufferSize(int bufOptName,int socket) 
{
    unsigned curSize;
    int sizeSize = sizeof(curSize);
    if(getsockopt(socket, SOL_SOCKET, bufOptName,(char*)&curSize, &sizeSize) < 0)
    {
        return 0;
    }
    return curSize;
}

unsigned CUDPClinet::getSendBufferSize(int socket) 
{
    return getBufferSize(SO_SNDBUF, socket);
}

unsigned CUDPClinet::getReceiveBufferSize(int socket) 
{
    return getBufferSize(SO_RCVBUF, socket);
}

unsigned CUDPClinet::setSendBufferTo(int socket, unsigned requestedSize)
{
	return setBufferTo(SO_SNDBUF, socket, requestedSize);
}

unsigned CUDPClinet::setReceiveBufferTo(int socket, unsigned requestedSize)
{
	return setBufferTo(SO_RCVBUF, socket, requestedSize);
}

bool CUDPClinet::makeSocketNonBlocking(int sock)
{
	#if defined(__WIN32__) || defined(_WIN32) || defined(IMN_PIM)
	  unsigned long arg = 1;
	  return ioctlsocket(sock, FIONBIO, &arg) == 0;
	#else
	  int curFlags = fcntl(sock, F_GETFL, 0);
	  return fcntl(sock, F_SETFL, curFlags|O_NONBLOCK) >= 0;
	#endif
}

bool CUDPClinet::makeSocketBlocking(int sock) 
{
	#if defined(__WIN32__) || defined(_WIN32) || defined(IMN_PIM)
	  unsigned long arg = 0;
	  return ioctlsocket(sock, FIONBIO, &arg) == 0;
	#else
	  int curFlags = fcntl(sock, F_GETFL, 0);
	  return fcntl(sock, F_SETFL, curFlags&(~O_NONBLOCK)) >= 0;
	#endif
}

bool CUDPClinet::InitUDPClient(int nType)
{
	m_nType = nType;
	m_lastGetLiveTime = 0;
	int nCount = 0;
	do
	{
		if(m_sendSocket > 0)
		{
			closesocket(m_sendSocket);
			m_sendSocket = -1;
		}
		if(m_recvSocket > 0)
		{
			closesocket(m_recvSocket);
			m_recvSocket = -1;
		}

		unsigned short nSendPort = 0;
		unsigned short nRecvPort = 0;
		struct sockaddr_in test;
		int len = sizeof(test);
		do
		{
			nSendPort = 0;
			if((m_sendSocket = Groupsock(nSendPort)) <= 0)
				continue;

			if(!getSourcePort(m_sendSocket,nSendPort))
			{
				continue;
			}

		}while((nSendPort&1) != 0 );

		nRecvPort = nSendPort|1;
		if((m_recvSocket = Groupsock(htons(nRecvPort))) > 0)
			break;

	}while(nCount++ < 10);

	if(m_sendSocket <= 0 || m_recvSocket<= 0)
		return false;
//	if(getsockname(m_recvSocket, (struct sockaddr*)&test, &len) < 0) 
	//	return false;

	setSendBufferTo(m_sendSocket,256*1024);
	setSendBufferTo(m_recvSocket,256*1024);
	setReceiveBufferTo(m_sendSocket,256*1024);
	setReceiveBufferTo(m_recvSocket,256*1024);
	makeSocketNonBlocking(m_sendSocket);
	makeSocketNonBlocking(m_recvSocket);

	m_OutBufPacket.setSSRC(m_nSSRC);
	m_recvThread.Begin(recv_media_loop,this);
	unsigned char szHelloBuf[256] = {0};
	strcpy(m_UDPBegHeader.szBuf,"HELLO UDP SERVER@");
	strcat(m_UDPBegHeader.szBuf,m_szStreamID);
	m_UDPBegHeader.nUserID = m_nUserID;
	if(m_nType == UDPPULL)
	{
		m_UDPBegHeader.nType = PULLAUDIO;
	}
	else
	{
		m_UDPBegHeader.nType = PUSHAUDIO;
	}
	memcpy(szHelloBuf,&m_UDPBegHeader,sizeof(m_UDPBegHeader));
	COutBuffer* pOutSendBuf = m_OutBufPacket.buildPacket((unsigned char*)szHelloBuf,sizeof(m_UDPBegHeader),2,0,true);
	unsigned int nIP = inet_addr(m_szDestIP);
	if(m_sendSocket > 0)
	{

		MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort));
		int addlen = sizeof(dest);
		int nSize = sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
		int len = nSize;
	}

	if(m_recvSocket>0)
	{
		MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort+1));
		int addlen = sizeof(dest);
		int nSize = sendto(m_recvSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
	}
	m_OutBufPacket.PutFreeOutBuffer(pOutSendBuf);
	return true;
}

bool CUDPClinet::UnInitUDPClient()
{
	if(m_sendSocket > 0)
	{
		unsigned char szHelloBuf[256] = {0};
		strcpy(m_UDPEndHeader.szBuf,"HELLO UDP SERVER@");
		strcat(m_UDPEndHeader.szBuf,m_szStreamID);
		m_UDPEndHeader.nUserID = m_nUserID;
		if(m_nType == UDPPULL)
		{
			m_UDPEndHeader.nType = ENDPULL;
		}
		else
		{
			m_UDPEndHeader.nType = ENDPUSH;
		}
		memcpy(szHelloBuf,&m_UDPEndHeader,sizeof(m_UDPEndHeader));
		COutBuffer* pOutSendBuf = m_OutBufPacket.buildPacket((unsigned char*)szHelloBuf,sizeof(m_UDPEndHeader),2,0,true);
		unsigned int nIP = inet_addr(m_szDestIP);
		if(m_sendSocket > 0)
		{

			MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort));
			int addlen = sizeof(dest);
			int nSize = sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
			sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
			int len = nSize;
		}
		m_OutBufPacket.PutFreeOutBuffer(pOutSendBuf);

		closesocket(m_sendSocket);
		m_sendSocket = -1;
	}
	if(m_recvSocket)
	{
		closesocket(m_recvSocket);
		m_recvSocket = -1;
	}
	m_recvThread.End();

	return false;
}


bool CUDPClinet::sendData(unsigned char* pBuf,unsigned int nBufLen,unsigned int nTimestamp)
{
	if(!m_bIsLogin)
	{
		unsigned char szHelloBuf[256] = {0};
		strcpy(m_UDPBegHeader.szBuf,"HELLO UDP SERVER@");
		strcat(m_UDPBegHeader.szBuf,m_szStreamID);
		m_UDPBegHeader.nUserID = m_nUserID;
		if(m_nType == UDPPULL)
		{
			m_UDPBegHeader.nType = PULLAUDIO;
		}
		else
		{
			m_UDPBegHeader.nType = PUSHAUDIO;
		}
		memcpy(szHelloBuf,&m_UDPBegHeader,sizeof(m_UDPBegHeader));
		COutBuffer* pOutSendBuf = m_OutBufPacket.buildPacket((unsigned char*)szHelloBuf,sizeof(m_UDPBegHeader),2,0,true);
		unsigned int nIP = inet_addr(m_szDestIP);
		if(m_bIsReHello)
		{
			if(m_sendSocket > 0)
			{

				MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort));
				int addlen = sizeof(dest);
				int nSize = sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
				sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
				int len = nSize;
			}
			m_bIsReHello = false;
		}
		if(m_recvSocket>0)
		{
			MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort+1));
			int addlen = sizeof(dest);
			int nSize = sendto(m_recvSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
			int n = nSize;
		}
		m_OutBufPacket.PutFreeOutBuffer(pOutSendBuf);
		return false;
	}
	else
		return buildMediaAndSendPacket(pBuf,nBufLen,0,nTimestamp,m_sendSocket);
}

int  CUDPClinet::recvData(unsigned char* pBuf,unsigned int nBufLen)
{
	if(!m_bIsLogin)
	{
		unsigned char szHelloBuf[256] = {0};
		strcpy(m_UDPBegHeader.szBuf,"HELLO UDP SERVER@");
		strcat(m_UDPBegHeader.szBuf,m_szStreamID);
		m_UDPBegHeader.nUserID = m_nUserID;
		if(m_nType == UDPPULL)
		{
			m_UDPBegHeader.nType = PULLAUDIO;
		}
		else
		{
			m_UDPBegHeader.nType = PUSHAUDIO;
		}
		memcpy(szHelloBuf,&m_UDPBegHeader,sizeof(m_UDPBegHeader));
		COutBuffer* pOutSendBuf = m_OutBufPacket.buildPacket((unsigned char*)szHelloBuf,sizeof(m_UDPBegHeader),2,0,true);
		unsigned int nIP = inet_addr(m_szDestIP);
		if(m_sendSocket > 0)
		{

			MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort));
			int addlen = sizeof(dest);
			int nSize = sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
			int len = nSize;
		}
		m_OutBufPacket.PutFreeOutBuffer(pOutSendBuf);
		return 0;
	}
	else
		return parsePacketAndRead(pBuf,nBufLen);
}

bool CUDPClinet::buildMediaAndSendPacket(unsigned char* pBuf,unsigned int nBufLen,unsigned char nRTPPayloadType,unsigned int nTimestamp,int nSocket)
{
	COutBuffer* pOutSendBuf = m_OutBufPacket.buildPacket(pBuf,nBufLen,nRTPPayloadType,nTimestamp,false);
	unsigned rtpHdr = ntohl(*(unsigned int*)(pOutSendBuf->startPtr()));
	unsigned short rtpSeqNo = (unsigned short)(rtpHdr&0xFFFF);
	if(pOutSendBuf)
	{
		if(nSocket > 0)
		{
			unsigned int nIP = inet_addr(m_szDestIP);
			MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort));
			int addlen = sizeof(dest);
			int nSize = sendto(nSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
						sendto(nSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
			int len = nSize;
		}
		m_OutBufPacket.PutBusyPacket(pOutSendBuf);
		return true;
	}
	return false;
}

int CUDPClinet::parsePacketAndRead(unsigned char* pBuf,unsigned int nBufLen)
{
	DWORD nCurTimePts = GetTickCount();
	unsigned int nDiff = nCurTimePts - m_readLastPacketTime;
	CInBuffer* pInBufer = m_InBufPacket.GetBusyPacket(m_readLastPacketTime,nDiff);
	if(pInBufer)
	{
		unsigned char nPayloadType = 0;
		unsigned int  nPayloadSize = 0;
		unsigned char* PayloadData = NULL;
		unsigned int nReadSize = 0;
		m_readLastPacketTime = nCurTimePts;
		m_SedLosePacketTime = 0;
		m_InBufPacket.parsePacket(pInBufer->startPtr(),pInBufer->getPacketSize(),&PayloadData,nPayloadSize,nPayloadType);
		if(nPayloadType == 0 && PayloadData) //audio
		{
			if(m_bIsFirstRead)
			{
				 memcpy(pBuf, flvHeader, sizeof(flvHeader));
				 nReadSize += sizeof(flvHeader);
				 m_bIsFirstRead = false;
			}
			unsigned int nFreeSize = nBufLen-nReadSize;
			m_FlvFile.writeMediaDataTagToMem(pBuf+nReadSize,nFreeSize,PayloadData,nPayloadSize,pInBufer->getTimePts(),false);
			nReadSize += nFreeSize;
		}
		m_InBufPacket.PutFreePacket(pInBufer);
		return nReadSize;
	}
	else
	{
		int nWaitTime = 0;
		if(m_SedLosePacketTime == 0)
		{
			nWaitTime = 300;
			m_SedLosePacketTime = m_readLastPacketTime;
		}
		else
		{
			nDiff = nCurTimePts - m_SedLosePacketTime;
			nWaitTime = 100;
		}
		if(nDiff > nWaitTime && m_InBufPacket.getSSRC() != 0) //send retransmission msg
		{
			m_SedLosePacketTime = nCurTimePts;

			char szMsg[256] = {'\0'};
			sprintf(szMsg,"%d|%d",m_InBufPacket.getSSRC(),m_InBufPacket.getCurReadSeqNo());		
			COutBuffer* pOutSendBuf = m_OutBufPacket.buildPacket((unsigned char*)szMsg,strlen(szMsg)+1,1,0,true);
			unsigned int nIP = inet_addr(m_szDestIP);
			if(m_recvSocket>0)
			{
				MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort+1));
				int addlen = sizeof(dest);
				int nSize = sendto(m_recvSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
			}
			m_OutBufPacket.PutFreeOutBuffer(pOutSendBuf);
		}
	}
	return 0;
}

void   CUDPClinet::parseLoseMsg(unsigned char* buf,unsigned int bufsize,int arrLose[],int&nCount)
{
	int n = 0;
	if(buf != NULL || bufsize > 1)
	{
		char *p = (char*)buf;
		char *q = p;
		unsigned int i = 0;
		while(*p != '\0' && i < bufsize)
		{
			if(*p == '|')
			{
				arrLose[n++] = atoi(q);
				p++;
				q=p;
				if(nCount <= n-2)
					break;
			}
			else
			{
				p++;
			}
		}
		arrLose[n++] = atoi(q);
	}
	nCount = n;
}

void CUDPClinet::recv_loop()
{
	while(!m_recvThread.GetStop())
	{
		if(m_sendSocket && m_nType == UDPPULL)
		{
			struct sockaddr_in client_addr;
			int cli_len=sizeof(client_addr);
			int nStreamSize  = recvfrom(m_sendSocket,(char*)m_recvbuf,m_recvbufsize,0,(struct sockaddr*)&client_addr,&cli_len);
			if(nStreamSize > 0)
			{
				unsigned char nPayloadFormat = m_InBufPacket.getRevcDataPayloadFormat(m_recvbuf);
				if(nPayloadFormat == 0)
				{
					if(m_InBufPacket.getLastReavPacketSeqNo() != m_InBufPacket.getRecvDataSeqNo(m_recvbuf))
						m_InBufPacket.putBufToInPacket(m_recvbuf,nStreamSize);
				}
				else if(nPayloadFormat == 2)
				{
					unsigned char* szTempMsgBuf = NULL;
					unsigned int  nTempMsgBuf = 0;
					int arrNum[128] = {0};
					int nSize = 128;
					m_InBufPacket.parsePacket(m_recvbuf,nSize,&szTempMsgBuf,nTempMsgBuf,nPayloadFormat);
					if(strncmp((char*)szTempMsgBuf,m_szLogin,strlen(m_szLogin)) == 0)
					{
						m_bIsLogin = true;
					}
				}
			}
		}
		if(m_recvSocket > 0 && m_nType == UDPPUSH)
		{
			struct sockaddr_in client_addr;
			int cli_len=sizeof(client_addr);

			DWORD nCurSysTime = GetTickCount();
			if(m_bIsLogin)
			{
				int nDiff = (nCurSysTime  - m_lastGetLiveTime)/1000;
				if(nDiff >= m_nMaxKeepAliveWaitTime) //超时最大心跳时间认为网络异常 需要从新打招呼
				{
					m_bIsLogin = false;
					m_bIsReHello = true;
					m_lastGetLiveTime = nCurSysTime;
				}
			}

			int nSize  = recvfrom(m_recvSocket,(char*)m_recvbuf,m_recvbufsize,0,(struct sockaddr*)&client_addr,&cli_len);
			if(nSize > 0)
			{

			   m_lastGetLiveTime = nCurSysTime;
			   unsigned char nPayloadFormat = m_InBufPacket.getRevcDataPayloadFormat(m_recvbuf);
			   if(nPayloadFormat == 1)
				{
					unsigned char* szTempMsgBuf = NULL;
					unsigned int  nTempMsgBuf = 0;
					int arrNum[128] = {0};
					int nSize = 128;
					m_InBufPacket.parsePacket(m_recvbuf,nSize,&szTempMsgBuf,nTempMsgBuf,nPayloadFormat);
					if(szTempMsgBuf)
					{
						printf("recv msg %s\n",szTempMsgBuf);
						parseLoseMsg(szTempMsgBuf,nTempMsgBuf,arrNum,nSize);
						if(nSize > 1 && m_sendSocket > 0)
						{
							if(arrNum[0] == m_nSSRC)
							{
								for(int i = 1; i< nSize ;i++)
								{
									COutBuffer* pOutSendBuf = m_OutBufPacket.getBusyOutBufferByNO(arrNum[i]);
									if(pOutSendBuf)
									{
										unsigned int nIP = inet_addr(m_szDestIP);
										MAKE_SOCKADDR_IN(dest,nIP,htons(m_nDestPort));
										int addlen = sizeof(dest);
										int nSize = sendto(m_sendSocket,(char*)pOutSendBuf->startPtr(),pOutSendBuf->curPacketSize(),0,(const sockaddr*)&dest,addlen);
										m_OutBufPacket.PutBusyPacket(pOutSendBuf);
									}
								}
							}
						}
					}
				}
				else if(nPayloadFormat == 2)
				{
					unsigned char* szTempMsgBuf = NULL;
					unsigned int  nTempMsgBuf = 0;
					int arrNum[128] = {0};
					int nSize = 128;
					m_InBufPacket.parsePacket(m_recvbuf,nSize,&szTempMsgBuf,nTempMsgBuf,nPayloadFormat);
					if(strncmp((char*)szTempMsgBuf,m_szLogin,strlen(m_szLogin)) == 0)
					{
						m_bIsLogin = true;
					}
			   }
			   else if(nPayloadFormat == 3)
			   {
				   	unsigned char* szTempMsgBuf = NULL;
					unsigned int  nTempMsgBuf = 0;
					int arrNum[128] = {0};
					int nSize = 128;
					m_InBufPacket.parsePacket(m_recvbuf,nSize,&szTempMsgBuf,nTempMsgBuf,nPayloadFormat);
				}
			}
			m_OutBufPacket.RemovePacket();
		}
		Sleep(10);
	}
}
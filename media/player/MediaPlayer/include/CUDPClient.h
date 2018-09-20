#ifndef CUDPCLIENT_H
#define CUDPCLIENT_H
#include "OutBufPacket.h"
#include "InBufPacket.h"
#include "sc_Thread.h"
#include "flv_file.h"

#define UDPPUSH 1
#define UDPPULL 2

struct UDPMsgHeader
{
	short nType;
	int   nUserID;
	char szBuf[200];
	UDPMsgHeader()
	{
		nType = 0;
		nUserID = 0;
		szBuf[0] = '\0';
	}
};

class CUDPClinet
{
public:
	CUDPClinet(unsigned int nDestIP,unsigned short nPDestPort,const char* szStreamID);
	CUDPClinet(const char* szDestIP,unsigned short nPDestPort,const char* szStreamID);
	CUDPClinet(const char* szStreamURL);
	~CUDPClinet();
public:
	bool sendData(unsigned char* pBuf,unsigned int nBufLen,unsigned int nTimestamp);
	int  recvData(unsigned char* pBuf,unsigned int nBufLen);

	bool InitUDPClient(long nUserID,int nType);
	bool UnInitUDPClient();

	void recv_loop();
private:
	bool InitNetEnv();
	void parseURL(const char* szStreamURL);
private:
	int  Groupsock(unsigned short nPort);
	bool getSourcePort0(int socket, unsigned short& resultPortNum) ;
	bool getSourcePort( int socket, unsigned short& port);
	unsigned setBufferTo(int bufOptName, int socket, unsigned requestedSize) ;
	unsigned getBufferSize(int bufOptName,int socket) ;
	unsigned getSendBufferSize(int socket);
	unsigned getReceiveBufferSize(int socket);
    unsigned setSendBufferTo(int socket, unsigned requestedSize);
	unsigned setReceiveBufferTo(int socket, unsigned requestedSize);

	bool makeSocketNonBlocking(int sock);
	bool makeSocketBlocking(int sock) ;

private:
	bool buildMediaAndSendPacket(unsigned char* pBuf,unsigned int nBufLen,unsigned char nRTPPayloadType,unsigned int nTimestamp,int nSocket);
	int parsePacketAndRead(unsigned char* pBuf,unsigned int nBufLen);
	void  parseLoseMsg(unsigned char* buf,unsigned int bufsize,int arrLose[],int&nCount);
private:
	unsigned int    m_nDestIP;
	char            m_szDestIP[64];
	unsigned short  m_nDestPort;
	char            m_szStreamID[256];
	unsigned char   m_nRTPPayloadType;
	unsigned int    m_readLastPacketTime;
	unsigned int    m_statTime;
	bool            m_bIsFirstRead;
	unsigned        m_nSSRC;
	bool            m_bIsLogin;
	char            m_szLogin[128];
private:
	int             m_sendSocket;
	int             m_recvSocket;
	COutBufPacket   m_OutBufPacket;
	CInBufPacket    m_InBufPacket;

private:
	CSCThread       m_recvThread;
	unsigned char*  m_recvbuf;
	unsigned int    m_recvbufsize;

	UDPMsgHeader    m_UDPBegHeader;
	UDPMsgHeader    m_UDPEndHeader;
private:
	CFlvFile        m_FlvFile;
	int             m_nType;
	long            m_nUserID;
private:
	int             m_nMaxKeepAliveWaitTime;
	unsigned long   m_lastGetLiveTime;
	bool            m_bIsReHello;
};
#endif
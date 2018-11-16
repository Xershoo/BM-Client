#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>  
#include <WS2tcpip.h>
#include "Ping.h"
#include <process.h>

#pragma comment(lib,"ws2_32.lib")

#define ICMP_ECHO_REQUEST			(8)			//定义回显请求类型
#define DEF_ICMP_DATA_SIZE			(20)		//定义发送数据长度
#define DEF_ICMP_PACK_SIZE			(32)		//定义数据包长度
#define MAX_ICMP_PACKET_SIZE		(1024)		//定义最大数据包长度
#define ICMP_TIMEOUT				(11)		//ICMP超时报文
#define ICMP_ECHO_REPLY				(0)			//定义回显应答类型

static void PrintTrace(char* format,...)
{
#ifndef DEBUG

#else
	char szText[1024]={0};
	va_list args;
	va_start(args, format);
	vsprintf_s(szText, sizeof(szText)-1, format, args);
	va_end(args);

	OutputDebugStringA(szText);
	OutputDebugStringA("\n");
#endif
}

//////////////////////////////////////////////////////////////////////////
CPing::CPing():m_sockRaw(NULL),m_icmpData(NULL),m_recvBuf(NULL)
	,m_timeout(DEF_ICMP_TIMEOUT)
	,m_sendPacket(0)
	,m_recvPacket(0)
	,m_lossPacket(0)
	,m_curDelay(0)
	,m_allDelay(0)
	,m_totalTimes(0)
	,m_error(0)
{
	memset(m_pingHost,NULL,sizeof(m_pingHost));
	initialize();
}

CPing::~CPing()
{
	release();
}

void CPing::initialize()
{
	WSADATA wsaData;  
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)  
	{	  
		m_error = PING_INIT_FAILED;
		return;  
	}

	m_icmpData = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, DEF_ICMP_PACK_SIZE);
	if(m_icmpData)
	{
		memset(m_icmpData,0,DEF_ICMP_PACK_SIZE);		
		fillICMPData(m_icmpData, DEF_ICMP_PACK_SIZE);
	}

	m_recvBuf = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, MAX_ICMP_PACKET_SIZE);
	if(m_recvBuf)
	{
		memset(m_recvBuf,0,MAX_ICMP_PACKET_SIZE);
	}
}

void CPing::release()
{
	if(NULL!=m_recvBuf)
	{
		HeapFree(GetProcessHeap(), 0, m_recvBuf);
		m_recvBuf = NULL;
	}

	if(NULL!=m_icmpData)
	{
		HeapFree(GetProcessHeap(), 0, m_icmpData);
		m_icmpData = NULL;
	}

	if(NULL!=m_sockRaw)
	{
		closesocket(m_sockRaw);
		m_sockRaw = NULL;
	}

	WSACleanup();
}

bool CPing::ping(const char* host,unsigned int timeout)
{
	if(NULL==host||NULL==host[0])
	{
		return false;
	}

	unsigned long destIP = inet_addr(host);
	if(destIP == INADDR_NONE)
	{
		HOSTENT *pHostent = gethostbyname(host) ;
		if(pHostent)
		{
			destIP = (*(IN_ADDR*)pHostent->h_addr).s_addr ;
		}
		else
		{
			PrintTrace("parse ip address failed!") ;
			return false;
		}
	}

	if(!setSocket())
	{
		PrintTrace("initialize socket failed!") ;
		return false;
	}
	
	strcpy_s(m_pingHost,host);

	m_error = PING_OK;
	return doPing(destIP);
}

void CPing::getStatus(int& delayTime,int& averDelayTime,int& lossRate,int& lossPacket,DWORD& err)
{
	delayTime = m_curDelay;

	if(m_totalTimes > 0 )
	{
		averDelayTime = m_allDelay / m_totalTimes;
	}
	else
	{
		averDelayTime = 0;
	}

	if(m_sendPacket > 0)
	{
		lossRate = (int)(m_sendPacket-m_recvPacket)*100/m_sendPacket;
	}
	else
	{
		lossRate = 0;
	}

	lossPacket = m_lossPacket;
	err = m_error;
}

void CPing::fillICMPData(char* icmpData, int dataSize)
{
	if(NULL==icmpData||dataSize <= sizeof(ICMP_HEADER))
	{
		return;
	}

	ICMP_HEADER* icmp_hdr = (ICMP_HEADER*)icmpData;  
	
	icmp_hdr->type = ICMP_ECHO_REQUEST;		//Request an ICMP echo  
	icmp_hdr->code = 0;  
	icmp_hdr->id = (USHORT)GetCurrentProcessId();  
	icmp_hdr->cksum = 0;  
	icmp_hdr->seq = 0;  

	char* dataPart = icmpData + sizeof(ICMP_HEADER);  

	//Place some junk in the buffer  
	memset(dataPart, 'E', DEF_ICMP_DATA_SIZE);
}

bool CPing::decodeICMPHeader(char* buffer, int bytes, DECODE_RESULT * retDecode)
{
	IP_HEADER *pIpHrd = (IP_HEADER*)buffer ;
	int iIphedLen = 20 ;
	if(bytes < (int)(iIphedLen + sizeof(ICMP_HEADER)))
	{
		PrintTrace("size error!") ;
		return false ;
	}

	ICMP_HEADER *pIcmpHrd = (ICMP_HEADER*)(buffer + iIphedLen) ;
	unsigned short pid = 0;
	unsigned short seq = 0;
	
	if(pIcmpHrd->type == ICMP_ECHO_REPLY)
	{
		pid = pIcmpHrd->id ;
		seq = ntohs(pIcmpHrd->seq) ;
	}

	if(pid != GetCurrentProcessId() || seq != retDecode->seq)
	{
		PrintTrace("pid or seq error!") ;
		return 0 ;
	}

	if(pIcmpHrd->type != ICMP_ECHO_REPLY)
	{
		PrintTrace("recv type error!");
		return false;
	}
	
	retDecode->addr.s_addr = pIpHrd->sourceIP ;
	retDecode->ttl = pIpHrd->ttl ;
	retDecode->time = GetTickCount() - retDecode->time ;

	m_curDelay = retDecode->time;		
	m_allDelay += m_curDelay;

	return true;  
}

USHORT CPing::checkSum(USHORT* buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)  
	{  
		cksum += *buffer++;  
		size -= sizeof(USHORT);  
	}  

	if (size)  
	{  
		cksum += *(UCHAR*)buffer;  
	}  

	cksum = (cksum>>16) + (cksum & 0xffff);  
	cksum += (cksum>>16);  

	return (USHORT)(~cksum); 
}

bool CPing::doPing(ULONG destIP)
{
	struct sockaddr_in destAddr={0}; 
	struct sockaddr_in fromAddr={0};

	int	seq = 0 ;
	unsigned int err = 0;
	DECODE_RESULT ret = {0} ;

	int fromAddrLen = sizeof(fromAddr);
	
	memset(&destAddr, 0, sizeof(SOCKADDR_IN));
	destAddr.sin_family = AF_INET;  
	destAddr.sin_addr.s_addr = destIP;
	destAddr.sin_port = htons(0);

	m_error = (long)PING_OK;

	//填充ICMP数据包个各字段
	ICMP_HEADER *pIcmpHeader  = (ICMP_HEADER*)m_icmpData;
	bool br = false;

	while( seq <= 5)
	{
		pIcmpHeader->seq = htons(seq) ;
		pIcmpHeader->cksum = 0;
		pIcmpHeader->cksum = checkSum((unsigned short*)m_icmpData , DEF_ICMP_PACK_SIZE) ; //生成校验位

		ret.seq = seq ;
		ret.time = GetTickCount() ;

		if(sendto(m_sockRaw , m_icmpData , DEF_ICMP_PACK_SIZE , 0 , (SOCKADDR*)&destAddr , sizeof(destAddr)) == SOCKET_ERROR)
		{
			//如果目的主机不可达则直接退出
			if(WSAGetLastError() == WSAEHOSTUNREACH)
			{
				PrintTrace("Target host is not reachable!") ;
				err = SOCKET_ERROR;

				break;
			}
			else
			{
				err ++;
				seq ++ ;

				Sleep(1000);
				continue;
			}
		}
		
		memset(m_recvBuf,0,MAX_ICMP_PACKET_SIZE);

		int recv_ret = recvfrom(m_sockRaw , m_recvBuf , MAX_ICMP_PACKET_SIZE , 0 , (SOCKADDR*)&fromAddr , &fromAddrLen) ;
		if(recv_ret != SOCKET_ERROR)
		{
			if(decodeICMPHeader(m_recvBuf , recv_ret , &ret))
			{
				PrintTrace("receive %s reply: length = %d time = %dms TTL = %d\n" , inet_ntoa(ret.addr),recv_ret - 20,ret.time ,ret.ttl) ;
				br = true;
				break;
			}
			else
			{
				err++;
			}
		}
		else if(WSAGetLastError() == WSAETIMEDOUT)
		{
			PrintTrace("receive time out...") ;
			err++;
		}
		else
		{
			PrintTrace("receive error....") ;
			err=SOCKET_ERROR;
			break;
		}

		/*
		if(err > 2){
			break;
		}

		if(seq>=1&&err==0){
			break;
		}
		*/

		seq++ ;
		Sleep(1000);
	}

	closesocket(m_sockRaw);
	m_sockRaw = NULL;
	return br;//err<=2 ? true:false;
}

bool CPing::setSocket()
{
	if(NULL==m_recvBuf || NULL == m_icmpData)
	if(NULL!=m_sockRaw)
	{
		closesocket(m_sockRaw);
		m_sockRaw = NULL;
	}

	m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP,NULL,0, WSA_FLAG_OVERLAPPED);
	if (m_sockRaw == INVALID_SOCKET)
	{
		m_error = PING_NET_ERROR;
		m_sockRaw = NULL;

		return false;  
	}

	setsockopt(m_sockRaw, SOL_SOCKET, SO_RCVTIMEO,(char*)&m_timeout, sizeof(m_timeout));
	setsockopt(m_sockRaw, SOL_SOCKET, SO_SNDTIMEO,(char*)&m_timeout, sizeof(m_timeout));

	if(m_icmpData)
	{
		memset(m_icmpData,0,DEF_ICMP_PACK_SIZE);		
		fillICMPData(m_icmpData, DEF_ICMP_PACK_SIZE);
	}

	if(m_recvBuf)
	{
		memset(m_recvBuf,0,MAX_ICMP_PACKET_SIZE);
	}

	return true;
}
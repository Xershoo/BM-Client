#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>  
#include <WS2tcpip.h>
#include "Ping.h"
#include <process.h>

#pragma comment(lib,"ws2_32.lib")

#define ICMP_ECHO			(8)  
#define ICMP_ECHOREPLY		(0)  
#define ICMP_MIN			(8)				//Minimum 8-byte ICMP packet (header)  

#define MAX_ICMP_PACKET_SIZE	(1024)
#define ICMP_PACKET_SIZE		(128)
#define MAX_IP_HDR_SIZE			(60) 
#define DEF_TIME_OUT			(1000)

//////////////////////////////////////////////////////////////////////////
CPing::CPing():m_sockRaw(NULL),m_icmpData(NULL),m_recvBuf(NULL)
	,m_timeout(DEF_TIME_OUT)
	,m_sendPacket(0)
	,m_recvPacket(0)
	,m_lossPacket(0)
	,m_curDelay(0)
	,m_allDelay(0)
	,m_totalTimes(0)
	,m_icmpSeq(0)
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

	m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP,NULL,0, WSA_FLAG_OVERLAPPED);
	if (m_sockRaw == INVALID_SOCKET)
	{
		m_error = PING_INIT_FAILED;
		m_sockRaw = NULL;
		return;  
	}

	setsockopt(m_sockRaw, SOL_SOCKET, SO_RCVTIMEO,(char*)&m_timeout, sizeof(m_timeout));
	setsockopt(m_sockRaw, SOL_SOCKET, SO_SNDTIMEO,(char*)&m_timeout, sizeof(m_timeout));

	m_icmpData = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, ICMP_PACKET_SIZE);
	if(m_icmpData)
	{
		memset(m_icmpData,0,ICMP_PACKET_SIZE);		
		fillICMPData(m_icmpData, ICMP_PACKET_SIZE);
	}

	m_recvBuf = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, MAX_ICMP_PACKET_SIZE);
	if(m_recvBuf)
	{
		memset(m_recvBuf,0,MAX_ICMP_PACKET_SIZE);
	}
}

void CPing::release()
{
	if(m_recvBuf)
	{
		HeapFree(GetProcessHeap(), 0, m_recvBuf);
		m_recvBuf = NULL;
	}

	if(m_icmpData)
	{
		HeapFree(GetProcessHeap(), 0, m_icmpData);
		m_icmpData = NULL;
	}

	if(m_sockRaw)
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

	if(inet_addr(host) == INADDR_NONE)
	{
		struct hostent* host_p = gethostbyname(host);
		if(NULL == host_p)
		{
			m_error = PING_UNKNOW_HOST;
			return false;
		}
	}
	
	if(INVALID_SOCKET==m_sockRaw||NULL==m_sockRaw)
	{
		return false;
	}

	if(NULL==m_recvBuf||NULL==m_icmpData)
	{
		return false;
	}

	if(m_timeout != timeout)
	{
		setsockopt(m_sockRaw, SOL_SOCKET, SO_RCVTIMEO,(char*)&m_timeout, sizeof(m_timeout));
		setsockopt(m_sockRaw, SOL_SOCKET, SO_SNDTIMEO,(char*)&m_timeout, sizeof(m_timeout));

		m_timeout = timeout;
	}

	strcpy_s(m_pingHost,host);
	m_error = PING_OK;
	m_icmpSeq = 0;

	return doPing();
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
	if(NULL==icmpData||dataSize <= sizeof(IcmpHeader))
	{
		return;
	}

	IcmpHeader* icmp_hdr = (IcmpHeader*)icmpData;  
	
	icmp_hdr->i_type = ICMP_ECHO;		//Request an ICMP echo  
	icmp_hdr->i_code = 0;  
	icmp_hdr->i_id = (USHORT)GetCurrentProcessId();  
	icmp_hdr->i_cksum = 0;  
	icmp_hdr->i_seq = 0;  

	char* dataPart = icmpData + sizeof(IcmpHeader);  

	//Place some junk in the buffer  
	memset(dataPart, 'E', dataSize - sizeof(IcmpHeader));
}

void CPing::decodeIPOptions(char* buffer, int bytes)
{
	IpOptionHeader* ipopt = (IpOptionHeader*)(buffer + 20);  
	
	for (int i = 0; i < (ipopt->ptr / 4) - 1; i++)  
	{  
		IN_ADDR inaddr;  
		inaddr.S_un.S_addr = ipopt->addr[i];  

		if (i != 0)  
		{  
			printf("        "); 
		}  

		HOSTENT* host = gethostbyaddr((char*)&inaddr.S_un.S_addr,sizeof(inaddr.S_un.S_addr), AF_INET);  
		if (host)
		{  
			printf("(%-15s) %s\n", inet_ntoa(inaddr), host->h_name);  
		}  
		else  
		{  
			printf("(%-15s)\n", inet_ntoa(inaddr));  
		}  
	}

	return;  
}

bool CPing::decodeICMPHeader(char* buffer, int bytes)
{
	IpHeader* iphdr = (IpHeader*)buffer;  

	//Number of 32-bit words * 4 = bytes  
	unsigned short iphdrlen = iphdr->h_len * 4;  
	DWORD tick = GetTickCount();  

	if ((iphdrlen == MAX_IP_HDR_SIZE) && (0==m_recvPacket))  
	{  
		decodeIPOptions(buffer, bytes);  
	}  

	if (bytes < iphdrlen + ICMP_MIN)  
	{  
		struct in_addr addrSource;
		addrSource.S_un.S_addr = iphdr->sourceIP;
		printf("Too few bytes from %s\n", inet_ntoa(addrSource));
		return false;
	}

	IcmpHeader* icmphdr = (IcmpHeader*)(buffer + iphdrlen);  
	if (icmphdr->i_type != ICMP_ECHOREPLY)  
	{  
		printf("nonecho type %d recvd\n", icmphdr->i_type);  
		return false;  
	}  

	//Make sure this is an ICMP reply to something we sent!  
	if (icmphdr->i_id != (USHORT)GetCurrentProcessId())  
	{  
		printf("someone else's packet!\n");  
		return false;  
	}  
	
	if (icmphdr->i_seq != (m_icmpSeq - 1))
	{
		resetSocket();
		return false;
	}

	m_curDelay = tick - icmphdr->timestamp;		
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

bool CPing::doPing()
{
	struct sockaddr_in destAddr; 
	struct sockaddr_in fromAddr;
	int fromAddrLen = sizeof(fromAddr);
	int tryTimes = 0;
	
	memset(&destAddr, 0, sizeof(destAddr));
	destAddr.sin_family = AF_INET;  
	destAddr.sin_addr.s_addr = inet_addr(m_pingHost);

	if(destAddr.sin_addr.s_addr == INADDR_NONE)
	{
		struct hostent* host_p = gethostbyname(m_pingHost);
		if(NULL==host_p)
		{
			m_error = PING_UNKNOW_HOST;
			return false;				
		}

		memcpy(&(destAddr.sin_addr), host_p->h_addr, host_p->h_length);  
		destAddr.sin_family = host_p->h_addrtype; 
	}

	m_error = (long)PING_OK;
	
	((IcmpHeader*)m_icmpData)->i_cksum = 0;  
	((IcmpHeader*)m_icmpData)->timestamp = GetTickCount();  
	((IcmpHeader*)m_icmpData)->i_seq = m_icmpSeq++;  
	((IcmpHeader*)m_icmpData)->i_cksum = checkSum((USHORT*)m_icmpData, ICMP_PACKET_SIZE);  

	bool ret = false;
	
	do 
	{
		int sock_ret = sendto(m_sockRaw, m_icmpData, ICMP_PACKET_SIZE, 0,(struct sockaddr*)&destAddr, sizeof(destAddr));  
		if (sock_ret == SOCKET_ERROR)  
		{
			if (WSAGetLastError() == WSAETIMEDOUT)  
			{ 
				m_error = PING_TIMEOUT;
			}
			else
			{
				resetSocket();
				m_error = (long)PING_NET_ERROR;
			}

			tryTimes ++;
			Sleep(200);
			continue;
		}  

		m_sendPacket++;
				
		if (sock_ret < ICMP_PACKET_SIZE)  
		{  
			printf("Wrote %d bytes\n", sock_ret);  
		}  

		sock_ret = recvfrom(m_sockRaw, m_recvBuf, MAX_ICMP_PACKET_SIZE, 0, (struct sockaddr*)&fromAddr, &fromAddrLen);  
		if (sock_ret == SOCKET_ERROR)  
		{
			if (WSAGetLastError() == WSAETIMEDOUT)  
			{
				m_lossPacket++;
				m_error = PING_TIMEOUT;				
			}
			else
			{
				m_error = PING_NET_ERROR;
			}
			
			tryTimes ++;
			Sleep(200);
			continue;			
		}  

		m_totalTimes ++;

		if(!decodeICMPHeader(m_recvBuf, sock_ret))
		{				
			m_error = PING_NET_ERROR; //PING_DATA_ERR;
			
			tryTimes ++;
			Sleep(200);
			continue;
		}
		
		m_recvPacket++;
		return true;
	}while(tryTimes < 3);

	return false;
}

void CPing::resetSocket()
{
	if(m_sockRaw)
	{
		closesocket(m_sockRaw);
		m_sockRaw = NULL;
	}

	m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP,NULL,0, WSA_FLAG_OVERLAPPED);
	if (m_sockRaw == INVALID_SOCKET)
	{
		m_error = PING_NET_ERROR;
		m_sockRaw = NULL;

		return;  
	}

	setsockopt(m_sockRaw, SOL_SOCKET, SO_RCVTIMEO,(char*)&m_timeout, sizeof(m_timeout));
	setsockopt(m_sockRaw, SOL_SOCKET, SO_SNDTIMEO,(char*)&m_timeout, sizeof(m_timeout));

	m_icmpSeq = 0;

	if(m_icmpData)
	{
		memset(m_icmpData,0,ICMP_PACKET_SIZE);		
		fillICMPData(m_icmpData, ICMP_PACKET_SIZE);
	}

	if(m_recvBuf)
	{
		memset(m_recvBuf,0,MAX_ICMP_PACKET_SIZE);
	}
}
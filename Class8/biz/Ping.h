#ifndef _PING_H
#define _PING_H

#include <Windows.h>

#define MAX_HOST_NUM	(5)

typedef unsigned int SOCKET;
//IP header structure  
typedef struct _iphdr  
{  
	unsigned int h_len:4;			//Length of the header  
	unsigned int version:4;			//Version of IP  
	unsigned char tos;				//Type of service  
	unsigned short total_len;		//Total length of the packet  
	unsigned short ident;			//Unique identifier  
	unsigned short frag_and_flags;	//Flags  
	unsigned char ttl;				//Time to live  
	unsigned char proto;			//Protocol (TCP,UDP,etc.)  
	unsigned short checksum;		//IP checksum  

	unsigned int sourceIP;  
	unsigned int destIP;  
} IpHeader;  

//ICMP header structure  
typedef struct _icmphdr  
{  
	BYTE i_type;  
	BYTE i_code;					//Type sub code  
	USHORT i_cksum;  
	USHORT i_id;  
	USHORT i_seq;  

	//This is not the standard header, but we reserve space for time  
	ULONG timestamp;  
} IcmpHeader;  

//IP option header--use with socket option IP_OPTIONS  
typedef struct _ipoptionhdr  
{  
	unsigned char code;			//Option type  
	unsigned char len;			//Length of option hdr  
	unsigned char ptr;			//Offset into optons  
	unsigned long addr[9];		//List of IP addrs  
} IpOptionHeader; 

class CPing
{
public:
	enum{
		PING_OK = 0,		
		PING_INIT_FAILED,
		PING_UNKNOW_HOST,
		PING_START_FAILED,
		PING_NET_ERROR,
		PING_TIMEOUT,
		PING_DATA_ERR,
		PING_STOP,
	};
public:
	CPing();
	~CPing();

	bool ping(const char* host,unsigned int timeout=1000);
	void getStatus(int& delayTime,int& averDelayTime,int& lossRate,int& lossPacket,DWORD& err);//网络延时(ms），平均延时(ms)，丢包率，丢包个数

protected:
	void	initialize();
	void	release();

	void	resetSocket();
	bool	doPing();
protected:
	void	fillICMPData(char* icmpData, int dataSize);
	void	decodeIPOptions(char* buffer, int bytes);
	bool	decodeICMPHeader(char* buffer, int bytes);

	USHORT	checkSum(USHORT* buffer, int size);

protected:
	SOCKET	m_sockRaw;
	char*	m_icmpData;
	char*	m_recvBuf;
	char	m_pingHost[128];

	unsigned int m_timeout;

	int	m_sendPacket;
	int	m_recvPacket;
	int	m_curDelay;
	int	m_allDelay;
	int m_totalTimes;
	int	m_lossPacket;

	DWORD	m_error;
	USHORT	m_icmpSeq;
};

#endif
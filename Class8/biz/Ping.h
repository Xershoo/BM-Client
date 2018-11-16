#ifndef _PING_H
#define _PING_H

#include <Windows.h>

#define MAX_HOST_NUM				(5)
#define DEF_ICMP_TIMEOUT			(5000)		//定义超时为5秒

typedef unsigned int SOCKET;

/*
 *IP报头结构
 */
typedef struct
{
    byte h_len_ver ;					//IP版本号
    byte tos ;							//服务类型
    unsigned short total_len ;			//IP包总长度
    unsigned short ident ;				//标识
    unsigned short frag_and_flags ;		//标志位
    byte ttl ;							//生存时间
    byte proto ;						//协议
    unsigned short cksum ;				//IP首部校验和
    unsigned long sourceIP ;			//源IP地址
    unsigned long destIP ;				//目的IP地址
} IP_HEADER ;

/*
 *定义ICMP数据类型
 */
typedef struct _ICMP_HEADER
{
    byte type ;							//类型-----8
    byte code ;							//代码-----8
    unsigned short cksum ;				//校验和------16
    unsigned short id ;					//标识符-------16
    unsigned short seq ;				//序列号------16
    unsigned int choose ;				//选项-------32
} ICMP_HEADER ;

typedef struct
{
	int			seq;				//记录序列号
	DWORD		time;				//记录当前时间
	byte		ttl;				//生存时间
	in_addr		addr ;				//源IP地址
} DECODE_RESULT ;

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

	bool ping(const char* host,unsigned int timeout=DEF_ICMP_TIMEOUT);
	void getStatus(int& delayTime,int& averDelayTime,int& lossRate,int& lossPacket,DWORD& err);//网络延时(ms），平均延时(ms)，丢包率，丢包个数

protected:
	void	initialize();
	void	release();

	bool	setSocket();
	bool	doPing(ULONG destIP);
protected:
	void	fillICMPData(char* icmpData, int dataSize);
	bool	decodeICMPHeader(char* buffer, int bytes, DECODE_RESULT * retDecode);

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
#ifndef _PING_H
#define _PING_H

#include <Windows.h>

#define MAX_HOST_NUM				(5)
#define DEF_ICMP_TIMEOUT			(5000)		//���峬ʱΪ5��

typedef unsigned int SOCKET;

/*
 *IP��ͷ�ṹ
 */
typedef struct
{
    byte h_len_ver ;					//IP�汾��
    byte tos ;							//��������
    unsigned short total_len ;			//IP���ܳ���
    unsigned short ident ;				//��ʶ
    unsigned short frag_and_flags ;		//��־λ
    byte ttl ;							//����ʱ��
    byte proto ;						//Э��
    unsigned short cksum ;				//IP�ײ�У���
    unsigned long sourceIP ;			//ԴIP��ַ
    unsigned long destIP ;				//Ŀ��IP��ַ
} IP_HEADER ;

/*
 *����ICMP��������
 */
typedef struct _ICMP_HEADER
{
    byte type ;							//����-----8
    byte code ;							//����-----8
    unsigned short cksum ;				//У���------16
    unsigned short id ;					//��ʶ��-------16
    unsigned short seq ;				//���к�------16
    unsigned int choose ;				//ѡ��-------32
} ICMP_HEADER ;

typedef struct
{
	int			seq;				//��¼���к�
	DWORD		time;				//��¼��ǰʱ��
	byte		ttl;				//����ʱ��
	in_addr		addr ;				//ԴIP��ַ
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
	void getStatus(int& delayTime,int& averDelayTime,int& lossRate,int& lossPacket,DWORD& err);//������ʱ(ms����ƽ����ʱ(ms)�������ʣ���������

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
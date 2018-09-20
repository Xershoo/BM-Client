#ifndef INBUFPACKET_H
#define INBUFPACKET_H
#include <map>
#include <list>
#include "sc_CSLock.h"

using namespace std;

class CInBuffer
{
public:
	CInBuffer(int nSize);
	~CInBuffer();
	bool copyData(unsigned char* pBuf,unsigned int nSize,unsigned char&rtpPayloadFormat);
	inline unsigned char* startPtr() {return m_pInBuf;}
	inline unsigned int   getPacketSize() {return m_nDataSize;}
	inline unsigned short   getSeqNo()   {return m_nSeqNo;}
	inline unsigned int   getTimePts()   {return m_nTimePts;}
	inline unsigned int   getBufSize()   {return m_nBufSize;}
	inline unsigned int   getMaxSize()   {return m_nMaxSize;}
	inline unsigned char*  dataPtr()      {return (m_pInBuf+12);}
	inline unsigned int   dataSize()      {return (m_nDataSize-12);}
	void  reset();
private:
	unsigned short  m_nSeqNo;
	unsigned char*  m_pInBuf;
	unsigned int    m_nBufSize;
	unsigned int    m_nMaxSize;
	unsigned int    m_nDataSize;
	unsigned int    m_nTimePts;
	unsigned char*  m_dataptr;
	unsigned int    m_payloadsize;

};

class CInBufPacket
{
public:
	CInBufPacket();
	~CInBufPacket();
	CInBuffer* GetBusyPacket(unsigned int nLastPacketTime,unsigned int nDiffPts);
	void  PutFreePacket(CInBuffer* pInBuff);
	CInBuffer* GetFreePacket(unsigned int nSize);
	bool parsePacket(unsigned char* pSrcBuf,unsigned int pSrcBufSize,unsigned char** pPayloadData
		             ,unsigned int& nPayloadDataSize,unsigned char& nPayloadFormat);
	void putBufToInPacket(unsigned char*recvbuf,unsigned int nSize);
	unsigned char getRevcDataPayloadFormat(unsigned char*recvbuf);

	unsigned int getRecvDataSeqNo(unsigned char*recvbuf);
	inline int getLastReavPacketSeqNo() {return m_recvLastPacketNo;}
	inline unsigned int getSSRC() {return m_nLastReceivedSSRC;}
	inline unsigned short getCurReadSeqNo()  {return m_nLastSeqNo+1;}
private:
	unsigned int    m_nLastReceivedSSRC;
	unsigned short  m_nLastSeqNo;
	map<unsigned short,CInBuffer*> m_mapInPacket;
	map<unsigned short,CInBuffer*>::iterator m_mapInPacketIter;

	list<CInBuffer*>               m_listFreeIbBuffer;
	list<CInBuffer*>::iterator     m_listFreeIbBufferIter;

	map<unsigned short,unsigned int>  m_giveUPPacket;
	CMutexLock                     m_mapgiveLock;

	unsigned short                 m_recvLastPacketNo;
	CMutexLock                     m_listLock;
	CMutexLock                     m_mapLock;
	unsigned char                  m_bIsRound;
private:
	unsigned short     m_nMaxWaitTime;
	bool            m_bIsFirst;
};
#endif
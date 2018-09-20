#ifndef OUTBUFPACKET_H
#define OUTBUFPACKET_H
#include <map>
#include <list>
#include "sc_CSLock.h"

using namespace std;

class COutBuffer
{
public:
	COutBuffer(unsigned nbufsize);
	~COutBuffer();
	inline unsigned int getBufSize() {return m_bufsize;}
	void Reset();
public:
	inline unsigned totalBytesAvailable() const {return m_bufsize - m_datasize;}
	inline unsigned char* curPtr() const {return &m_buf[m_datasize];}
	inline void increment(unsigned numBytes) {m_datasize += numBytes;}
	inline unsigned curPacketSize() const {return m_datasize;}
	void enqueue(unsigned char const* from, unsigned numBytes);
	void enqueueWord(unsigned int word) ;
	void skipBytes(unsigned numBytes);
	void insertWord(unsigned int word, unsigned int toPosition) ;
	void insert(unsigned char const* from, unsigned numBytes, unsigned toPosition) ;
	bool wouldOverflow(unsigned numBytes) const { return (m_datasize+numBytes) <= m_bufsize;}
	void extract(unsigned char* to, unsigned numBytes,unsigned fromPosition);
	unsigned int extractWord(unsigned fromPosition) ;
	inline unsigned char* startPtr() { return m_buf;}
	inline unsigned short getSeqNo() { return m_nSeqNo;}
	inline void setSeqNo(unsigned short nSeqNo);
	inline unsigned long getTimePts() {return m_timepts ;}
private:
	unsigned char*  m_buf;
	unsigned int    m_datasize;
	unsigned int    m_bufsize;
	unsigned short  m_nSeqNo;
	unsigned long           m_timepts;
};

class COutBufPacket
{
public:
	COutBufPacket(unsigned int nSSRC = 0);
	~COutBufPacket();
	
public:
	void setSSRC(unsigned int nSSRC) {m_nSSRC = nSSRC; }
	COutBuffer* buildPacket(unsigned char* pBuf,unsigned int nBufLen,unsigned char nRTPPayloadType,
	unsigned int nTimestamp,bool isFirstPacket);
	void  PutBusyPacket(COutBuffer* pOutBuff);

	void  RemovePacket();
	COutBuffer * getBusyOutBufferByNO(unsigned short nNum);
	COutBuffer * getFreeOutBuffer(unsigned int nBufSize);
	void PutFreeOutBuffer(COutBuffer* pOutBuf);
private:
	unsigned int specialHeaderSize() const;
	void setMarkerBit(COutBuffer * pOutBuf);
private:
	unsigned int m_bufmaxSize;
	unsigned int m_nRTPHeaderSize;
	unsigned int m_MaxPacketSize;
private:
	map<unsigned short,COutBuffer*> m_mapOutPutPacket;
	map<unsigned short,COutBuffer*>::iterator m_mapOutPutIter;
	CMutexLock       m_OutLock;

	list<COutBuffer*>               m_listFreeOutBuffer;
	list<COutBuffer*>::iterator     m_listFreeIter;
	CMutexLock       m_freeLock;
private:
	unsigned short  m_nSeqNo;
	bool            m_IsFirstPacket; 
	unsigned int    m_nSSRC;
	unsigned int	m_nTimestampPosition;
    unsigned int    m_nSpecialHeaderPosition;
    unsigned int    m_nSpecialHeaderSize;
	unsigned int    m_nTotalFrameSpecificHeaderSizes;
	unsigned long    m_lastTime;

};

#endif
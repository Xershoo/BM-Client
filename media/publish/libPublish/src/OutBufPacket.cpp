#include "OutBufPacket.h"
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>

COutBuffer::COutBuffer(unsigned nbufsize)
{
	m_bufsize = nbufsize;
	m_buf = NULL;
	m_buf = (unsigned char*)malloc(m_bufsize);
	m_datasize = 0;
	m_nSeqNo = 0;
	m_timepts = 0;
}

COutBuffer::~COutBuffer()
{
	if(m_buf)
	{
		free(m_buf);
		m_buf = NULL;
	}
	m_datasize = 0;
	m_bufsize = 0;
}

void COutBuffer::Reset()
{
	m_datasize = 0;
	m_nSeqNo = 0;
	m_timepts = 0;
}


void COutBuffer::enqueue(unsigned char const* from, unsigned numBytes)
{
	if (numBytes > totalBytesAvailable()) 
	{
		numBytes = totalBytesAvailable();
	}

	if (curPtr() != from) 
		memmove(curPtr(), from, numBytes);
	increment(numBytes);
}

void COutBuffer::enqueueWord(unsigned int word) 
{
	unsigned int nWord = htonl(word);
	enqueue((unsigned char*)&nWord, 4);
}

void COutBuffer::skipBytes(unsigned numBytes)
{
	if (numBytes > totalBytesAvailable()) 
	{
		numBytes = totalBytesAvailable();
	}

	increment(numBytes);
}

void COutBuffer::insert(unsigned char const* from, unsigned numBytes, unsigned toPosition) 
{
	unsigned realToPosition = toPosition;
	if (realToPosition + numBytes > m_bufsize) 
	{
		if (realToPosition > m_bufsize) 
			return; // we can't do this
		numBytes = m_bufsize - realToPosition;
	}

	memmove(&m_buf[realToPosition], from, numBytes);
	if (toPosition + numBytes > m_datasize) 
	{
		m_datasize = toPosition + numBytes;
	}
}

void COutBuffer::extract(unsigned char* to, unsigned numBytes,unsigned fromPosition)
{
	unsigned realFromPosition =  fromPosition;
	if (realFromPosition + numBytes > m_bufsize)
	{ // sanity check
		if (realFromPosition > m_bufsize) 
			return; // we can't do this
		numBytes = m_bufsize - realFromPosition;
	}

	memmove(to, &m_buf[realFromPosition], numBytes);
}

unsigned int COutBuffer::extractWord(unsigned fromPosition) 
{
	unsigned int nWord;
	extract((unsigned char*)&nWord, 4, fromPosition);
	return ntohl(nWord);
}

void COutBuffer::insertWord(unsigned int word, unsigned int toPosition) 
{
	unsigned int nWord = htonl(word);
	insert((unsigned char*)&nWord, 4, toPosition);
}

void COutBuffer::setSeqNo(unsigned short nSeqNo) 
{
	m_nSeqNo = nSeqNo;
	m_timepts = GetTickCount();
}

COutBufPacket::COutBufPacket(unsigned int nSSRC)
{
	m_bufmaxSize = 1024;
	m_nRTPHeaderSize = 48;
	m_MaxPacketSize = 64;
	m_mapOutPutPacket.clear();
	m_listFreeOutBuffer.clear();
	m_nSeqNo = 1;
	m_IsFirstPacket = 0; 
	m_nSSRC = 0;
	m_nTimestampPosition = 0;
    m_nSpecialHeaderPosition = 0;
    m_nSpecialHeaderSize = 0;
	m_nTotalFrameSpecificHeaderSizes = 0;
	m_lastTime = 0;
}

COutBufPacket::~COutBufPacket()
{
	m_OutLock.Lock();
	map<unsigned short,COutBuffer*>::iterator mapOutPutIter;
	for(mapOutPutIter = m_mapOutPutPacket.begin();mapOutPutIter != m_mapOutPutPacket.end();mapOutPutIter++)
	{
		delete mapOutPutIter->second;
		mapOutPutIter->second = NULL;
	}
	m_mapOutPutPacket.clear();
	m_OutLock.Unlock();

	
	m_freeLock.Lock();
	for(m_listFreeIter = m_listFreeOutBuffer.begin();m_listFreeIter != m_listFreeOutBuffer.end();m_listFreeIter++)
	{
		delete (*m_listFreeIter);
	}
	m_listFreeOutBuffer.clear();
	m_freeLock.Unlock();
}

unsigned int COutBufPacket::specialHeaderSize() const
{
	return 0;
}

void COutBufPacket::setMarkerBit(COutBuffer * pOutBuf)
{
	if(pOutBuf)
	{
		unsigned rtpHdr = pOutBuf->extractWord(0);
		rtpHdr |= 0x00800000;
		pOutBuf->insertWord(rtpHdr, 0);
	}
}

COutBuffer* COutBufPacket::buildPacket(unsigned char* pBuf,unsigned int nBufLen,unsigned char nRTPPayloadType
								,unsigned int nTimestamp,bool isFirstPacket)
{
	if(pBuf == NULL || nBufLen == 0 )
		return NULL;
	unsigned int nCopySize = 0;
	COutBuffer * pOutBuf = getFreeOutBuffer(nBufLen);
	if(pOutBuf)
	{
		m_IsFirstPacket = isFirstPacket;

		// Set up the RTP header:
		unsigned rtpHdr = 0x80000000; // RTP version 2
		rtpHdr |= (nRTPPayloadType<<16);
		if(nRTPPayloadType == 0)
		{
			unsigned short nSeqNo = m_nSeqNo++;
			pOutBuf->setSeqNo(nSeqNo);
			rtpHdr |= nSeqNo; // sequence number
		}
		else
		{
			rtpHdr |= 0;
		}

		pOutBuf->enqueueWord(rtpHdr);

		m_nTimestampPosition = pOutBuf->curPacketSize();
		pOutBuf->skipBytes(4); 
		pOutBuf->enqueueWord(m_nSSRC);
		// RTP header:
		//m_nSpecialHeaderPosition = pOutBuf->curPacketSize();
		//m_nSpecialHeaderSize = specialHeaderSize();
		//pOutBuf->skipBytes(m_nSpecialHeaderSize);
		//m_nTotalFrameSpecificHeaderSizes = 0;

		if(pOutBuf->wouldOverflow(nBufLen))
		{
			pOutBuf->enqueue(pBuf, nBufLen);
			setMarkerBit(pOutBuf);
			pOutBuf->insertWord(nTimestamp, m_nTimestampPosition);
			return pOutBuf;
		}
		else
		{
			PutFreeOutBuffer(pOutBuf);
			pOutBuf = NULL;
		}
	}
	return NULL;

}



COutBuffer * COutBufPacket::getFreeOutBuffer(unsigned int nBufSize)
{
	COutBuffer * pOutBuf  = NULL;
	if(m_bufmaxSize >  nBufSize + m_nRTPHeaderSize )
	{
		m_freeLock.Lock();
		if(m_listFreeOutBuffer.size() <= 0)
		{
			pOutBuf = new COutBuffer(m_bufmaxSize);
		}
		else
		{
			pOutBuf = m_listFreeOutBuffer.front();
			m_listFreeOutBuffer.pop_front();
		}
		m_freeLock.Unlock();
	}
	else
	{
		pOutBuf = new COutBuffer(nBufSize + m_bufmaxSize);
	}
	return pOutBuf;
}

void COutBufPacket::PutFreeOutBuffer(COutBuffer* pOutBuf)
{
	if(pOutBuf)
	{
		m_freeLock.Lock();
		if(pOutBuf->getBufSize() == m_bufmaxSize && m_listFreeOutBuffer.size() < m_MaxPacketSize)
		{
			pOutBuf->Reset();
			m_listFreeOutBuffer.push_back(pOutBuf);
		}
		else
		{
			delete pOutBuf;
			pOutBuf = NULL;
		}
		m_freeLock.Unlock();
	}
}

void  COutBufPacket::PutBusyPacket(COutBuffer* pOutBuff)
{
	if(pOutBuff)
	{
		m_OutLock.Lock();
		m_mapOutPutPacket[pOutBuff->getSeqNo()] = pOutBuff;
		m_OutLock.Unlock();
	}
}

void  COutBufPacket::RemovePacket()
{
	DWORD nowtime = GetTickCount();
	if(nowtime - m_lastTime > 1000)
	{
		m_OutLock.Lock();
		m_mapOutPutIter = m_mapOutPutPacket.begin();
		while(m_mapOutPutIter != m_mapOutPutPacket.end())
		{
			DWORD dwDiff = nowtime - m_mapOutPutIter->second->getTimePts();
			if(dwDiff > 2000)
			{
				PutFreeOutBuffer(m_mapOutPutIter->second);
				m_mapOutPutPacket.erase(m_mapOutPutIter++);
			}
			else
			{
				m_mapOutPutIter++;
			}
		}
		m_OutLock.Unlock();
		m_lastTime = nowtime;
	}
}

COutBuffer * COutBufPacket::getBusyOutBufferByNO(unsigned short nNum)
{
	COutBuffer * pOutBuf = NULL;
	m_OutLock.Lock();
	m_mapOutPutIter = m_mapOutPutPacket.find(nNum);
	if(m_mapOutPutIter != m_mapOutPutPacket.end())
	{
		pOutBuf = m_mapOutPutIter->second;
		m_mapOutPutPacket.erase(m_mapOutPutIter);
	}
	m_OutLock.Unlock();
	return pOutBuf;
}
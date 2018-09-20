#include "InBufPacket.h"
#include <WinSock2.h>



CInBuffer::CInBuffer(int nSize)
{
	m_nSeqNo = 1;
	m_pInBuf = NULL;
	m_nDataSize = 0;
	m_nMaxSize = 1024;
	m_nTimePts = 0;
	m_nBufSize = m_nMaxSize >=  nSize ? m_nMaxSize : nSize;
	m_pInBuf = (unsigned char*)malloc(m_nBufSize);
	m_dataptr = NULL;
	m_payloadsize = 0;

}

CInBuffer::~CInBuffer()
{
	if(m_pInBuf)
	{
		free(m_pInBuf);
		m_pInBuf = NULL;
	}
}

bool CInBuffer::copyData(unsigned char* pBuf,unsigned int nSize,unsigned char&rtpPayloadFormat)
{
	if(m_nBufSize < nSize)
		return false;
	unsigned rtpHdr = ntohl(*(unsigned int*)(pBuf));
	m_nSeqNo = (unsigned short)(rtpHdr&0xFFFF);
	unsigned rtpTimestamp = ntohl(*(unsigned int*)(pBuf+4));
	m_nTimePts = rtpTimestamp;
	memcpy(m_pInBuf,pBuf,nSize);
	m_nDataSize = nSize;
	rtpPayloadFormat = (unsigned char)((rtpHdr&0x007F0000)>>16);
	return true;
}

void CInBuffer::reset()
{
	m_nSeqNo = 1;
	m_nTimePts = 0;
	m_dataptr = NULL;
	m_payloadsize = 0;
}


CInBufPacket::CInBufPacket()
{
	m_nLastReceivedSSRC = 0;
	m_nLastSeqNo = 0;
	m_nMaxWaitTime = 500;
	m_mapInPacket.clear();
	m_listFreeIbBuffer.clear();
	m_giveUPPacket.clear();
	m_recvLastPacketNo = 0;
	m_bIsRound = false;
	m_bIsFirst = true;
}

CInBufPacket::~CInBufPacket()
{

	m_mapLock.Lock();
	for(m_mapInPacketIter = m_mapInPacket.begin();m_mapInPacketIter != m_mapInPacket.end();m_mapInPacketIter++)
	{
		if(m_mapInPacketIter->second)
		{
			delete m_mapInPacketIter->second;
			m_mapInPacketIter->second = NULL;
		}
	}
	m_mapInPacket.clear();
	m_mapLock.Unlock();

	m_listLock.Lock();
	for(m_listFreeIbBufferIter = m_listFreeIbBuffer.begin();m_listFreeIbBufferIter != m_listFreeIbBuffer.end();m_listFreeIbBufferIter++)
	{
		if(*m_listFreeIbBufferIter)
		{
			delete (*m_listFreeIbBufferIter);
			*m_listFreeIbBufferIter = NULL;
		}
	}
	m_listFreeIbBuffer.clear();
	m_listLock.Unlock();

}

	
bool CInBufPacket::parsePacket(unsigned char* pSrcBuf,unsigned int pSrcBufSize ,unsigned char** pPayloadData,unsigned int& nPayloadDataSize,
	unsigned char& nPayloadFormat)
{
	if(pSrcBuf == NULL || pSrcBufSize == 0)
		return false;

	if (pSrcBufSize < 12) 
		return false;
	unsigned char * p = pSrcBuf;

	unsigned rtpHdr = ntohl(*(unsigned int*)(p));
	p += 4;

	bool rtpMarkerBit = (rtpHdr&0x00800000) >> 23;
	unsigned rtpTimestamp = ntohl(*(unsigned int*)(p));
	p += 4;
	unsigned rtpSSRC = ntohl(*(unsigned int*)(p));
	p += 4;

	if ((rtpHdr&0xC0000000) != 0x80000000) 
		return false;

	unsigned cc = (rtpHdr>>24)&0xF;
	int datasize = pSrcBufSize - (p - pSrcBuf);

	if (datasize < cc) 
		return false;

	p += (cc*4);

	if (rtpHdr&0x10000000) 
	{
		datasize = pSrcBufSize - (p - pSrcBuf);
		if (datasize < 4) 
			return false;
		unsigned extHdr = ntohl(*(unsigned char*)(p));
		p += 4;
		unsigned remExtSize = 4*(extHdr&0xFFFF);
		datasize = pSrcBufSize - (p - pSrcBuf);
		if (datasize < remExtSize)
			return false;
		p += remExtSize;
	}

	if (rtpHdr&0x20000000) 
	{
		datasize = pSrcBufSize - (p - pSrcBuf);
		if (datasize == 0) 
			return false;
		unsigned numPaddingBytes = (unsigned)(p)[datasize-1];
		if (datasize < numPaddingBytes) 
			return false;
		p += numPaddingBytes;
	}
	unsigned char rtpPayloadFormat = (unsigned char)((rtpHdr&0x007F0000)>>16);

	if (rtpSSRC != m_nLastReceivedSSRC) 
	{
		m_nLastReceivedSSRC = rtpSSRC;
	}

	unsigned short rtpSeqNo = (unsigned short)(rtpHdr&0xFFFF);

	nPayloadDataSize = (unsigned int)(pSrcBufSize - (p - pSrcBuf));
	*pPayloadData = p;

	nPayloadFormat = rtpPayloadFormat;
	return true;
}

unsigned int CInBufPacket::getRecvDataSeqNo(unsigned char*recvbuf)
{
	unsigned rtpHdr = ntohl(*(unsigned int*)(recvbuf));
	return (unsigned short)(rtpHdr&0xFFFF);;
}

unsigned char CInBufPacket::getRevcDataPayloadFormat(unsigned char*recvbuf)
{
	unsigned rtpHdr = ntohl(*(unsigned int*)(recvbuf));
	return (unsigned char)((rtpHdr&0x007F0000)>>16);
}

CInBuffer* CInBufPacket::GetBusyPacket(unsigned int nLastPacketTime,unsigned int nDiffPts)
{
	CInBuffer* pInBuff = NULL;
	unsigned short nCurSeqNo = m_nLastSeqNo+1;
	if(nCurSeqNo > 65536)
	{
		nCurSeqNo = 0;
	}
	m_mapLock.Lock();
	m_mapInPacketIter = m_mapInPacket.find(nCurSeqNo);
	if(m_mapInPacketIter != m_mapInPacket.end())
	{
		m_nLastSeqNo = m_mapInPacketIter->first;
		pInBuff = m_mapInPacketIter->second;
		m_mapInPacket.erase(m_mapInPacketIter);
		m_mapLock.Unlock();
		return pInBuff;
	}
	else
	{
		if(m_bIsFirst || nDiffPts > m_nMaxWaitTime)
		{
			if(m_mapInPacket.size() > 0)
			{
				m_mapInPacketIter = m_mapInPacket.begin() ; 
			    map<unsigned short,CInBuffer*>::iterator mapInPacketIter;
				while(m_mapInPacketIter != m_mapInPacket.end())
				{
					if(m_mapInPacketIter->second->getTimePts() < nLastPacketTime)
					{
						nLastPacketTime = m_mapInPacketIter->second->getTimePts();
						
						mapInPacketIter = m_mapInPacketIter;
					}
					m_mapInPacketIter++;
				}
				if(mapInPacketIter->second)
				{
					m_bIsFirst = false;
					int nSize = m_mapInPacket.size();
					m_nLastSeqNo = mapInPacketIter->first;
					pInBuff = mapInPacketIter->second;
					m_mapInPacket.erase(mapInPacketIter);
					m_mapLock.Unlock();
					return pInBuff;
				}
			}
		}
	}
	m_mapLock.Unlock();
	return NULL;
}

void  CInBufPacket::PutFreePacket(CInBuffer* pInBuff)
{
	if(pInBuff)
	{
		if(m_listFreeIbBuffer.size() < 32 && pInBuff->getBufSize() < pInBuff->getMaxSize() )
		{
			pInBuff->reset();

			m_listLock.Lock();
			m_listFreeIbBuffer.push_back(pInBuff);
			m_listLock.Unlock();
		}
		else
		{
			delete pInBuff;
			pInBuff = NULL;
		}
	}
}

CInBuffer* CInBufPacket::GetFreePacket(unsigned int nSize)
{
	CInBuffer* pInBuf = NULL;
	if(nSize > 1024 || m_listFreeIbBuffer.size() <= 0)
	{
		pInBuf = new CInBuffer(nSize);
	}
	else
	{
		m_listLock.Lock();
		pInBuf = m_listFreeIbBuffer.front();
		m_listFreeIbBuffer.pop_front();
		m_listLock.Unlock();
	}
	return pInBuf;
}

void CInBufPacket::putBufToInPacket(unsigned char*recvbuf,unsigned int nSize)
{
	CInBuffer* pInBuf = GetFreePacket(nSize);
	if(pInBuf)
	{
		unsigned char rtpPayloadFormat = 0;
		pInBuf->copyData(recvbuf,nSize,rtpPayloadFormat);
		if(rtpPayloadFormat == 0)
		{
	
			m_mapLock.Lock();
			m_recvLastPacketNo = pInBuf->getSeqNo();
			m_mapInPacket[pInBuf->getSeqNo()] = pInBuf;
			m_mapLock.Unlock();
		}
		/*bool bIsPush = true;
		unsigned char rtpPayloadFormat = 0;
		pInBuf->copyData(recvbuf,nSize,rtpPayloadFormat);
		unsigned short nCurRecvPacketNo = pInBuf->getSeqNo();

		if(nCurRecvPacketNo < 32768 && m_recvLastPacketNo >  32768)
		{
			m_bIsRound = true;
		}
		else
		{
			m_bIsRound = false;
		}

		if(m_giveUPPacket.size() > 0)
		{
			m_mapgiveLock.Lock();
			map<unsigned short,unsigned int>::iterator mapGiveUPIter;
			mapGiveUPIter = m_giveUPPacket.find(nCurRecvPacketNo);
			if(mapGiveUPIter != m_giveUPPacket.end())
			{
				bIsPush = false;
				m_giveUPPacket.erase(mapGiveUPIter);
			}
			m_mapgiveLock.Unlock();

		}
		
		if(nCurRecvPacketNo - m_recvLastPacketNo > 1)
			printf("cur packet no %d  last packet no %d\n",nCurRecvPacketNo,m_recvLastPacketNo);

		if(bIsPush)
		{
			if(nCurRecvPacketNo != m_recvLastPacketNo)
			{
				if(!m_bIsRound)
				{
					if(nCurRecvPacketNo > m_recvLastPacketNo)
					{
						int nGiveUpNo = m_recvLastPacketNo;
			
						
						if(nCurRecvPacketNo - nGiveUpNo  > 1)
						{
							m_mapgiveLock.Lock();
							do
							{
								nGiveUpNo += 1;
								timeNode tn;
								tn.recvTime = nRecvTime;
								tn.lastSendTime = nRecvTime;
								m_giveUPPacket[nGiveUpNo] = tn;
							}while(nCurRecvPacketNo - nGiveUpNo > 1);
							m_mapgiveLock.Unlock();
						}
						m_recvLastPacketNo = nCurRecvPacketNo;
					}
				}
				else
				{
					int nGiveUpNo = m_recvLastPacketNo;
					struct timeval nRecvTime;
					gettimeofday(&nRecvTime,NULL);

					if(65536 - nGiveUpNo > 1)
					{
						m_mapgiveLock.Lock();
						do
						{
							nGiveUpNo += 1;
							timeNode tn;
							tn.recvTime = nRecvTime;
							tn.lastSendTime = nRecvTime;
							m_giveUPPacket[nGiveUpNo] = tn;
						}while(65536 - nGiveUpNo > 1);
						m_mapgiveLock.Unlock();
					}

					nGiveUpNo = 0;
					if(nCurRecvPacketNo - nGiveUpNo > 0)
					{
						m_mapgiveLock.Lock();
						do
						{
							timeNode tn;
							tn.recvTime = nRecvTime;
							tn.lastSendTime = nRecvTime;
							m_giveUPPacket[nGiveUpNo] = tn;
							nGiveUpNo += 1;
						}while(nCurRecvPacketNo - nGiveUpNo > 0);
						m_mapgiveLock.Unlock();
					}
					m_recvLastPacketNo = nCurRecvPacketNo;
				}
			}
			
			if(rtpPayloadFormat == 0)
			{
				m_mapLock.Lock();
				m_mapInPacket[pInBuf->getSeqNo()] = pInBuf;
				m_mapLock.Unlock();
			}
		}*/
	
	}
}
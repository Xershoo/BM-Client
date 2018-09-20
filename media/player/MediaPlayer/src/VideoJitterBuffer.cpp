#include "VideojitterBuffer.h"

CVideoJitterBuffer::CVideoJitterBuffer()
{
	m_listH264Video.clear();
	m_nMaxFrameSize = 0;
	m_VideoH = 0;
	m_VideoW = 0;

	m_nFrameCount = 0;
	m_nMaxVideoJitterTime = 0;
	m_nBufferCacheTime = 0;
	m_nLastSysRecvFrameTime = 0;
	m_nMaxTempJitterTime = 0;
}

CVideoJitterBuffer::~CVideoJitterBuffer()
{

	clear();
	ClearMemNode();
}

void CVideoJitterBuffer::push(unsigned char *prefix,unsigned int nPrefixLen,unsigned char* pData,unsigned int nDataLen, int64_t pts)
{
	MEMNode node;
	if(pData == NULL || nDataLen == 0 || prefix == NULL || nPrefixLen == 0)
		return ;


	if(m_lastVideoFrame.pFrame == NULL || m_lastVideoFrame.nFrameSize <= 0)
	{
		int nDataSize = nPrefixLen + nDataLen;
		GetMemNode(node,nDataSize);
		m_lastVideoFrame.pFrame = node.buf;
		if(m_lastVideoFrame.pFrame)
		{
			memcpy(m_lastVideoFrame.pFrame,prefix,nPrefixLen);
			memcpy(m_lastVideoFrame.pFrame+nPrefixLen,pData,nDataLen);
			m_lastVideoFrame.mW = 0;
			m_lastVideoFrame.nH = 0;
			m_lastVideoFrame.nFrameSize = nDataSize;
			m_lastVideoFrame.pts = pts;
			m_lastVideoFrame.nSpan = 0;
			m_nLastSysRecvFrameTime = timeGetTime();
		}
	}
	else
	{
		int nDataSize = nPrefixLen + nDataLen;
		GetMemNode(node,nDataSize);
		H264VideoFrame hvf;
		hvf.pFrame = node.buf;
		hvf.pts = pts;
		hvf.nSpan = 0;
		hvf.mW = 0;
		hvf.nH = 0;
		if(hvf.pFrame)
		{
			memcpy(hvf.pFrame,prefix,nPrefixLen);
			memcpy(hvf.pFrame+nPrefixLen,pData,nDataLen);
			hvf.nFrameSize = nDataSize;
		}
		else
		{
			int n = 0;
		}
		m_lastVideoFrame.nSpan = hvf.pts - m_lastVideoFrame.pts;

		if(m_lastVideoFrame.nSpan > 1000)
		{
			m_lastVideoFrame.nSpan = 0;
		}
		unsigned int nCurSysRecvFrameTime = timeGetTime();
		int nJitter = nCurSysRecvFrameTime - m_nLastSysRecvFrameTime;
		m_nLastSysRecvFrameTime = nCurSysRecvFrameTime;

		m_H264VideoLock.Lock();
		m_nFrameCount++;

		m_nMaxTempJitterTime = m_nMaxTempJitterTime > nJitter ? m_nMaxTempJitterTime : nJitter;
		m_nMaxTempJitterTime = m_nMaxTempJitterTime > 1000 ? 1000 : m_nMaxTempJitterTime;
		if(m_nMaxTempJitterTime  > m_nMaxVideoJitterTime)
		{
			m_nMaxVideoJitterTime = m_nMaxTempJitterTime;
			m_nFrameCount = 0;
			m_nMaxTempJitterTime = 0;
		}

		if(m_nFrameCount > 100)
		{
			if(m_nMaxVideoJitterTime > m_nMaxTempJitterTime)
			{
				int nTemp = m_nMaxVideoJitterTime * 0.85 ;

				m_nMaxVideoJitterTime = nTemp > m_nMaxTempJitterTime ? nTemp : m_nMaxTempJitterTime ;
			}
			m_nFrameCount = 0;
		}

		m_nBufferCacheTime += m_lastVideoFrame.nSpan;
		m_listH264Video.push_back(m_lastVideoFrame);
		m_H264VideoLock.Unlock();
		m_lastVideoFrame = hvf;
	}
}

void CVideoJitterBuffer::pop(H264VideoFrame &hvf,int& nMaxJitterTime, int& nBufferCacheTime,bool bIsFlag)
{
	m_H264VideoLock.Lock();
	if(m_listH264Video.size() > 0)
	{
		nMaxJitterTime = m_nMaxVideoJitterTime;
		hvf = m_listH264Video.front();
		m_listH264Video.pop_front();
		m_nBufferCacheTime -= hvf.nSpan;
		nBufferCacheTime = m_nBufferCacheTime;

		if(!bIsFlag )
		{
			float fltRate = (m_nBufferCacheTime*1.0) / m_nMaxVideoJitterTime;
			if(fltRate > 1.0)
			{
				float fltTemp = (hvf.nSpan * (fltRate - 0.8));
				int nTemp = fltTemp > hvf.nSpan ? hvf.nSpan : fltTemp;
				hvf.nSpan = hvf.nSpan - nTemp;
			}
			else if(fltRate < 1.0)
			{
				float fltTemp = (hvf.nSpan * (fltRate + 0.5));
				int nTemp = fltTemp > hvf.nSpan ? hvf.nSpan : fltTemp;
				hvf.nSpan = hvf.nSpan + nTemp;
			}
		}
	}
	m_H264VideoLock.Unlock();
}

void CVideoJitterBuffer::clear()
{
	m_H264VideoLock.Lock();
	while(m_listH264Video.size() > 0)
	{
		H264VideoFrame hvf = m_listH264Video.front();
		m_listH264Video.pop_front();
		freeVideoFrameMem(hvf);
	}
	m_listH264Video.clear();
	if(m_lastVideoFrame.pFrame)
	{
		free(m_lastVideoFrame.pFrame);
		m_lastVideoFrame.pFrame = NULL;
	}
	m_H264VideoLock.Unlock();
	m_nFrameCount = 0;
	m_nMaxVideoJitterTime = 0;
	m_nBufferCacheTime = 0;
	m_nLastSysRecvFrameTime = 0;
	m_nMaxTempJitterTime = 0;
}

void CVideoJitterBuffer::freeVideoFrameMem(H264VideoFrame& hvf)
{
	if(hvf.pFrame == NULL)
		return;
	MEMNode  mn;
	mn.buf = hvf.pFrame;
	mn.size = hvf.nFrameSize;
	PutMemNode(mn);
}

void CVideoJitterBuffer::GetMemNode(MEMNode& mnode,int len)
{
	m_memLock.Lock();
	int nSize = m_freeMemList.size();
	if(nSize > 0)
	{
		list<MEMNode>::iterator iter;
		for(iter = m_freeMemList.begin();iter != m_freeMemList.end();iter++)
		{
			MEMNode node = (*iter);
			if(node.size > len)
			{
				mnode = node;
				m_freeMemList.erase(iter);
				m_memLock.Unlock();
				return;
			}
		}
	}
	m_memLock.Unlock();
	
	unsigned int nDataLen = len ;
	mnode.buf = (unsigned char*)malloc(nDataLen);
	mnode.size = nDataLen;
}

void CVideoJitterBuffer::PutMemNode(MEMNode& mnode)
{
	m_memLock.Lock();
	int nSize = m_freeMemList.size();
	if(nSize > 5)
	{
		free(mnode.buf);
	}
	else
	{
		m_freeMemList.push_back(mnode);
	}
	m_memLock.Unlock();
}

void CVideoJitterBuffer::ClearMemNode()
{
	m_memLock.Lock();
	int nSize = m_freeMemList.size();
	if(nSize > 0)
	{
		list<MEMNode>::iterator iter;
		for(iter = m_freeMemList.begin();iter != m_freeMemList.end();iter++)
		{
			MEMNode node = (*iter);
			free(node.buf);
		}
	}
	m_memLock.Unlock();
	m_freeMemList.clear();
}
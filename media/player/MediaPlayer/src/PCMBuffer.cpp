#include "StdAfx.h"

#include "PCMBuffer.h"
#include <stdio.h>
#include "wchar.h"
#include <string>

//void CALLBACK _threadRecvPCMdata(void* dwUser)
//{
//      CPCMBuffer * pThis=(CPCMBuffer*)dwUser;//由this指针获得实例的指针
//      pThis->OnRecvPCMdata();//调用要回调的成员方法
//}

CPCMBuffer::CPCMBuffer()
{
	m_pWPCMBuffer = NULL;
	m_nWPCMBufferSize = 50 * PCMFRAMESIZE;
	m_nRPCMBufferSize = PCMFRAMESIZE;
	m_pRPCMBuffer = NULL;
	nWritePCMPos = 0;
	nReadPCMPos = 0;
	m_nCycleWAR = 0;
	m_bFirstCahce = true;
	m_nErrCount = 0;
	m_nEacheNum = 3;// 初始化开始缓存帧音频才开始播放
	m_nTempMaxEacheNum = 10; 
	m_nMaxEacheNum = 25; //最大缓存的20帧音频
	m_samplerate = 0;
	m_nchannel = 0;
	m_lastsamplerate = 0;
	m_lastnchannel = 0;
	m_nLastPcmFormatSize = 0;
	m_bIsFlag = false;
}

CPCMBuffer::~CPCMBuffer()
{
	if(m_pWPCMBuffer)
	{
		free(m_pWPCMBuffer);
		m_pWPCMBuffer = NULL;
	}
	if(m_pRPCMBuffer)
	{
		free(m_pRPCMBuffer);
		m_pRPCMBuffer = NULL;
	}
	nWritePCMPos = 0;
	nReadPCMPos = 0;
}

void CPCMBuffer::setConfig(bool bIsFlag)
{
	m_bIsFlag = bIsFlag;
	if(m_bIsFlag)
	{
		m_nEacheNum = 6;// 初始化开始缓存帧音频才开始播放
		m_nTempMaxEacheNum = 20; 
		m_nMaxEacheNum = 50; //最大缓存的20帧音频
		m_nWPCMBufferSize = 100 * PCMFRAMESIZE;
	}
}

void CPCMBuffer::ReSetConfig(unsigned int nJitterAvgTime,unsigned int nMaxJitterYime)
{
	if(m_nEacheNum * PCMFRAMETIME < nJitterAvgTime)
	{
		m_nEacheNum = (nJitterAvgTime) / PCMFRAMETIME + 1;
		m_nEacheNum = m_nEacheNum > 5 ? 5 : m_nEacheNum;
	}
	else
		m_nEacheNum = 3;

	int nMaxEacheNum = nMaxJitterYime / PCMFRAMETIME + 1;
	if(m_bIsFlag)
	{
		if(m_nEacheNum  <= 3)
			m_nEacheNum = 6;
		else
			m_nEacheNum *= 2;
		m_nEacheNum = m_nEacheNum > 10 ? 10 : m_nEacheNum;
		nMaxEacheNum *= 2;
	}
	nMaxEacheNum = nMaxEacheNum > m_nMaxEacheNum ? m_nMaxEacheNum : nMaxEacheNum;
	m_nTempMaxEacheNum = nMaxEacheNum > m_nTempMaxEacheNum ? nMaxEacheNum : m_nTempMaxEacheNum;
}

void CPCMBuffer::ClearBuffer()
{
	m_bFirstCahce = true;
	nWritePCMPos = 0;
	nReadPCMPos = 0;
	m_nCycleWAR = 0;
	m_nErrCount = 0;
	m_samplerate = 0;
	m_nchannel = 0;
	m_lastsamplerate = 0;
	m_lastnchannel = 0;
	if(m_bIsFlag)
	{
		m_nEacheNum = 6;// 初始化开始缓存帧音频才开始播放
		m_nMaxEacheNum = 50; //最大缓存的20帧音频
		m_nTempMaxEacheNum = 20; 
	}
	else
	{
		m_nEacheNum = 3;// 初始化开始缓存帧音频才开始播放
		m_nMaxEacheNum = 25; //最大缓存的20帧音频
		m_nTempMaxEacheNum = 10; 
	}
	m_nLastPcmFormatSize = 0;
}

unsigned int CPCMBuffer::GetCacheDataTime()
{
	int nPCMSize = PCMFRAMESIZE * m_nchannel * (m_samplerate / SIMPLERATENUM);
	float fltCacheTime  = (GetBusyPCMDataSize()* 1.0 / nPCMSize) * PCMFRAMETIME;
	unsigned int nCacheTime  = fltCacheTime / 10 * 10;
	return nCacheTime;
}

unsigned int CPCMBuffer::GetBusyPCMDataSize()
{
	return (nWritePCMPos - nReadPCMPos) + (m_nCycleWAR > 0 ? m_nWPCMBufferSize:0);
}


void CPCMBuffer::WritePCMData(unsigned char* data,unsigned int nLen,int	samplerate,int channel)
{
	m_mutexLock.Lock();
	int nBusyDataSize = (nWritePCMPos - nReadPCMPos) + (m_nCycleWAR > 0 ? m_nWPCMBufferSize:0);

	if(m_bFirstCahce && nBusyDataSize / PCMFRAMESIZE >= m_nEacheNum )
	{
		m_bFirstCahce = false;
	}

	if(m_samplerate != samplerate || m_nchannel != channel)
	{
		m_lastsamplerate = m_samplerate;
		m_lastnchannel = m_nchannel;
		m_samplerate = samplerate;
		m_nchannel = channel;
		m_nLastPcmFormatSize = nBusyDataSize;
	}

	if( nBusyDataSize / PCMFRAMESIZE  > m_nTempMaxEacheNum)
	{
		
		int nPCMSize = PCMFRAMESIZE * m_nchannel * (m_samplerate / SIMPLERATENUM);
		float fltTime = PCMFRAMETIME * 1.0 / nPCMSize * nLen;
		m_nDorpTime += fltTime;
		m_mutexLock.Unlock();
		return ;
	}

	if(m_pWPCMBuffer == NULL)
		m_pWPCMBuffer = (unsigned char*)malloc(m_nWPCMBufferSize);
	if(m_pWPCMBuffer)
	{
		int nLastFree = m_nWPCMBufferSize - nWritePCMPos;
		if(nLastFree >= nLen)
		{
			memcpy(m_pWPCMBuffer+nWritePCMPos,data,nLen);
		}
		else
		{
			memcpy(m_pWPCMBuffer+nWritePCMPos,data,nLastFree);
			memcpy(m_pWPCMBuffer,data+nLastFree,nLen - nLastFree);
		}
		nWritePCMPos += nLen;
		if(nWritePCMPos >= m_nWPCMBufferSize)
			m_nCycleWAR++;
		nWritePCMPos %= m_nWPCMBufferSize;
		if(m_nCycleWAR > 1 && nWritePCMPos > nReadPCMPos)
			nReadPCMPos = nWritePCMPos;
	}
	m_mutexLock.Unlock();
}

void CPCMBuffer::ReadPCMData(unsigned char** data,unsigned int& nLen,unsigned int& nZreoData,unsigned int& nPtsSpan)
{
	int nCopySize = 0;

	m_mutexLock.Lock();
	int nBusyDataSize = (nWritePCMPos - nReadPCMPos) + (m_nCycleWAR > 0 ? m_nWPCMBufferSize:0);
	if(m_bFirstCahce && nBusyDataSize / PCMFRAMESIZE < m_nEacheNum )
	{
		m_mutexLock.Unlock();
		nLen = 0;
		return ;
	}

	if(nLen > m_nRPCMBufferSize)
	{
		if(m_pRPCMBuffer)
		{
			free(m_pRPCMBuffer);
			m_pRPCMBuffer = NULL;
		}
		m_nRPCMBufferSize = nLen;
	}

	if(m_pRPCMBuffer == NULL)
	{
		m_pRPCMBuffer = (unsigned char*)malloc(m_nRPCMBufferSize);
	}
	
	if(m_pRPCMBuffer)
	{
		if(nBusyDataSize > PCMFRAMESIZE)
		{
			unsigned  nDataSize =  0;
			memset(m_pRPCMBuffer,0,PCMFRAMESIZE);
			if(nWritePCMPos > nReadPCMPos)
			{
				nDataSize = nWritePCMPos - nReadPCMPos;
			}
			else if(nWritePCMPos == nReadPCMPos)
			{
				if(m_nCycleWAR > 0)
					nDataSize = m_nWPCMBufferSize;
				else
					nDataSize = 0;
			}
			else 
			{
				nDataSize = (m_nWPCMBufferSize - nReadPCMPos) + nWritePCMPos;
			}

			nCopySize = nDataSize > nLen ? nLen : nDataSize;

			if(nCopySize > 0)
			{
				if(nReadPCMPos + nCopySize > m_nWPCMBufferSize)
				{
					int nLastDataSize = m_nWPCMBufferSize - nReadPCMPos;
					memcpy(m_pRPCMBuffer,m_pWPCMBuffer+nReadPCMPos,nLastDataSize);
					memcpy(m_pRPCMBuffer+nLastDataSize,m_pWPCMBuffer,nCopySize-nLastDataSize);
				}
				else
				{
					memcpy(m_pRPCMBuffer,m_pWPCMBuffer+nReadPCMPos,nCopySize);
				}
				nReadPCMPos += nCopySize;
				if(nReadPCMPos >= m_nWPCMBufferSize)
					m_nCycleWAR--;
				nReadPCMPos %= m_nWPCMBufferSize;

				if(nCopySize < PCMFRAMESIZE)
					m_nErrCount++;
				else
					m_nErrCount = 0;
			}
			else
			{
				m_nErrCount++;
				memset(m_pRPCMBuffer,0,PCMFRAMESIZE);
				nCopySize = 0;
			}
		}
		else
		{
			m_nErrCount++;
			memset(m_pRPCMBuffer,0,PCMFRAMESIZE);
			nCopySize = 0;
		}
		
	}

	nZreoData = nLen - nCopySize;
	nLen = nCopySize;
	*data = m_pRPCMBuffer;
	m_mutexLock.Unlock();
}



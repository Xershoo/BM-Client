#include "AudiojitterBuffer.h"
#include <stdio.h>
#include "wchar.h"

#define CACHEMINTIME 100
#define CACHEMAXTIME 100

#define MAX_AUDIO_FRAME_SIZE 19200

CAudioJitterBuffer::CAudioJitterBuffer()
{

	m_nMaxFrameSize = (MAX_AUDIO_FRAME_SIZE *3)/2;
	m_nPlayFrameSize = (MAX_AUDIO_FRAME_SIZE * 3)/2;

	m_lastAAFrame.pFrame = (unsigned char*)malloc(m_nMaxFrameSize);
	m_CurPlayAAFrame.pFrame = (unsigned char*)malloc(m_nPlayFrameSize);

	m_pJitter = NULL;
	m_nSamplerate = 0;
	m_nChannel = 0;
	m_framSize = 200;
	m_nSequence = 0;
	m_nSysSpan = ~0;
	m_bCahceTime = false;
	m_CahceTime = 0;
	m_nErrCount = 0;
	m_lastAAFrame.nChannel = 0;
	m_lastAAFrame.nFrameSize = 0;
	m_lastAAFrame.nSamplerate = 0;
	m_lastAAFrame.nSpan = 0;
	m_lastAAFrame.pts = 0;

	m_CurPlayAAFrame.nChannel = 0;
	m_CurPlayAAFrame.nFrameSize = 0;
	m_CurPlayAAFrame.nSamplerate = 0;
	m_CurPlayAAFrame.nSpan = 0;
	m_CurPlayAAFrame.pts = 0;

	m_lastRecvSysTime = 0;
	m_nMaxRecvDiffTime = 0;
	m_nTwoSecondAvgRecvTime = 0;
	n_nRecvCount = 0;
	n_nDiffTime = 0;
}

CAudioJitterBuffer::~CAudioJitterBuffer()
{
	m_jitterBufLock.Lock();
	if(m_pJitter)
	{
		jitter_buffer_destroy(m_pJitter);
		m_pJitter = NULL;
	}
	m_jitterBufLock.Unlock();
	if(m_lastAAFrame.pFrame)
	{
		free(m_lastAAFrame.pFrame);
		m_lastAAFrame.pFrame = NULL;
	}

	if(m_CurPlayAAFrame.pFrame)
	{
		free(m_CurPlayAAFrame.pFrame);
		m_CurPlayAAFrame.pFrame = NULL;
	}
}

void CAudioJitterBuffer::Init()
{
	m_nSamplerate = 0;
	m_nChannel = 0;
	m_framSize = 200;
	m_nSequence = 0;
	m_nSysSpan = ~0;
	m_bCahceTime = false;
	m_CahceTime = 0;
	m_nErrCount = 0;
	m_lastAAFrame.nChannel = 0;
	m_lastAAFrame.nFrameSize = 0;
	m_lastAAFrame.nSamplerate = 0;
	m_lastAAFrame.nSpan = 0;
	m_lastAAFrame.pts = 0;

	m_CurPlayAAFrame.nChannel = 0;
	m_CurPlayAAFrame.nFrameSize = 0;
	m_CurPlayAAFrame.nSamplerate = 0;
	m_CurPlayAAFrame.nSpan = 0;
	m_CurPlayAAFrame.pts = 0;
	m_lastRecvSysTime = 0;
	m_nMaxRecvDiffTime = 0;
	m_nTwoSecondAvgRecvTime = 0;
	n_nRecvCount = 0;
	n_nDiffTime = 0;
	if(m_pJitter == NULL)
	{
		m_pJitter = jitter_buffer_init(1);
		jitter_buffer_update_delay(m_pJitter, NULL, NULL);
	}
	m_bIsInit = true;
	
}
void CAudioJitterBuffer::unInit()
{
	m_jitterBufLock.Lock();
	m_bIsInit = false;
	if(m_pJitter)
	{
		jitter_buffer_destroy(m_pJitter);
		m_pJitter = NULL;
	}
	m_jitterBufLock.Unlock();
}

void CAudioJitterBuffer::Push(char* pData,unsigned int nSize,unsigned int nPts)
{
	JitterBufferPacket p;

	if(pData == NULL || nSize <= 0 )
	{
		return;
	}

	p.data = (char*)pData;
	p.len = nSize;
	p.timestamp = m_nSequence;
	p.span = 1;
	p.sequence = m_nSequence++;

	DWORD  nCurSysTime = timeGetTime();
	if(m_lastRecvSysTime == 0)
	{
		m_lastRecvSysTime = nCurSysTime;
	}

	int nDiff = nCurSysTime - m_lastRecvSysTime;
	if(nDiff > 2000)
	{
		m_lastRecvSysTime = 0;
		m_nMaxRecvDiffTime = 0;
		m_nTwoSecondAvgRecvTime = 0;
		n_nRecvCount = 0;
		n_nDiffTime = 0;
	}
	else if(nDiff > 20)
	{
		if(n_nRecvCount >= 20)
		{
			m_nTwoSecondAvgRecvTime = n_nDiffTime / n_nRecvCount;
			n_nRecvCount = 0;
			n_nDiffTime = 0;
		}
		n_nRecvCount++;
		n_nDiffTime += nDiff;
		if(m_nMaxRecvDiffTime < nDiff )
			m_nMaxRecvDiffTime = nDiff;
	}

	m_lastRecvSysTime = nCurSysTime;
	p.user_data = nPts;
	
	if(m_pJitter)
	{
		m_jitterBufLock.Lock();
		jitter_buffer_put(m_pJitter, &p);
		m_jitterBufLock.Unlock();
	}

}

void CAudioJitterBuffer::reset()
{
	if(m_pJitter)
	{
		m_jitterBufLock.Lock();
		jitter_buffer_reset(m_pJitter);
		m_jitterBufLock.Unlock();
	}
	m_bIsInit = false;
}

int CAudioJitterBuffer::Pop(char** pData,unsigned int& nSize,unsigned int& nPts ,bool bIsWait)
{
	if(NULL == m_pJitter)
	{
		return 3;
	}

	JitterBufferPacket packet;

	packet.data = (char*)m_CurPlayAAFrame.pFrame;
	packet.len = m_nPlayFrameSize;

	if(m_nSysSpan == (~0))
	{
		m_nSysSpan = timeGetTime();
	}

	m_jitterBufLock.Lock();

	int nr = 0;
	do 
	{
		DWORD nCurSysTime = timeGetTime();
		if(bIsWait)
		{
			unsigned int des_delay = nCurSysTime - m_nSysSpan;
			if(des_delay < 20 )
			{
				nr = 2;
				break;
			}
			m_nSysSpan = nCurSysTime;
		}

		nr = jitter_buffer_get(m_pJitter, &packet,1, NULL);

		if (JITTER_BUFFER_OK != nr)
		{
			if(m_CahceTime != 0)
			{
				m_CahceTime = 0;
			}

			m_nErrCount++;
			if(m_nErrCount > 100)
			{
				nr = 3;
				break;
			}

			if(m_nErrCount > 3)
			{
				jitter_buffer_tick(m_pJitter);
				nr = 1;

				break;
			}
			
			nr = 2;

			break;
		}

		m_nErrCount = 0;
		jitter_buffer_tick(m_pJitter);		

		*pData = packet.data;	
		nSize = packet.len;
		nPts = packet.user_data;
	} while (false);

	m_jitterBufLock.Unlock();

	return nr;
}
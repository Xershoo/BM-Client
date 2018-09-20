//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�RtmpStream.cpp
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.24
//	˵����rtmp�������Ͳ��ŷ�װ����
//  �ӿڣ�
//	�޸ļ�¼��
//**********************************************************************

#include "RtmpStream.h"
#include <QMutexLocker>

CRTMPStream::CRTMPStream()
{
    m_isStart = false;
    m_baseIndex = 0;
}

int CRTMPStream::addShow(CRTMPStreamShow* rtmpShow)
{   
	if(NULL == rtmpShow)
	{
		return 0;
	}

    QMutexLocker autoLock(&m_mutexShow);

	m_baseIndex++;
	m_mapShow.insert(RtmpStreamShowMap::value_type(m_baseIndex,rtmpShow));

	return m_baseIndex;
}

void CRTMPStream::delShow(int index)
{
    QMutexLocker autoLock(&m_mutexShow);
	if(m_mapShow.empty())
	{
		return;
	}

	RtmpStreamShowMap::iterator itr = m_mapShow.find(index);
	if(itr == m_mapShow.end())
	{
		return;
	}

	m_mapShow.erase(itr);
	return;
}

void CRTMPStream::show(const RtmpVideoBuf& videoBuf)
{
    QMutexLocker autoLock(&m_mutexShow);
	if(m_mapShow.empty())
	{
		return;
	}

	RtmpStreamShowMap::iterator itr;
	for(itr = m_mapShow.begin();itr != m_mapShow.end();itr++)
	{
		CRTMPStreamShow* rtmpShow = itr->second;
		if(rtmpShow)
		{
			rtmpShow->showRtmpVideoBuf(videoBuf);
		}		
	}

}

void CRTMPStream::show(int nIndex,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{    
    if(NULL == pData || 0 >= nSize || 0 >= nVideoW || 0 >= nVideoH)
    {
        return;
    }
    
    RtmpVideoBuf videoData(nIndex,pData,nSize,nVideoW,nVideoH);
    show(videoData); 
}

//////////////////////////////////////////////////////////////////////////
///
CRTMPStreamShow::CRTMPStreamShow():m_rtmpStream(NULL),m_index(0)
{

}

CRTMPStreamShow::~CRTMPStreamShow()
{
	if(NULL != m_rtmpStream)
	{
		m_rtmpStream->delShow(m_index);
	}
}

void CRTMPStreamShow::setRtmpStream(CRTMPStream* rtmpStream)
{
	if(NULL != m_rtmpStream)
	{
		m_rtmpStream->delShow(m_index);
	}

	m_rtmpStream = rtmpStream;
    if(NULL != m_rtmpStream)
	{
        m_index = m_rtmpStream->addShow(this);
    }
}

//////////////////////////////////////////////////////////////////////////
CRtmpStreamCallback::CRtmpStreamCallback()
{
	m_rtmpStream = NULL;
};

CRtmpStreamCallback::~CRtmpStreamCallback()
{
	if(m_rtmpStream)
	{
		m_rtmpStream->stop();
		m_rtmpStream = NULL;
	}
};

void CRtmpStreamCallback::ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{
	if(NULL == m_rtmpStream)
	{
		return;
	}

	int nVideo = (int)ShowHandle;        
	m_rtmpStream->show(nVideo,pData,nSize,nVideoW,nVideoH);
};

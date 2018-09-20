//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpStream.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.24
//	说明：rtmp流发布和播放封装基类
//  接口：
//	修改记录：
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

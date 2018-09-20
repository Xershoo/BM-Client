//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaInitThread.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.26
//	说明：初始化和反初始化多媒体DLL线程的实现
//  接口：
//	修改记录：
//**********************************************************************
#include "MediaInitThread.h"
#include "MediaPublishMgr.h"

//////////////////////////////////////////////////////////////////////////
//
CMediaHandle::CMediaHandle(CMediaPublishMgr* mgrMediaPublish):m_mgrMediaPublish(mgrMediaPublish)
{
	m_isInitMedia = false;
}

CMediaHandle::~CMediaHandle()
{
    disconnect(NULL,NULL,NULL);

	if(m_isInitMedia)
	{
		::UninitLibEnv();
	}

	m_isInitMedia = false;
}

void CMediaHandle::initMediaLibEnv()
{
	if(!m_isInitMedia)
	{
        m_isInitMedia = true;
		::InitLibEnv();
	}
	
	emit initFinish();
}

void CMediaHandle::startAllPublish()
{
	QMutexLocker AutoLock(&m_mutexStart);
	if(NULL == m_mgrMediaPublish)
	{
		return;
	}
	
	m_mgrMediaPublish->startAllSeatVideo();

	emit startFinish();
}

void CMediaHandle::startPublishSeat(int nIndex)
{
	QMutexLocker AutoLock(&m_mutexStart);
	if(NULL == m_mgrMediaPublish)
	{
		return;
	}

	m_mgrMediaPublish->startSeatVideo(nIndex);

	emit startFinish();
}

//////////////////////////////////////////////////////////////////////////
//
CMediaThread::CMediaThread(CMediaPublishMgr* mgrMediaPublish):m_mgrMediaPublish(mgrMediaPublish)
	,m_mediaHandle(NULL)
{
   
}

CMediaThread::~CMediaThread()
{
       
}

void CMediaThread::run()
{
	m_mediaHandle = new CMediaHandle(m_mgrMediaPublish);
	if(NULL == m_mediaHandle)
	{
		return;
	}
	
	connect(m_mediaHandle,SIGNAL(initFinish()),m_mgrMediaPublish,SLOT(mediaInitFinish()),Qt::AutoConnection);
	connect(m_mgrMediaPublish,SIGNAL(startAllPublishVideoAsync()),m_mediaHandle,SLOT(startAllPublish()),Qt::AutoConnection);
	connect(m_mgrMediaPublish,SIGNAL(startPublishSeatAsync(int)),m_mediaHandle,SLOT(startPublishSeat(int)),Qt::AutoConnection);

	m_mediaHandle->initMediaLibEnv();

    exec();

    delete m_mediaHandle;
    m_mediaHandle = NULL;

	return;
}

CMediaHandle* CMediaThread::getMediaHandle()
{
	return m_mediaHandle;
}

void CMediaThread::quit()
{
    QThread::quit();
    QThread::wait(-1);
}
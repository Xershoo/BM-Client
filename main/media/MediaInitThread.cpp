//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�MediaInitThread.cpp
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.26
//	˵������ʼ���ͷ���ʼ����ý��DLL�̵߳�ʵ��
//  �ӿڣ�
//	�޸ļ�¼��
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
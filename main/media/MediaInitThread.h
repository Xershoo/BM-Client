//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�MediaInitThread.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.26
//	˵������ʼ����ý��DLL�̵߳ķ�װ�Ķ���
//  �ӿڣ�
//	�޸ļ�¼��
//**********************************************************************

#ifndef MEDIA_INIT_THREAD_H
#define MEDIA_INIT_THREAD_H

#include <QtCore/QThread>
#include <QtGUI/qwindowdefs_win.h>
#include "../media/publish/libPublish/include/PublishInterface.h"

class CMediaPublishMgr;

class CMediaHandle : public QObject
{
	Q_OBJECT
public:
	explicit CMediaHandle(CMediaPublishMgr* mgrMediaPublish);
	virtual ~CMediaHandle();
   
public slots:
	void  initMediaLibEnv();
	void  startAllPublish();
	void  startPublishSeat(int nIndex);

signals:
	void  initFinish();
	void  startFinish();

protected:
	QMutex		m_mutexStart;
	bool		m_isInitMedia;

	CMediaPublishMgr*  m_mgrMediaPublish;
};

class CMediaThread : public QThread
{
    Q_OBJECT
public:
    explicit CMediaThread(CMediaPublishMgr* mgrMediaPublish);
    virtual ~CMediaThread();
    
	CMediaHandle* getMediaHandle();

    void quit();
protected:
    virtual void run();

protected:
    CMediaHandle*		m_mediaHandle;
	CMediaPublishMgr*	m_mgrMediaPublish;
};

#endif
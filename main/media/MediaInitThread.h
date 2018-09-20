//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaInitThread.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.26
//	说明：初始化多媒体DLL线程的封装的定义
//  接口：
//	修改记录：
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
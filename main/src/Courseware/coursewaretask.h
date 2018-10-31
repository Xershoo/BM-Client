//**********************************************************************
//	Copyright （c） 2015-2020. All rights reserved.
//	文件名称：coursewaretask.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：课件下载任务
//	修改记录：
//  备注：
//**********************************************************************
#pragma once

#include <QObject>
#include <QMutex>
#include <QVariant>
#include "../varname.h"
#include "courseware.h"
#include "CL_thread.h"

enum TaskEventType
{
    TASK_EVENT_TYPE_UPLOAD = 0, 
    TASK_EVENT_TYPE_DOWN,
    TASK_EVENT_TYPE_AGAIN,
};

//任务（上传、下载、重新执行）回调函数
typedef void (*scTaskCallBack)(int nEventType, void *lpUser, int nState, void *Param, int nError);
//任务（进度、获取下载文件的大小）回调函数
typedef void (*scTaskProCallBack)(void *lpUser, unsigned int nCoursewareID, int nPerc, long long nFileSize, void *Param);

class CoursewareTask : public QObject
{
    Q_OBJECT

public:    
	CoursewareTask(PCOURSEWAREDATA pData,QObject *parent = NULL);
    virtual ~CoursewareTask();

public:
    bool StartUploadTask();
    bool StopUploadTask();

    bool StartDownTask();
    bool StopDownTask();

    void SetTaskCallBack(scTaskCallBack pTaskFunc, scTaskProCallBack pTaskProFunc, void* lpTaskUser);

    inline bool isStop()
    {
        return m_stop;
    };

protected:
    static void UploadFunc(void *pUser);
    bool ExecUpload();
    bool UploadFile(LPCWSTR pszFileName, LPCWSTR pszFilePath, const char * pszToken);


    static void DownFunc(void *pUser);
    bool ExecDown();

    static void TransMediaOverCallBack(EventInfoType enmuType,void* dwUser,unsigned char* pBuf,unsigned int nBufSize,void* Param);

    bool SetPos(int nPerc);

    bool TransMediaOver();

    void HttpEventBengin(unsigned int nHttpID, int nState);
    void HttpEventProgress(unsigned int nHttpID, int nPos, bool bisFirst = false, long long nFileSize = 0);
    void HttpEventEnd(LPVOID pHttpData);

	void stopHttpSession();
protected slots:
    void doFinished();
	void HttpEventCallBack(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param);

private:
    CL_Thread       m_UploadThread;    
    CL_Thread       m_DownThread;
    
    COURSEWAREDATA  m_data;
    bool            m_bISLocalTrans;

    scTaskCallBack      m_TaskFunc;
    scTaskProCallBack   m_TaskProFunc;
    void*				m_pTaskUser;

    bool    m_stop;    //是否退出线程
	bool	m_doHttpEvent;
};


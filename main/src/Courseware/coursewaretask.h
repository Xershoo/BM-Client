//**********************************************************************
//	Copyright ��c�� 2015-2020. All rights reserved.
//	�ļ����ƣ�coursewaretask.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.2
//	˵�����μ���������
//	�޸ļ�¼��
//  ��ע��
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

//�����ϴ������ء�����ִ�У��ص�����
typedef void (*scTaskCallBack)(int nEventType, void *lpUser, int nState, void *Param, int nError);
//���񣨽��ȡ���ȡ�����ļ��Ĵ�С���ص�����
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

    bool    m_stop;    //�Ƿ��˳��߳�
	bool	m_doHttpEvent;
};


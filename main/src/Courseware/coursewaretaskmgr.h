//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：coursewaredata.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：课件管理
//	修改记录：
//  备注：
//**********************************************************************
#ifndef COURSEWAREDATA_H
#define COURSEWAREDATA_H

#include <QObject>
#include <QMutex>
#include <vector>
#include <QVariant>
#include "../varname.h"
#include "courseware.h"

using namespace std;


typedef struct tagTaskMgr 
{
    tagTaskMgr()
    {
        memset(this, NULL, sizeof(tagTaskMgr));
    }

    ~tagTaskMgr()
    {
        if (m_pData)
        {
            //delete m_pData;
            //m_pData = NULL;
        }
    }
    int     m_nCoursewareID;
    int     m_nStateTask;
    void    *m_pData;      
}TASKMGR, *PTASKMGR;

typedef vector<PTASKMGR> TASKMgrVector;
typedef vector<int> WaitTaskIDVector;

class CoursewareTaskMgr : public QObject
{
    Q_OBJECT

public:
    CoursewareTaskMgr(QObject *parent);
    ~CoursewareTaskMgr();

public:
    static CoursewareTaskMgr *CoursewareTaskMgr::getInstance();
    static void freeInstance();
    static bool isValidate();

public:
    void InitData();
    void UninitData();

    bool StartTimerTask();
    bool EndTimerTask();

    bool AddCourseBySource(LPCWSTR pszSourceFile, int *nError);
    bool AddCourseByNet(LPCWSTR pszNetFile, int *nError);
    bool AddCourseBySky(LPCWSTR pszFileName, LPCWSTR pszFielUrl, int *nError);

    bool DeleteCourseByName(LPCWSTR pszName);

    bool OpenCourseware(LPCWSTR pszFilePath);
    bool CloseCourseware(LPCWSTR pszFileName,bool sendMsg = true);

    void SetFileOpen(int nCoursewareID, bool bISOpen);
    
    PCOURSEWAREDATA GetCoursewareByIdEx(int nCoursewareID);
    PCOURSEWAREDATA GetCoursewareByNameEx(LPCWSTR pszName);
    PCOURSEWAREDATA GetCoursewareByFilePathEx(LPCWSTR pszFilePath);
    PCOURSEWAREDATA GetCoursewareByNetUrlEx(LPCWSTR pszNetUrl);

    void ClearAll();

    int  GetCoursewareCount();
    int  GetWaitDownFileSize();
    void SetAllNotOpen();

	PTASKMGR GetTask(int nCoursewareID);
signals:
    void del_courseware(QString);
    void add_courseware(QString, long long, int);
    void deal_courseware(QString, int, int);
    void set_pos(QString, int);
    void set_filesize(QString, long long);
    void mainshowchanged(int, int, int);
protected:
    PCOURSEWAREDATA GetCoursewareByName(LPCWSTR pszName);
    PCOURSEWAREDATA GetCoursewareByFilePath(LPCWSTR pszFilePath);
    PCOURSEWAREDATA GetCoursewareByHttp(int nHttpID);
    PCOURSEWAREDATA GetCoursewareByID(int nCoursewareID);

    bool GetFileNameByNetUrl(LPCWSTR pszOldUrl, LPWSTR pszNewUrl, LPWSTR pszName);
    bool GetFileTransName(LPCWSTR pszFilePath, LPWSTR pszTransName);

    bool DeleteTask(int nCoursewareID,bool stopTask = true);
    bool DeleteTask(PTASKMGR pTaskInfo,bool stopTask = true);
    void DeleteData(int nCoursewareID);

    void timerEvent(QTimerEvent *event); 
    bool doUploadTimer();
    bool doDownTimer();

	bool CloseCourseware(int nIndex,bool sendMsg = true);

    static void TaskEventCallBack(int nEventType, void *lpUser, int nState, void *Param, int nError);
    static void TaskProCallBack(void *lpUser, unsigned int nCoursewareID, int nPerc, long long nFileSize, void *Param);

private:
    void UploadEvent(int nCoursewareID, int nState, int nError);
    void DownEvent(int nCoursewareID, int nState, int nError);
    void AgainEvent(int nCoursewareID);
    void SetPos(int nCoursewareID, int nPrec);
    void SetFilesize(int nCoursewareID, long long nFileSize);

private:
	void clearAllTask();
	void addTask(PCOURSEWAREDATA pData,int state);

private:
    QMutex              m_DataMutex;
    CoursewareVector    m_vecCoursewareData;
    WaitTaskIDVector    m_vecWaitDownTask;
    WaitTaskIDVector    m_vecWaitUploadTask;

    QMutex              m_taskMutex;
    TASKMgrVector       m_vecMgrTask;

    static unsigned int        m_nCoursewareID;
    
    int     m_nMgrMax;
    int     m_nTimerID;

private:
    static CoursewareTaskMgr* m_instance;
};

#endif // COURSEWAREDATA_H

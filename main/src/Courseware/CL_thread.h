#pragma once

#include <QThread>
#include <QEvent>
#include <QtCore/QCoreApplication>

class CL_Event;

typedef void (*cbThreadFunc)(void *pUser);
typedef void (*cbThreadFuncEx)(void *pUser, CL_Event *e);

class CL_Thread : public QThread
{
    Q_OBJECT
public:
    CL_Thread();

    virtual ~CL_Thread();

    bool startthread(cbThreadFunc pFunc, void *pUser);
    bool stopthread();

protected:
    virtual void run();

protected:
    cbThreadFunc m_pFunc;
    void * m_pUser;
};

class CL_Event : public QEvent
{
public:
    //CL_Event(Type nType) : QEvent(nType) {}
    CL_Event(Type nType, long lParam, long wParam) : QEvent(nType),
        m_lParam(lParam),
        m_wParam(wParam)
    {
        
    }

public:
    long    m_lParam;
    long    m_wParam;
};
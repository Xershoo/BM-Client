#pragma once

#include <QThread>

typedef void (*cbHttpHeadThreadFunc)(void *pUser, QEvent *e);
typedef void (*cbHttpThreadFunc)(void *pUser);

class CL_Http_Head_Thread : public QThread
{
    Q_OBJECT
public:
    CL_Http_Head_Thread();

    ~CL_Http_Head_Thread();

    bool startthread(cbHttpHeadThreadFunc pFunc, void *pUser);
    bool stopthread();

private:
    void run();
    void customEvent(QEvent * e);

protected:
    void         *m_pUser;
    cbHttpHeadThreadFunc m_pFunc;
};

class CL_Http_Thread : public QThread
{
    Q_OBJECT
public:
    CL_Http_Thread();

    bool startthread(cbHttpThreadFunc pFunc, void *pUser);
    bool stopthread();
protected:


private:
    void run();

protected:
    void    *m_pUser;    
    cbHttpThreadFunc m_pFunc;

};
#include "CL_Http_thread.h"
#include <QDebug>

CL_Http_Head_Thread::CL_Http_Head_Thread()
{
    m_pFunc = NULL;
    m_pUser = NULL;
}

CL_Http_Head_Thread::~CL_Http_Head_Thread()
{

}

bool CL_Http_Head_Thread::startthread(cbHttpHeadThreadFunc pFunc, void *pUser)
{
    if (pFunc && pUser)
    {
        m_pFunc = pFunc;
        m_pUser = pUser;
    }
    start();
    return true;
}

bool CL_Http_Head_Thread::stopthread()
{
    exit();
    wait();
    return true;
}

void CL_Http_Head_Thread::run()
{
    exec();
}

void CL_Http_Head_Thread::customEvent(QEvent * e)
{
    if (NULL == e)
    {
        return;
    }
    if (m_pFunc && m_pUser)
    {
        m_pFunc(m_pUser, e);
    }	
}

CL_Http_Thread::CL_Http_Thread()
{   
    m_pFunc = NULL;
    m_pUser = NULL;
}

bool CL_Http_Thread::startthread(cbHttpThreadFunc pFunc, void *pUser)
{
    if (pFunc && pUser)
    {
        m_pFunc = pFunc;
        m_pUser = pUser;
        
        start();
        return true;
    }
    return false;
}

bool CL_Http_Thread::stopthread()
{
    m_pFunc = NULL;
    m_pUser = NULL;
    exit();

    return true;
}

void CL_Http_Thread::run()
{
    if (m_pFunc && m_pUser)
    {
        m_pFunc(m_pUser);
    }
}
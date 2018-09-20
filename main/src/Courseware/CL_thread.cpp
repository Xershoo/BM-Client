#include "CL_thread.h"
#include <QDebug>

CL_Thread::CL_Thread()
{
}

CL_Thread::~CL_Thread()
{

}

bool CL_Thread::startthread(cbThreadFunc pFunc, void *pUser)
{
    if (pFunc && pUser)
    {
        m_pFunc = pFunc;
        m_pUser = pUser;
    }
    start();
    return true;
}

bool CL_Thread::stopthread()
{
    exit();
        
    return true;
}

void CL_Thread::run()
{
    if (m_pFunc && m_pUser)
    {
        m_pFunc(m_pUser);
    }
}
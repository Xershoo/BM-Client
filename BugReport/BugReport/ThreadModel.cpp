// -------------------------------------------------------------------------
//	文件名		：	HttpDownloadModel.h
//	创建者		：	yuyu
//	创建时间	：	2004-2-13 10:00:00
//	功能描述	：	CHttpDownload使用的线程模型
//			
//					下载线程的同步处理保证
//
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ThreadModel.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


UINT __stdcall CThreadModel::__ThreadProxyProc(LPVOID pParam)
{
	CThreadModel *pDownload = reinterpret_cast<CThreadModel *> (pParam);
	HANDLE hEvtThreadOver = pDownload->m_hEvtThreadOver;

	pDownload->BeforeThread();
	UINT nResult =  pDownload->Run();
	pDownload->AfterThread();
		
	VERIFY( ::SetEvent(hEvtThreadOver) );

	return nResult;
}



CThreadModel::CThreadModel():
	m_bLowCpuPriority(TRUE),
	m_hThread(NULL)
{
	 
	m_hEvtThreadStart		=	::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hEvtThreadOver		=	::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hEvtUserAbort			=	::CreateEvent(NULL,TRUE,FALSE,NULL);

	AddEvent(m_hEvtUserAbort);
}


CThreadModel::~CThreadModel()
{
	StopThread();
	::CloseHandle(m_hEvtThreadStart); 
	::CloseHandle(m_hEvtThreadOver); 

	//::CloseHandle(m_hEvtUserAbort);

	for(int i = 0; i < m_arrayhEvt.GetSize();  i ++)
	{
		::CloseHandle(m_arrayhEvt.GetAt(i));
 	}	
}



BOOL CThreadModel::StartThread()
{
	if (m_hThread != NULL) 
		return FALSE;
			
	::ResetEvent(m_hEvtUserAbort);
	::ResetEvent(m_hEvtThreadOver);

	//
	// Create Download Thread
	////////////
	/*		！！ERROR!
	 *	  用AfxBeginThread会出现问题. 问题在于不知道如何控制删除WinThread的时间
	 *		可能出现CThread 删除拉WinThread对象， 而AfxbeginThread线程结束之后还有一些东西要做
	 */ 
	// Modified by yuyu 2004.3.3
/*
	if (m_bLowCpuPriority)
		AfxBeginThread(__ThreadProxyProc, (void *)this, THREAD_PRIORITY_IDLE);
	else
		AfxBeginThread(__ThreadProxyProc, (void *)this);*/
		

	UINT nThreadID = 0;
	int nVal = _beginthreadex(NULL, 0, __ThreadProxyProc, (void *)this, 0, &nThreadID);
	if(nVal <= 0)	
	{
		return FALSE;
	}
	
	m_hThread = reinterpret_cast<HANDLE>(nVal);
	if (m_bLowCpuPriority)
		SetThreadPriority(m_hThread, THREAD_PRIORITY_IDLE);

	//ResumeThread(m_hThread);


	// wait thread start
	
	DWORD nWait = ::WaitForSingleObject(m_hEvtThreadStart, INFINITE);

	::ResetEvent(m_hEvtThreadStart);

	if (nWait != WAIT_OBJECT_0)
 		return FALSE;
	
	return TRUE;
}


void CThreadModel::StopThread(DWORD nTimeOut/* = 5000*/)
{	
	if (m_hThread != NULL)
	{			
		// try to Stop thread
		::SetEvent(m_hEvtUserAbort);
		// default wait 5 secs
		if (::WaitForSingleObject(m_hEvtThreadOver, nTimeOut) == WAIT_OBJECT_0)
		{
			// ok . 			
			::ResetEvent(m_hEvtThreadOver);
		}
		else  // kill thread
		{						
			::TerminateThread(m_hThread, 1);
			m_hThread = NULL;
		}		
	}
	else
		::ResetEvent(m_hEvtThreadOver);
}

// add 2004.3.3 by yuyu
BOOL CThreadModel::WaitThread(DWORD nTimeOut /*= INFINITE*/)
{
	// working 
	//
	///////////////
	
	if (NULL == m_hThread)
		return FALSE;

	BOOL result = (WaitForSingleObject(m_hEvtThreadOver, nTimeOut) == WAIT_OBJECT_0) ?  
	//bool result = (WaitForSingleObject(reinterpret_cast<HANDLE>(m_hThread), nTimeOut) == WAIT_OBJECT_0) ?  
		true : false;

	if (!result)
	{
	
		::TerminateThread(m_hThread, 1);  
		
		// add 2004.3.3 by yuyu
		m_hThread = NULL;

	}
	
	return result;	
}

void CThreadModel::BeforeThread()
{
//	m_hThread = ::GetCurrentThread();

	//now start
	::SetEvent(m_hEvtThreadStart);
}


void CThreadModel::AfterThread()
{

	::CloseHandle(m_hThread);
	
 	m_hThread = NULL;

	//::SetEvent(m_hEvtThreadOver);
}


void CThreadModel::AddEvent(HANDLE hEvt)
{
	// 只限于线程启动前使用。
	// 如果允许线程内使用， 需要添加m_arrayhEvt临界区处理
	ASSERT(m_hThread == NULL);

	m_arrayhEvt.Add(hEvt);
}


BOOL CThreadModel::QueryEvent(HANDLE &hHandle, BOOL bAutoReset /* = TRUE */)
{
	int nSize = m_arrayhEvt.GetSize();
	hHandle = NULL;

	DWORD dwWake = ::WaitForMultipleObjects(nSize, m_arrayhEvt.GetData(), FALSE, INFINITE);

	if ((dwWake >= WAIT_OBJECT_0) && (dwWake < WAIT_OBJECT_0 + nSize))
	{
		hHandle = m_arrayhEvt[dwWake - WAIT_OBJECT_0];
		if (bAutoReset == TRUE)
			::ResetEvent(hHandle);
		
		if (hHandle == m_hEvtUserAbort)
			return TRUE;
	}

	return FALSE;
}

BOOL CThreadModel::QueryExitEvent()
{
	DWORD dwWake = ::WaitForSingleObject(m_hEvtUserAbort, 0);

	if (dwWake == WAIT_OBJECT_0)
	{
		::ResetEvent(m_hEvtUserAbort);
		
		return TRUE;
	}

	return FALSE;
}
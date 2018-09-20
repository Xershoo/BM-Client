   // -------------------------------------------------------------------------
//	文件名		：	HttpDownloadUIProxy.cpp
//	创建者		：	yuyu
//	创建时间	：	2004-2-13 10:00:00
//	功能描述	：	管理所有下载的UI界面
//			
//					本类使用Singleton模式， 当前只有一个实例
//
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "HttpDownloadUIProxy.h"
#include "HttpDownloadUIInterface.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHttpDownloadUIProxy


CHttpDownloadUIProxy::CHttpDownloadUIProxy():
	m_hEvtUIExit(NULL)
{	
}


CHttpDownloadUIProxy::~CHttpDownloadUIProxy()
{
	DeleteHeap();
}


BEGIN_MESSAGE_MAP(CHttpDownloadUIProxy, CWnd)
	//{{AFX_MSG_MAP(CHttpDownloadUIProxy)
	ON_MESSAGE(WM_DOWNLOAD_START, OnDownloadStart)
	ON_MESSAGE(WM_DOWNLOAD_CONNECTING, OnConnecting)
	ON_MESSAGE(WM_DOWNLOAD_CONNECTED, OnConnected)
	ON_MESSAGE(WM_DOWNLOAD_COMPLETE, OnDownloadComplete)
	ON_MESSAGE(WM_DOWNLOAD_PROGRESS, OnProgress)
	ON_MESSAGE(WM_DOWNLOAD_ERROR, OnError)
	ON_MESSAGE(WM_DOWNLOAD_EXITINSTANCE, OnExitInstance)
	ON_MESSAGE(WM_DOWNLOAD_REDIRECTED, OnRedirected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHttpDownloadUIProxy message handlers


CCriticalSection		CHttpDownloadUIProxy::m_nInstanceCS;

CHttpDownloadUIProxy *	CHttpDownloadUIProxy::m_pInstance		= NULL;

long					CHttpDownloadUIProxy::m_nInstanceCounts = 0;

HANDLE					CHttpDownloadUIProxy::m_hProcessHeap = NULL;

CHttpDownloadUIProxy *CHttpDownloadUIProxy::GetInstance()
{
	CSingleLock cs(&m_nInstanceCS);
	cs.Lock();
	if (cs.IsLocked())
	{	

	// when counts == 0, create the instance
	if (m_pInstance == NULL)
	{
		ASSERT(m_nInstanceCounts == 0);

		m_pInstance = new CHttpDownloadUIProxy;

		// init object.
		if (!m_pInstance->InitInstance())
		{
			delete m_pInstance;
			m_pInstance = NULL;
			return m_pInstance;
		}
	}
	m_nInstanceCounts ++;	

	cs.Unlock();
	}
	
	return m_pInstance;
}


void CHttpDownloadUIProxy::ExitInstance()
{
	CSingleLock cs(&m_nInstanceCS);
	cs.Lock();
	if (cs.IsLocked())
	{	

	ASSERT(m_nInstanceCounts > 0);

	m_nInstanceCounts --;
	// when counts == 0, delete the instance
	if (m_nInstanceCounts == 0)
	{
		TRACE("void CHttpDownloadUIProxy::ExitInstance() post exit\n");
//		m_pInstance->PostMessage(WM_CLOSE);

		//ClearAllMessage();

		m_pInstance->DestroyWindow();
		//delete m_pInstance; // 因特殊原因，此处内存泄漏请忽略
		m_pInstance = NULL;
	}

	cs.Unlock();	
	}
}

void CHttpDownloadUIProxy::ClearAllMessage()
{
	HWND hwnd = m_pInstance->GetSafeHwnd();

	if (hwnd == NULL)	return;

        MSG		msg; 

		MSG		msgClose;
		ZeroMemory(&msgClose, sizeof(MSG));
		BOOL	bCloseMsg = FALSE;

		// by yuyu ;)
		//
        // Read all of the messages in the window, 
        // Removing each message as we read it.
        while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) 
        { 
            // If it's a close message, we remember it and will handle it at last
            if (msg.message == WM_CLOSE)  
			{
				bCloseMsg = TRUE;
				msgClose = msg;
                continue;
			}
            // Otherwise, dispatch the message.
            DispatchMessage(&msg); 
        } // End of PeekMessage while loop.

		// Dispatch the close message.
		if (bCloseMsg)
			DispatchMessage(&msgClose);

}


BOOL CHttpDownloadUIProxy::InitInstance()
{
	if (!Create())
		return FALSE;

	if (!CreateHeap())
		return FALSE;

	return TRUE;
}


BOOL CHttpDownloadUIProxy::Create()
{
	if(!CWnd::CreateEx(WS_EX_TOOLWINDOW, AfxRegisterWndClass(0) ,NULL,WS_POPUP,CRect(0,0,1,1), NULL,0))
		return FALSE;

	return TRUE;
}


void CHttpDownloadUIProxy::PostNcDestroy() 
{
	TRACE("void CHttpDownloadUIProxy::PostNcDestroy()\n ");
	CWnd::PostNcDestroy();
	delete this;	
}


LRESULT CHttpDownloadUIProxy::OnExitInstance(WPARAM wParam, LPARAM lParam)
{
	// remove ui interface first
	RemoveUIInterface(wParam);

	//::SetEvent(m_hEvtUIExit);
	//::ReplyMessage(TRUE);
	ExitInstance();
	return 0;
}


LRESULT CHttpDownloadUIProxy::OnConnecting(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;

	CHttpDownloadUIInterface	*pUIInterface = NULL;
	pUIInterface = GetUIInterface(wParam);

	if (pUIInterface)
	{
		pUIInterface->OnConnecting((LPCTSTR)pMem);
	}

	FreeHeap(pMem);
	return 0;
}


LRESULT CHttpDownloadUIProxy::OnConnected(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;
	
	CHttpDownloadUIInterface	*pUIInterface = NULL;
	pUIInterface = GetUIInterface(wParam);

	TRACE("OnConnected: %x\n", pMem);

	if (pUIInterface)
	{		
		pUIInterface->OnConnected();
	}
	FreeHeap(pMem);

	return 0;
}


LRESULT CHttpDownloadUIProxy::OnDownloadStart(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;
	
	CHttpDownloadUIInterface	*pUIInterface = NULL;
	pUIInterface = GetUIInterface(wParam);
	
	if (pUIInterface)
	{
		DWORD *pBuffer ;
		pBuffer = (DWORD *)pMem;
		pUIInterface->OnDownloadStart(pBuffer[0], pBuffer[1]);
	}
	
	FreeHeap(pMem);
	return 0;
}

LRESULT CHttpDownloadUIProxy::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;
	CHttpDownloadUIInterface *pUIInterface = GetUIInterface(wParam);
	if(pUIInterface)
	{
		DWORD nErrorCode = *(DWORD *)pMem;
		BYTE * pSavePath = (BYTE *)pMem + sizeof(DWORD);
		
		//int nLen = strlen((LPCTSTR)pSavePath) + 1;
		//BYTE * pResponseFile = (BYTE *)pMem + sizeof(DWORD) + nLen;

		pUIInterface->OnDownloadComplete(nErrorCode, (LPCTSTR)pSavePath/*, (LPCTSTR)pResponseFile*/);
	}
	TRACE("void CHttpDownloadUIProxy::OnDownloadComplete(WPARAM wParam, LPARAM lParam)\n");
	FreeHeap(pMem);	
	return 0;
}

LRESULT CHttpDownloadUIProxy::OnRedirected(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;

	CHttpDownloadUIInterface	*pUIInterface = NULL;
	pUIInterface = GetUIInterface(wParam);

	if (pUIInterface)
	{
		pUIInterface->OnRedirected((LPCTSTR)pMem);
	}
	FreeHeap(pMem);

	return 0;
}

LRESULT CHttpDownloadUIProxy::OnError(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;
	
	CHttpDownloadUIInterface	*pUIInterface = NULL;
	pUIInterface = GetUIInterface(wParam);
	
	if (pUIInterface)
	{
		int nCode = *(int *)(lParam);
		////modify by maisong at 2008.2.28
		//pUIInterface->OnError(nCode);
	}
	
	FreeHeap(pMem);	
	return 0;
}


LRESULT CHttpDownloadUIProxy::OnProgress(WPARAM wParam, LPARAM lParam)
{
	LPVOID pMem = (LPVOID) lParam;
	CHttpDownloadUIInterface	*pUIInterface = NULL;
	pUIInterface = GetUIInterface(wParam);
	if (pUIInterface)
	{
		DWORD *pBuffer = (DWORD*)pMem;
		LPCTSTR lpszSave = (LPCTSTR)((BYTE *)pMem + sizeof(DWORD) * 2);
		////modify by maisong at 2008.2.28
		pUIInterface->OnProgress(pBuffer[0],pBuffer[1],lpszSave);
		//pUIInterface->OnProgressEx(pBuffer[0],pBuffer[1],lpszSave);
	}
	FreeHeap(pMem);
	return 0;
}


BOOL CHttpDownloadUIProxy::CreateHeap()
{
	m_hProcessHeap = ::GetProcessHeap();
	//m_hProcessHeap= HeapCreate()
	return (m_hProcessHeap != NULL);
}


void CHttpDownloadUIProxy::DeleteHeap()
{
	
}


void CHttpDownloadUIProxy::FreeHeap(LPVOID pMem)
{
	ASSERT(m_hProcessHeap != NULL);
	
	if (pMem)
		::HeapFree(m_hProcessHeap, 0, pMem);

}


HANDLE CHttpDownloadUIProxy::GetHeap()
{
	ASSERT(m_hProcessHeap != NULL);

	return m_hProcessHeap;
}


CHttpDownloadUIInterface *CHttpDownloadUIProxy::GetUIInterface(DWORD dwKey)
{
	CHttpDownloadUIInterface *pTemp = NULL;
	m_mapUIInterface.Lookup(dwKey, pTemp);

	return pTemp;
}


void CHttpDownloadUIProxy::SetUIInterface(DWORD dwKey, CHttpDownloadUIInterface *pUIInterface)
{
	//m_pUIInterface = pUIInterface;
	ASSERT(pUIInterface != NULL);

	// if debug version, check if it's repeated key

	// 这里有个bug
	// 线程退出后, 只是postmessage过来. message可能未处理, 也就是可能还没有RemoveUIInterface
	// 这时候恰好新的UIInterface的key和旧的一样, 这里就出现了重复的key

	// 已经解决2004.2.16
	// 详细看CHttpdownload::DeleteUIProxy();
	
#ifdef _DEBUG
	CHttpDownloadUIInterface *pTemp = NULL;
	ASSERT(m_mapUIInterface.Lookup(dwKey, pTemp) == FALSE);
#endif 

	m_mapUIInterface.SetAt(dwKey, pUIInterface);
}


void CHttpDownloadUIProxy::RemoveUIInterface(DWORD dwKey)
{
	m_mapUIInterface.RemoveKey(dwKey);
}


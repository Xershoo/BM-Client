// -------------------------------------------------------------------------
//	文件名		：	HttpDownloadUIProxy.h
//	创建者		：	yuyu
//	创建时间	：	2004-2-13 10:00:00
//	功能描述	：	管理所有下载的UI界面
//			
//					本类使用Singleton模式， 当前只有一个实例
//
//-------------------------------------------------------------------------


#if !defined(AFX_HTTPDOWNLOADUIPROXY_H__859C9C10_5A0C_4416_9CE3_BF57AC2B6E76__INCLUDED_)
#define AFX_HTTPDOWNLOADUIPROXY_H__859C9C10_5A0C_4416_9CE3_BF57AC2B6E76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxTempl.h>
#include <afxmt.h>

#define WM_DOWNLOAD_START			WM_USER + 4000
#define WM_DOWNLOAD_PROGRESS		WM_DOWNLOAD_START + 1
#define WM_DOWNLOAD_COMPLETE		WM_DOWNLOAD_START + 2
#define WM_DOWNLOAD_CONNECTING		WM_DOWNLOAD_START + 3
#define WM_DOWNLOAD_CONNECTED		WM_DOWNLOAD_START + 4
#define WM_DOWNLOAD_EXITINSTANCE	WM_DOWNLOAD_START + 5
#define WM_DOWNLOAD_ERROR			WM_DOWNLOAD_START + 6
#define WM_DOWNLOAD_REDIRECTED		WM_DOWNLOAD_START + 7



class CHttpDownloadUIInterface;

/////////////////////////////////////////////////////////////////////////////
// CHttpDownloadUIProxy window
// 

class CHttpDownloadUIProxy : public CWnd
{
//#ifdef _DEBUG
//#ifdef new
//#define xnew
//#pragma push_macro ("new")
//#undef new
//#endif
//	void * operator new (size_t sz,const char * file,int line)
//	{
//		return ::operator new(sz,_IGNORE_BLOCK,file,line);
//	}
//#ifdef xnew
//#undef xnew
//#pragma pop_macro("new")
//#endif
//#endif
// 实现Singleton模式
private:

	// 引用锁
	static CCriticalSection			m_nInstanceCS;
	// 引用记数
	static long						m_nInstanceCounts;

	// 唯一的实例Instance
	static CHttpDownloadUIProxy *	m_pInstance;
		
public:
	// 获得Instance
	static CHttpDownloadUIProxy *	GetInstance();
	
	// 取消对Instance的引用
	static void						ExitInstance();
	

// 保护对象不被创建和删除
// protected Construction. disable create the object
	virtual ~CHttpDownloadUIProxy();
protected:
	CHttpDownloadUIProxy();
	
	// 初始化Instance, 失败则instance创建失败
	BOOL	InitInstance();

	static void	ClearAllMessage();
	
////////////////////////////////////////

//
protected:
	static HANDLE	m_hProcessHeap;
	HANDLE	m_hEvtUIExit;	

	CMap<DWORD, DWORD, CHttpDownloadUIInterface *, CHttpDownloadUIInterface *>
			m_mapUIInterface;

protected:
	BOOL	Create();

	BOOL	CreateHeap();
	void	DeleteHeap();
	void	FreeHeap(LPVOID pMem);	

	void						RemoveUIInterface(DWORD dwKey);
	CHttpDownloadUIInterface *	GetUIInterface(DWORD dwKey);
// 
public:	
	// 取得公用的Heap
	HANDLE	GetHeap();
	
	// 把CHttpDownload类作为key,对应CHttpDownloadUIInterface类
	void	SetUIInterface(DWORD dwKey, CHttpDownloadUIInterface *pUIInterface);

	void	SetUIExitEvent(HANDLE hEvtUIExit) { m_hEvtUIExit = hEvtUIExit ;}
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHttpDownloadUIProxy)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	

	// Generated message map functions
protected:
	//{{AFX_MSG(CHttpDownloadUIProxy)
	afx_msg LRESULT OnError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloadStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloadComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnecting(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExitInstance(WPARAM wParam, LPARAM lParam);		
	afx_msg LRESULT OnRedirected(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_HTTPDOWNLOADUIPROXY_H__859C9C10_5A0C_4416_9CE3_BF57AC2B6E76__INCLUDED_)

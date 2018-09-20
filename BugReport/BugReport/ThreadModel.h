   // -------------------------------------------------------------------------
//	文件名		：	HttpDownloadModel.h
//	创建者		：	yuyu
//	创建时间	：	2004-2-13 10:00:00
//	功能描述	：	CHttpDownload使用的线程模型
//			
//					下载线程的同步处理保证
//
//-------------------------------------------------------------------------

#if !defined(AFX_THREADMODEL_H__3BE8BB3E_3305_47A1_ACD3_76E949A82ED7__INCLUDED_)
#define AFX_THREADMODEL_H__3BE8BB3E_3305_47A1_ACD3_76E949A82ED7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
// HTTP下载线程模型类
class CThreadModel
{

	//friend UINT __GetFileThreadProc(LPVOID);

public:
	// 启动线程
	BOOL	StartThread();

	// 停止线程
	void	StopThread(DWORD nTimeOut = 5000);	

	// add 2004.3.3 by yuyu
	// 等待线程工作完毕。
	BOOL	WaitThread(DWORD nTimeOut = INFINITE);	

	// 虚拟函数
protected:	

	virtual void	BeforeThread();

	virtual void	AfterThread();	

	// 线程函数
	virtual UINT	Run() = 0;

protected:
	// 添加Query Event
	void	AddEvent(HANDLE hEvt);

	// 查询Event。包括线程退出事件和用户添加的定义事件
	//		bAutoRest = TRUE   自动Reset触发的Event
	//		返回True表示请求退出线程
	BOOL	QueryEvent(HANDLE &hHandle, BOOL bAutoReset = TRUE);
	
	// 只查询退出事件
	BOOL	QueryExitEvent();

	BOOL	IsThreadRunning() { return (m_hThread != 0); }

private:
	static UINT __stdcall __ThreadProxyProc(LPVOID pParam);

// protect Construction
protected:
	CThreadModel();
	virtual ~CThreadModel();

protected:
	HANDLE		m_hThread;	
	HANDLE		m_hEvtUserAbort;

private:

	BOOL		m_bLowCpuPriority; 
	HANDLE		m_hEvtThreadStart;
 	HANDLE		m_hEvtThreadOver;

	CArray<HANDLE, HANDLE> m_arrayhEvt;				// 事件数组 for WaitForMultipleObjects
};

#endif // !defined(AFX_THREADMODEL_H__3BE8BB3E_3305_47A1_ACD3_76E949A82ED7__INCLUDED_)

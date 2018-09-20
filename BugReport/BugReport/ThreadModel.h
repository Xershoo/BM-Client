   // -------------------------------------------------------------------------
//	�ļ���		��	HttpDownloadModel.h
//	������		��	yuyu
//	����ʱ��	��	2004-2-13 10:00:00
//	��������	��	CHttpDownloadʹ�õ��߳�ģ��
//			
//					�����̵߳�ͬ������֤
//
//-------------------------------------------------------------------------

#if !defined(AFX_THREADMODEL_H__3BE8BB3E_3305_47A1_ACD3_76E949A82ED7__INCLUDED_)
#define AFX_THREADMODEL_H__3BE8BB3E_3305_47A1_ACD3_76E949A82ED7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
// HTTP�����߳�ģ����
class CThreadModel
{

	//friend UINT __GetFileThreadProc(LPVOID);

public:
	// �����߳�
	BOOL	StartThread();

	// ֹͣ�߳�
	void	StopThread(DWORD nTimeOut = 5000);	

	// add 2004.3.3 by yuyu
	// �ȴ��̹߳�����ϡ�
	BOOL	WaitThread(DWORD nTimeOut = INFINITE);	

	// ���⺯��
protected:	

	virtual void	BeforeThread();

	virtual void	AfterThread();	

	// �̺߳���
	virtual UINT	Run() = 0;

protected:
	// ���Query Event
	void	AddEvent(HANDLE hEvt);

	// ��ѯEvent�������߳��˳��¼����û���ӵĶ����¼�
	//		bAutoRest = TRUE   �Զ�Reset������Event
	//		����True��ʾ�����˳��߳�
	BOOL	QueryEvent(HANDLE &hHandle, BOOL bAutoReset = TRUE);
	
	// ֻ��ѯ�˳��¼�
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

	CArray<HANDLE, HANDLE> m_arrayhEvt;				// �¼����� for WaitForMultipleObjects
};

#endif // !defined(AFX_THREADMODEL_H__3BE8BB3E_3305_47A1_ACD3_76E949A82ED7__INCLUDED_)

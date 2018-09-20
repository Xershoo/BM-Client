// ExtraTimer.h: interface for the CTimer class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(AFX_EXTRATIMER_H__8EBB9073_B9AB_461F_BDF2_D2B3C4683905__INCLUDED_)
#define AFX_EXTRATIMER_H__8EBB9073_B9AB_461F_BDF2_D2B3C4683905__INCLUDED_

#include "ITimer.h"
#include "CriticalSectionAuto.h"
#include "Mmsystem.h"
#include "macros.h"
#include "xxtimer.h"
#include <map>
#include <list>


#pragma comment(lib, "winmm.lib")

const UINT TM_PULSE = (WM_USER + 3053);
struct TimeSpan
{
	ITimerSenseObj* obj_;
	UINT timeSpan;//时间间隔
	int spareTime;//剩余的时间
};


typedef int timer_id;

namespace nsox{
	class nutimer;
}

struct ListenerWrapper;




class  CThreadTimer 
{
public:
	CThreadTimer();
	virtual ~CThreadTimer();
	BOOL CreateSynWindow();
	void SynPulse();

	void RegisterListener(ITimerSenseObj* listener,UINT timeSpan);
	void UnRegisterListener(ITimerSenseObj* listener);
public:
	void OnTimer();
	void Pulse();
	void EnsureEveryTick(BOOL ensure){ bEnsureEveryTick_ = ensure; }

protected:
	typedef std::map<ITimerSenseObj*,ListenerWrapper*> listener_map;
	listener_map __listners;
	nsox::xxtimer*	__timer;

	nsox::xxtimer*	__large_timer;

	int			__elapse;



	UINT millisecond_;
	BOOL bEnsureEveryTick_;
	BOOL bWaitingSyn_;
	BOOL bPulseing_;

	HWND hSynObjWnd_;

	BOOL bDeleted;
	DWORD threadID;
};





class   CTimer : public CThreadTimer
{
private:
	CTimer();
	virtual ~CTimer();
public:
	static CTimer* Instance();
	static void		Release();
	BOOL SetTick(UINT millisecond );
	void OnTimer(UINT timerID);



	UINT	timerID_;
	HANDLE	m_hCloseEvent;
};
class Nothing
{
public:
	Nothing()
	{
		CTimer::Instance()->SetTick(50);
	}

	~Nothing()
	{
		CTimer::Instance()->Release();
	}
};





#define LARGE_TIMER_SIZE 255


LRESULT CALLBACK  TimeWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

//该类目前不能用在多线程，多线程需要另外考虑

inline void DestroyTimer(void)
{
	CTimer::Release();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 定时器回调函数
inline void CALLBACK TimeProc(DWORD uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	CTimer::Instance()->OnTimer(uID);
}



struct ListenerWrapper : public nsox::xx_timer_handler
{
	ListenerWrapper(nsox::xxtimer* timer, ITimerSenseObj* listener, int timespan)
	{
		__listener = listener;
		__timespan = timespan;
		__timer	   = timer;
	}
	virtual void timeout()
	{		
		if(!__listener)
		{
			delete this;
			return;
		}

		if(__listener)
		{
			__listener->TimeArrive(); // 这个通知会删除本对象，NOTICE
			__timer->add_timeout(__timespan, this);
		}
	}
	ITimerSenseObj* __listener;
	int				__timespan;
	nsox::xxtimer*	__timer;
	timer_id		__timer_id;
};




inline CTimer::CTimer()
{

	static TCHAR szAppName[] = _T("TimeSynWindow") ;

	timerID_ = 0;


	WNDCLASS	wndclass ;

	wndclass.style		  = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc  = TimeWindowProc ;
	wndclass.cbClsExtra	  = 0 ;
	wndclass.cbWndExtra	  = 0 ;
	wndclass.hInstance	  = _AtlBaseModule.GetModuleInstance() ;
	wndclass.hIcon		  = NULL ;
	wndclass.hCursor	  = NULL;
	wndclass.hbrBackground	= NULL;
	wndclass.lpszMenuName	= NULL ;
	wndclass.lpszClassName	= szAppName ;

	RegisterClass(&wndclass);

	/*if (!RegisterClass (&wndclass))
	{
	MessageBox (	NULL, TEXT ("Timer SysObj Create Failed!"), 
	szAppName, MB_ICONERROR) ;
	}*/

	m_hCloseEvent					= CreateEvent(NULL, FALSE, FALSE, _T("timer close event"));

	threadID = GetCurrentThreadId();
	CreateSynWindow();


}
inline CTimer::~CTimer()
{
	if(timerID_ != 0)
	{
		timeKillEvent(timerID_);
		timerID_ = 0;
	}
	::WaitForSingleObject(m_hCloseEvent, 500);
	::CloseHandle(m_hCloseEvent);
}
inline void CTimer::OnTimer(UINT timerID)
{
	CThreadTimer::OnTimer();
	::SetEvent(m_hCloseEvent);
}
inline CTimer* CTimer::Instance()
{
	static CTimer timer;
	return &timer;
}

inline void CTimer::Release()
{
}
inline BOOL CTimer::SetTick(UINT millisecond )
{
	if(millisecond_ == millisecond)
	{
		return TRUE;
	}

	SAFE_DELETE(__timer);
	SAFE_DELETE(__large_timer);

	__timer = new nsox::xxtimer(millisecond);
	__large_timer = new nsox::xxtimer(LARGE_TIMER_SIZE*millisecond);


	millisecond_ = millisecond;
	if(timerID_ != 0)
	{ 
		timeKillEvent(timerID_);
		timerID_ = 0;
	}
	if(millisecond != 0)
	{
		timerID_ = timeSetEvent(millisecond_,millisecond_/10,(LPTIMECALLBACK)TimeProc,NULL,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
		AFFIRM_BOOL(timerID_ != 0);
	}
	return TRUE;
}

inline void CThreadTimer::RegisterListener(ITimerSenseObj* listener,UINT timespan)
{
	if(threadID != GetCurrentThreadId())
	{
		// diffrent thread, are you kidding?  guys!!!
		ASSERT(FALSE); return;
	}

	ListenerWrapper* new_lisn = NULL;
	if(timespan >= LARGE_TIMER_SIZE * millisecond_)
	{
		new_lisn = new ListenerWrapper(__large_timer, listener,timespan);
		new_lisn->__timer_id =  __large_timer->add_timeout(timespan,new_lisn);
	}else{
		new_lisn = new ListenerWrapper(__timer, listener,timespan);
		new_lisn->__timer_id =  __timer->add_timeout(timespan,new_lisn);		
	}

	listener_map::iterator itr = __listners.find(listener);
	if(itr !=  __listners.end())
	{
		itr->second->__listener = NULL;
	}
	__listners[listener] = new_lisn;
}
inline void CThreadTimer::UnRegisterListener(ITimerSenseObj* listener)
{
	listener_map::iterator itr = __listners.find(listener);
	if(itr != __listners.end())
	{	
		itr->second->__listener = NULL;
		__listners.erase(itr);
	}
}


/************************************************************************/
/* CThreamTimer                                                         */
/************************************************************************/
inline CThreadTimer::CThreadTimer()
{
	__timer = NULL;
	__large_timer = NULL;
	millisecond_ = 0;

	bEnsureEveryTick_ = FALSE;
	bWaitingSyn_ = FALSE;
	bPulseing_ = FALSE;
	hSynObjWnd_ = NULL;
	bDeleted = FALSE;

	__elapse = 0;


}
inline CThreadTimer::~CThreadTimer()
{
	__timer->notify_all();
	__large_timer->notify_all();

	SAFE_DELETE(__timer);
	SAFE_DELETE(__large_timer);

	if(hSynObjWnd_)
	{
		::DestroyWindow(hSynObjWnd_);
	}

	for(listener_map::iterator itr = __listners.begin();
		itr != __listners.end();
		++ itr)
	{
		delete itr->second;
	}
	hSynObjWnd_ = NULL;
}
inline void CThreadTimer::SynPulse()//时钟
{
	Pulse();
	bWaitingSyn_ = FALSE;
}
inline void CThreadTimer::Pulse()
{
	__timer->tickaway(millisecond_);
	__elapse = __large_timer->tickaway(__elapse + millisecond_);
}
inline void CThreadTimer::OnTimer()
{
	static int tickcount = 0; 
	tickcount ++; 
	if(hSynObjWnd_)
	{
		if(bEnsureEveryTick_)
		{
			::PostMessage(hSynObjWnd_,TM_PULSE,NULL,(LPARAM)threadID);
		}
		else if(!bWaitingSyn_ || tickcount >= 5)
		{
			::PostMessage(hSynObjWnd_,TM_PULSE,NULL,(LPARAM)threadID);
			bWaitingSyn_ = TRUE;
			tickcount = 0;
		}
	}
	else
	{
		Pulse();
	}
}
inline BOOL CThreadTimer::CreateSynWindow()
{
	static TCHAR szAppName[] = _T("TimeSynWindow") ;

	HINSTANCE instance = _AtlBaseModule.GetModuleInstance();
	hSynObjWnd_ = CreateWindow(szAppName,	// window class name
		NULL,	// window caption
		WS_POPUP,	// window style
		CW_USEDEFAULT,	// initial x position
		CW_USEDEFAULT,	// initial y position
		CW_USEDEFAULT,	// initial x size
		CW_USEDEFAULT,	// initial y size
		NULL,			// parent window handle
		NULL,	        // window menu handle
		instance,	    // program instance handle
		NULL) ; 	    // creation parameters
	ASSERT(hSynObjWnd_ != NULL);
	return hSynObjWnd_ != NULL;
}
inline static LRESULT CALLBACK  TimeWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if(uMsg == TM_PULSE)
	{
		CTimer::Instance()->SynPulse();
		return 0;
	}
	return DefWindowProc (hWnd, uMsg, wParam, lParam) ;
}

#endif // !defined(AFX_EXTRATIMER_H__8EBB9073_B9AB_461F_BDF2_D2B3C4683905__INCLUDED_)
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "func_binder.h"


// ���ܣ���һ���̵߳ĺ������ð�װ����ת�Ƶ���һ���̵߳���.
// ע�⣺��һ���̱߳�������Ϣѭ����������Ч.

namespace biz_utility
{
	class AsynTimer;


	class AnsyFuncDispcher
	{
	public:
		static const UINT WM_ANSY_TASK	= WM_USER + 2013;
		AnsyFuncDispcher();

		bool Startup();
		bool Cleanup();
		DWORD GetThreadID(){ return _threadID; }


		// 0 ����
		template <typename TRet, typename TC>
		bool PostTask(TC* pc, TRet (TC::*pfunc)())
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			::PostMessageA(_hwnd, WM_ANSY_TASK, (WPARAM)NewTask(pfunc, pc), NULL);
			return true;
		}


		// 1 ����
		template <typename TRet, typename TC, typename TArg>
		bool PostTask(TC* pc, TRet (TC::*pfunc)(TArg), TArg arg)
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			::PostMessageA(_hwnd, WM_ANSY_TASK, (WPARAM)NewTask(pfunc, pc, arg), NULL);
			return true;
		}


		// 2 ����
		template <typename TRet, typename TC, typename TArg1, typename TArg2>
		bool PostTask(TC* pc, TRet (TC::*pfunc)(TArg1, TArg2), TArg1 arg1, TArg2 arg2)
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			::PostMessageA(_hwnd, WM_ANSY_TASK, (WPARAM)NewTask(pfunc, pc, arg1, arg2), NULL);
			return true;
		}

		// 3 ����
		template <typename TRet, typename TC, typename TArg1, typename TArg2, typename TArg3>
		bool PostTask(TC* pc, TRet (TC::*pfunc)(TArg1, TArg2, TArg3), TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			::PostMessageA(_hwnd, WM_ANSY_TASK, (WPARAM)NewTask(pfunc, pc, arg1, arg2, arg3), NULL);
			return true;
		}

		// 4 ����
		template <typename TRet, typename TC, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
		bool PostTask(TC* pc, TRet (TC::*pfunc)(TArg1, TArg2, TArg3, TArg4), TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			::PostMessageA(_hwnd, WM_ANSY_TASK, (WPARAM)NewTask(pfunc, pc, arg1, arg2, arg3, arg4), NULL);
			return true;
		}


		bool ScheduleTimer(AsynTimer& timer, int period);
		void DeleteTimer(AsynTimer&);


	private:
		static bool RegisterClass();
		static LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
		static void CALLBACK TimerProc(HWND, UINT, UINT_PTR idEvent, DWORD);


	private:
		HWND	_hwnd;
		DWORD	_threadID;
		static const wchar_t* windowClassName;
	};


	///////////// Task /////////////////////////////////////////////////////////////
	class Task
	{
	public:
		virtual void Process() = 0;
		virtual ~Task(){};

		void RunDelete()
		{
			Process();
			delete this;
		}
	};


	template <typename TFunc>
	class TaskBinder : public Task
	{
	public:
		TaskBinder(const TFunc& f) : m_func(f){}
		virtual void Process(){ m_func(); }
		TFunc m_func;
	};


	template <typename TFunc>
	inline TaskBinder<TFunc>* NewTask(const TFunc& f)
	{
		return new TaskBinder<TFunc>(f);
	}


	//��0������Ա����
	template <typename TRet, typename TC>
	inline TaskBinder<mem_fun_binder_0<TRet, TC> >* NewTask(TRet (TC::*pfunc)(), TC* pc)
	{
		mem_fun_binder_0<TRet, TC> fun(pfunc, pc);
		return NewTask(fun);
	}


	//��1������Ա����
	template <typename TRet, typename TC, typename TArg>
	inline TaskBinder<mem_fun_binder_1<TRet, TC, TArg> >* NewTask(TRet (TC::*pfunc)(TArg), TC* pc, TArg arg)
	{
		mem_fun_binder_1<TRet, TC, TArg> fun(pfunc, pc, arg);
		return NewTask(fun);
	}


	//��2������Ա����
	template <typename TRet, typename TC, typename TArg1, typename TArg2>
	inline TaskBinder<mem_fun_binder_2<TRet, TC, TArg1, TArg2> >* NewTask(TRet (TC::*pfunc)(TArg1, TArg2), TC* pc, TArg1 arg1, TArg2 arg2)
	{
		mem_fun_binder_2<TRet, TC, TArg1, TArg2> fun(pfunc, pc, arg1, arg2);
		return NewTask(fun);
	}

	//��3������Ա����
	template <typename TRet, typename TC, typename TArg1, typename TArg2, typename TArg3>
	inline TaskBinder<mem_fun_binder_3<TRet, TC, TArg1, TArg2, TArg3> >* NewTask(TRet (TC::*pfunc)(TArg1, TArg2, TArg3), TC* pc, TArg1 arg1, TArg2 arg2, TArg3 arg3)
	{
		mem_fun_binder_3<TRet, TC, TArg1, TArg2, TArg3> fun(pfunc, pc, arg1, arg2, arg3);
		return NewTask(fun);
	}

	//��4������Ա����
	template <typename TRet, typename TC, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	inline TaskBinder<mem_fun_binder_4<TRet, TC, TArg1, TArg2, TArg3, TArg4> >* NewTask(TRet (TC::*pfunc)(TArg1, TArg2, TArg3, TArg4), TC* pc, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
	{
		mem_fun_binder_4<TRet, TC, TArg1, TArg2, TArg3, TArg4> fun(pfunc, pc, arg1, arg2, arg3, arg4);
		return NewTask(fun);
	}


	////////// Timer ////////////////////////////////////////////////////////////////
	class AsynTimer
	{
	public:
		AsynTimer(){ isruning = false; }
		virtual void Process() = 0;
		virtual ~AsynTimer(){}
		bool	isruning;
	};


	template <typename TFunc>
	class TimeBinder : public AsynTimer
	{
	public:
		TimeBinder(const TFunc& f) : m_func(f){}
		virtual void Process(){ m_func(); }
		TFunc m_func;
	};


	template <typename TFunc>
	inline TimeBinder<TFunc>* NewTimer(const TFunc& f)
	{
		return new TimeBinder<TFunc>(f);
	}


	template <typename TRet, typename TC>
	inline TimeBinder<mem_fun_binder_0<TRet, TC> >* NewTimer(TRet (TC::*pfunc)(), TC* pc)
	{
		mem_fun_binder_0<TRet, TC> fun(pfunc, pc);
		return NewTimer(fun);
	}
}
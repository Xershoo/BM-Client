#pragma once

#include <set>
#include <Windows.h>
#include "bizutility/func_dispatcher/func_binder.h"

#define MSVC_PUSH_DISABLE_WARNING(n) __pragma(warning(push)) \
	__pragma(warning(disable:n))

// Pop effects of innermost MSVC_PUSH_* macro.
#define MSVC_POP_WARNING() __pragma(warning(pop))

#define ALLOW_THIS_IN_INITIALIZER_LIST(code) MSVC_PUSH_DISABLE_WARNING(4355) \
	code \
	MSVC_POP_WARNING()


namespace gnet
{
	class Pipe;
	struct IOHandler;
	struct IOContext
	{
		OVERLAPPED overlapped;
		IOHandler* handler;
	};


	struct IOHandler
	{
		virtual ~IOHandler(){}
		virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered, DWORD error) = 0;
	};


	class IOTask : public IOHandler
	{
	public:
		virtual void Process() = 0;
		virtual ~IOTask(){};

		virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered, DWORD error)
		{
			Process();
			delete this;
		}

		IOContext	_context;
	};


	template <typename TFunc>
	class IOTaskT : public IOTask
	{
	public:
		IOTaskT(const TFunc& f) : _func(f){ _context.handler = this; }
		virtual void Process()
		{
			_func();
		}

		TFunc		_func;
	};

	template <typename TFunc>
	inline IOTaskT<TFunc>* NewIOTask(const TFunc& f)
	{
		return new IOTaskT<TFunc>(f);
	}


	//绑定0参数成员函数
	template <typename TRet, typename TC>
	inline IOTaskT<mem_fun_binder_0<TRet, TC> >* NewIOTask(TRet (TC::*pfunc)(), TC* pc)
	{
		mem_fun_binder_0<TRet, TC> fun(pfunc, pc);
		return NewIOTask(fun);
	}


	//绑定1参数成员函数
	template <typename TRet, typename TC, typename TArg>
	inline IOTaskT<mem_fun_binder_1<TRet, TC, TArg> >* NewIOTask(TRet (TC::*pfunc)(TArg), TC* pc, TArg arg)
	{
		mem_fun_binder_1<TRet, TC, TArg> fun(pfunc, pc, arg);
		return NewIOTask(fun);
	}


	class IOThread : public IOHandler
	{
	public:
		static IOThread* GetInst();
		bool Start();
		void RegisterIOHandler(HANDLE file_handle, IOHandler* handler);
		void UnRegisterIOHandler(IOHandler* handler);
		void WaitForIOCompletion();
		void Stop();

		// 0 参数
		template <typename TRet, typename TC>
		bool PostTask(TRet (TC::*pfunc)(), TC* pc)
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			PostQueuedCompletionStatus(
				_port,
				0,
				0,
				&NewIOTask(pfunc, pc)->_context.overlapped);

			return true;
		}


		// 1 参数
		template <typename TRet, typename TC, typename TArg>
		bool PostTask(TRet (TC::*pfunc)(TArg), TC* pc, TArg arg)
		{
			if (::GetCurrentThreadId() == _threadID)
				return false;

			PostQueuedCompletionStatus(
				_port,
				0,
				0,
				&NewIOTask(pfunc, pc, arg)->_context.overlapped);

			return true;
		}


	private:
		virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered, DWORD error);

		IOThread();
		unsigned ThreadRun();
		static unsigned WINAPI sThreadRun(void*);


	private:
		HANDLE				_thread;
		HANDLE				_port;
		bool				_quit;
		std::set<Pipe*>		_pipes;
		UINT				_threadID;
	};
}
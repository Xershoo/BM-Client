#include "pipe.h"
#include "iothread.h"
#include "bizutility/log/assert_log.h"
#include <process.h>


namespace gnet
{
	IOThread* IOThread::GetInst()
	{
		static IOThread iothread;
		return &iothread;
	}


	IOThread::IOThread()
		:	_thread(NULL),
		_quit(false),
		_threadID(NULL)
	{
		_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);	// ²¢·¢Îª1
	}


	bool IOThread::Start()
	{
		assert_(NULL == _thread);
		_thread = (HANDLE)_beginthreadex(NULL, 0, sThreadRun, NULL, 0, &_threadID);
		assert_(_thread != NULL);
		return _thread != NULL;
	}


	void IOThread::RegisterIOHandler(HANDLE file_handle, IOHandler* handler)
	{
		ULONG_PTR key = reinterpret_cast<ULONG_PTR>(handler);
		HANDLE port = CreateIoCompletionPort(file_handle, _port, key, 1);
		_pipes.insert(reinterpret_cast<Pipe*>(handler));
	}


	void IOThread::UnRegisterIOHandler(IOHandler* handler)
	{
		std::set<Pipe*>::iterator it = _pipes.find(reinterpret_cast<Pipe*>(handler));
		if (it != _pipes.end())
			_pipes.erase(it);
	}


	void IOThread::WaitForIOCompletion()
	{
		DWORD bytes_transferred;
		ULONG_PTR key = NULL;
		OVERLAPPED* overlapped = NULL;
		IOContext *context = NULL;
		if(GetQueuedCompletionStatus(
			_port,
			&bytes_transferred,
			&key,
			(LPOVERLAPPED*)&context,
			INFINITE))
			context ->handler->OnIOCompleted(context, bytes_transferred, 0);
		else
			context->handler->OnIOCompleted(context, 0, GetLastError());
	}


	void IOThread::Stop()
	{
		assert_(_thread != NULL);
		if(NULL == _thread) return;

		// clear pipes
		std::set<Pipe*> pipes = _pipes;
		std::set<Pipe*>::iterator it = pipes.begin();
		while(it != pipes.end())
		{	
			(*it)->Close();
			it++;
		}
		_pipes.clear();

		IOContext *context = new IOContext();
		memset(&(context->overlapped),0,sizeof(OVERLAPPED));
		context->handler = IOThread::GetInst();
		PostQueuedCompletionStatus(
			_port,
			0,
			0,
			&(context->overlapped));

		WaitForSingleObject(_thread, INFINITE);
		_quit = false;

		CloseHandle(_thread);
		_thread = NULL;
		_threadID = 0;
	}


	void IOThread::OnIOCompleted(IOContext* context, DWORD bytes_transfered, DWORD error)
	{
		_quit = true;
		delete context;
	}


	unsigned IOThread::ThreadRun()
	{
		while(!_quit)
			WaitForIOCompletion();
		return 0;
	}


	unsigned IOThread::sThreadRun(void*)
	{
		return IOThread::GetInst()->ThreadRun();
	}
}
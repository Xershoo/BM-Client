#include "pipe.h"
#include "gnet/log_gnet.h"
#include "bizutility/log/assert_log.h"


namespace gnet
{
	Pipe::Pipe(IPipeListener* lis)
		:	_pipe(INVALID_HANDLE_VALUE),
		ALLOW_THIS_IN_INITIALIZER_LIST(_connect_state(this)),
		ALLOW_THIS_IN_INITIALIZER_LIST(_input_state(this)),
		ALLOW_THIS_IN_INITIALIZER_LIST(_output_state(this)),
		_listener(lis)
	{
		memset(_input_buf, 0, sizeof(_input_buf));
		_closeEvent = CreateEventW(NULL, false, false, NULL);
	}


	Pipe::~Pipe()
	{
		CloseHandle(_closeEvent);
	}


	bool Pipe::Connect4Server(std::wstring name)
	{
		_pipe_name = PipeName(name);
		return Connect4Server();
	}


	bool Pipe::Connect4Server()
	{
		assert_(INVALID_HANDLE_VALUE == _pipe);

		_pipe = CreateNamedPipeW(_pipe_name.c_str(),
			PIPE_ACCESS_DUPLEX| FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE| PIPE_READMODE_BYTE,
			1,
			kReadBufferSize,
			kReadBufferSize,
			5000,
			NULL);

		if(INVALID_HANDLE_VALUE == _pipe)
		{
			LOG << "pip err Connect4Server " << GetLastError();
			assert_(false);
			return false;
		}

		IOThread::GetInst()->RegisterIOHandler(_pipe, this);
		if (!ProcessConnection())
		{
			CloseHandle(_pipe);
			_pipe = INVALID_HANDLE_VALUE;
			return false;
		}

		if (!_connect_state.is_pending)
			PullData();

		return true;
	}


	int Pipe::Connect4Client(std::wstring name)
	{
		assert_(INVALID_HANDLE_VALUE == _pipe);
		_pipe_name = PipeName(name);
		_pipe = CreateFileW(_pipe_name.c_str(),
			GENERIC_READ|GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL);

		DWORD err = GetLastError();

		if(INVALID_HANDLE_VALUE == _pipe)
		{
			if (ERROR_PIPE_BUSY == err)
			{
				assert_(false);
				return 2;
			}
			LOG << "pipe connect error " << err;
			return 1;	// fail
		}

		(IOThread::GetInst())->RegisterIOHandler(_pipe, this);

		// !!! this is critical, do not do it in thread that will end.
		// the io will be canceled cuz of the end of thread.
		IOThread::GetInst()->PostTask(&Pipe::PullData, this);

		_listener->OnConnected();
		return 0;	// succeed
	}


	void Pipe::Send(Message* msg)
	{
		if (IOThread::GetInst()->PostTask(&Pipe::Send, this, msg))
			return;

		if (INVALID_HANDLE_VALUE == _pipe)	// closed
			return;

		bool bEmpty = _output_queue.empty();
		_output_queue.push(msg);

		if (_connect_state.is_pending)
			return;

		if (!bEmpty)
			return;

		PushData();
	}


	void Pipe::Close()
	{
		if (INVALID_HANDLE_VALUE == _pipe)		// ??? there is still chance?
			return;

		ResetEvent(_closeEvent);
		PostClose();
		WaitForSingleObject(_closeEvent, INFINITE);
	}


	void Pipe::OnIOCompleted(IOContext* context, DWORD bytes_transfered, DWORD error)
	{
		bool ok = true;
		if (context == &_connect_state.context)
		{
			if(!ProcessConnection()) return;
			if(_connect_state.is_pending) return;		//	do we need this part so complex ???
			ok = PullData();
			if(ok && !_output_queue.empty()) ok = PushData();
			if (!ok)
				LOG << "pipe err _connect_state " << error;
		}
		if(context == &_input_state.context)
		{
			_input_state.is_pending = false;
			if(0 == bytes_transfered) ok = false;
			else ok = AsyncReadComplete(bytes_transfered);
			if(ok) ok = PullData();
			if (!ok)
				LOG << "pipe err _input_state " << bytes_transfered << ", " << error;
		}
		else if(context == &_output_state.context)
		{
			_output_state.is_pending = false;
			if(0 == bytes_transfered) ok = false;
			else ok = AsyncWriteComplete(bytes_transfered);
			if (ok && !_output_queue.empty()) ok = PushData();
			if (!ok)
				LOG << "pipe err _output_state " << bytes_transfered << ", " << error;
		}

		if(!ok && _pipe != INVALID_HANDLE_VALUE)
			PostClose();
	}


	std::wstring Pipe::PipeName(const std::wstring& channel_id)
	{
		return std::wstring(L"\\\\.\\pipe\\IPC.").append(channel_id);
	}


	bool Pipe::ProcessConnection()
	{
		BOOL ok = ConnectNamedPipe(_pipe, &_connect_state.context.overlapped);
		assert_(!ok);
		// Uhm, the API documentation says that this function should never
		// return success when used in overlapped mode.

		switch(GetLastError())
		{
		case ERROR_IO_PENDING:
			_connect_state.is_pending = true;
			break;
		case ERROR_PIPE_CONNECTED:
			_connect_state.is_pending = false;
			_listener->OnConnected();
			break;
		default:
			return false;
		}
		return true;
	}


	bool Pipe::PullData()
	{
		if(INVALID_HANDLE_VALUE == _pipe) return false;
		DWORD bytes_read = 0;
		if(!ReadFile(_pipe, _input_buf, kReadBufferSize, &bytes_read, &_input_state.context.overlapped))
		{
			DWORD err = GetLastError();
			if(ERROR_IO_PENDING == GetLastError())
			{
				_input_state.is_pending = true;
				return true;
			}
			assert_(false);
			return false;
		}

		// pending for callback, Our completion port will be signalled.
		_input_state.is_pending = true;
		return true;
	}


	bool Pipe::PushData()
	{
		if (INVALID_HANDLE_VALUE == _pipe)
			return false;

		assert_(!_output_queue.empty());
		if(_output_queue.empty())
			return true;

		Message* msg = _output_queue.front();
		DWORD bytes_written = 0;
		BOOL ok = WriteFile(_pipe,
			msg->Head(), msg->Size(),
			&bytes_written,
			&_output_state.context.overlapped);

		_output_state.is_pending = true;

		DWORD err = GetLastError();
		if (!ok && err != ERROR_IO_PENDING)
		{
			assert_(false);
			LOG << "pipe err PushData " << err;
			return false;
		}
		return true;
	}


	bool Pipe::AsyncReadComplete(int bytes_read)
	{
		_last_input.append(_input_buf, bytes_read);
		const char* cur = _last_input.c_str();
		const char* end = cur + _last_input.size();
		const char* next = Message::GetNext(cur, end);
		while(next != NULL)
		{
			_listener->OnMessageReceived(std::string(cur+4, next));
			cur = next;
			next = Message::GetNext(cur, end);
		}
		_last_input.erase(_last_input.begin(), _last_input.begin()+(cur-_last_input.c_str()));

		return true;
	}


	bool Pipe::AsyncWriteComplete(int bytes_written)
	{
		assert_(!_output_queue.empty());	// pending
		if (_output_queue.empty())
			return false;

		Message *m = _output_queue.front();
		_output_queue.pop();
		delete m;
		return true;
	}


	void Pipe::PostClose()
	{
		if (IOThread::GetInst()->PostTask(&Pipe::PostClose, this))
			return;

		if (_input_state.is_pending || _output_state.is_pending)
			CancelIo(_pipe);

		CloseHandle(_pipe);
		// disable the recursive calling when close.
		_pipe = INVALID_HANDLE_VALUE;

		while(_input_state.is_pending || _output_state.is_pending)
			IOThread::GetInst()->WaitForIOCompletion();

		while (!_output_queue.empty()) 
		{
			Message* m = _output_queue.front();
			_output_queue.pop();
			delete m;
		}

		IOThread::GetInst()->UnRegisterIOHandler(this);
		_listener->OnClosed();
		SetEvent(_closeEvent);
	}


	Pipe::State::State(Pipe *channel) : is_pending(false)
	{
		memset(&context.overlapped, 0, sizeof(context.overlapped));
		context.handler = channel;
	}
}
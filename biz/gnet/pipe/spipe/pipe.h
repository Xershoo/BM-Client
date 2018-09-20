#pragma once

#include "iothread.h"
#include "message.h"
#include <queue>


namespace gnet
{
	struct IPipeListener;

	class Pipe : public IOHandler
	{
	public:
		Pipe(IPipeListener*);
		~Pipe();
		bool Connect4Server(std::wstring name);
		bool Connect4Server();
		// return 0 success
		// return 1 failed
		// return 2 ERROR_PIPE_BUSY, in case of another client is in connection now.
		int Connect4Client(std::wstring name);

		void Send(Message* msg);
		void Close();

		// form IOHandler
		virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered, DWORD error);


	private:
		std::wstring PipeName(const std::wstring& channel_id);  // pipe's name is "\\.\pipe\IPC.[channel_id]"
		bool ProcessConnection();	// only for server
		bool PullData();
		bool PushData();
		bool AsyncReadComplete(int bytes_read);
		bool AsyncWriteComplete(int bytes_written);
		void PostClose();


	protected:
		struct State 
		{
			explicit State(Pipe* channel);
			IOContext	context;
			bool		is_pending;
		};


		static const size_t		kReadBufferSize = 4 * 1024;
		HANDLE					_pipe;
		std::wstring			_pipe_name;
		State					_connect_state;	// only for server
		State					_input_state;
		State					_output_state;
		std::queue<Message*>	_output_queue;
		char					_input_buf[kReadBufferSize];
		std::string				_last_input;
		IPipeListener*			_listener;
		HANDLE					_closeEvent;
	};


	struct IPipeListener
	{
		virtual void OnConnected() = 0;
		virtual void OnMessageReceived(std::string data) = 0;
		virtual void OnClosed() = 0;
		virtual ~IPipeListener(){}
	};
}
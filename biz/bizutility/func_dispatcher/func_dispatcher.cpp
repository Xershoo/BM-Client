#include "func_dispatcher.h"
#include "../log/assert_log.h"


namespace biz_utility
{
	const wchar_t* AnsyFuncDispcher::windowClassName = L"biz_utility::AnsyFuncDispcher";


	AnsyFuncDispcher::AnsyFuncDispcher()
		: _hwnd(NULL),
		_threadID(NULL){}


	bool AnsyFuncDispcher::Startup()
	{
		_threadID = ::GetCurrentThreadId();

		if (!RegisterClass())
			return false;

		_hwnd = ::CreateWindowW(windowClassName, L"", 
			0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

		return NULL != _hwnd;
	}


	bool AnsyFuncDispcher::Cleanup()
	{
		if (NULL != _threadID
			&& GetCurrentThreadId() != _threadID)
		{
			assert_(false);
			return false;
		}

		if (NULL != _hwnd)
		{
			::DestroyWindow(_hwnd);
			_hwnd = NULL;
		}
		_threadID = NULL;
		UnregisterClassW(windowClassName, ::GetModuleHandle(NULL));

		return true;
	}


	bool AnsyFuncDispcher::ScheduleTimer(AsynTimer& timer, int period)
	{
		if (timer.isruning) return true;
		timer.isruning = 0 != ::SetTimer(_hwnd, (UINT_PTR)&timer, period, TimerProc);
		return timer.isruning;
	}


	void AnsyFuncDispcher::DeleteTimer(AsynTimer& timer)
	{
		if (timer.isruning)
		{
			::KillTimer(_hwnd, (UINT_PTR)&timer);
			timer.isruning = false;
		}
	}


	bool AnsyFuncDispcher::RegisterClass()
	{
		WNDCLASSEXW wc;
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = 0;
		wc.lpfnWndProc = AnsyFuncDispcher::WndProc;
		wc.hInstance = ::GetModuleHandle(NULL);
		wc.lpszClassName = windowClassName;
		return (NULL != ::RegisterClassExW(&wc) || 
			ERROR_CLASS_ALREADY_EXISTS == ::GetLastError());
	}


	LRESULT WINAPI AnsyFuncDispcher::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (WM_ANSY_TASK == msg)
		{
			Task* task = reinterpret_cast<Task*>(wParam);
			task->RunDelete();
			return 0;
		}

		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}


	void CALLBACK AnsyFuncDispcher::TimerProc(HWND, UINT, UINT_PTR idEvent, DWORD)
	{
		((AsynTimer*)idEvent)->Process();
	}
}
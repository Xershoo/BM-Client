#pragma once

#include <Windows.h>
#include <sstream>
#include <fstream>
#include <time.h>


// def1. define log file size.
#ifndef LOG_FILE_SIZE
#define LOG_FILE_SIZE 1024 * 1024	// default file size 1M.
#endif

// def2. define log file name.
#ifndef LOG_FILE_NAME
#define LOG_FILE_NAME "biz_log.txt"
#endif


namespace biz_utility
{
	class LogColor
	{
	public:
		static LogColor Default(){ return WhiteFront(); }

		static LogColor RedFront() { return FOREGROUND_RED; }
		static LogColor GreenFront() { return FOREGROUND_GREEN; }
		static LogColor BlueFront() { return FOREGROUND_BLUE; }
		static LogColor WhiteFront() { return FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE; }
		static LogColor IntensityFront() { return FOREGROUND_INTENSITY; }

		static LogColor RedBack() { return BACKGROUND_RED; }
		static LogColor GreenBack() { return BACKGROUND_GREEN; }
		static LogColor BlueBack() { return BACKGROUND_BLUE; }
		static LogColor WhiteBack() { return BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE; }
		static LogColor IntensityBack() { return BACKGROUND_INTENSITY; }

		LogColor(WORD col) : _color(col) {}
		operator WORD() { return _color; }

	private:
		WORD _color;
	};


	class LogConsole
	{
	public:
		LogConsole(bool bNull = false) : _bNull(bNull)
		{ !bNull && AllocConsole(); }	// bNull表示是否为空log
		~LogConsole(){ !_bNull && FreeConsole(); }

		template<typename T>
		LogConsole& operator << (T outdata)
		{
			if (_bNull)	return *this;

			std::wostringstream ostr;
			ostr << outdata;

			DWORD nWritten;
			::WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),
				ostr.str().c_str(), ostr.str().size(), &nWritten, 0);
			return *this;
		}

		LogConsole& operator << (std::string outdata)
		{
			return *this << outdata.c_str();
		}

		LogConsole& operator << (std::wstring outdata)
		{
			return *this << outdata.c_str();
		}

		LogConsole& operator << (LogColor col)
		{
			if (_bNull)	return *this;

			CONSOLE_SCREEN_BUFFER_INFO info;
			::GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
			WORD oldAttri = info.wAttributes;
			WORD targetAttri = oldAttri&
				~(LogColor::WhiteFront()|LogColor::WhiteBack()
				|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY)|col;
			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), targetAttri);
			return *this;
		}

		// 兼容老的log打印方式.
		LogConsole& operator ()(const char * fmt, ...)
		{
			char buf[1024];

			va_list ap;
			va_start(ap, fmt);
			vsprintf_s(buf, 1024-1, fmt, ap);
			va_end(ap);

			DWORD nWritten = 0;
			::WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buf, strlen(buf), &nWritten, 0);
			return *this;
		}

		void SetID(__int64 id){};


	private:
		bool	_bNull;
	};


	class LogFile
	{
	public:
		// 名字为空则表示空log，所有log丢弃。
		LogFile(const char* pszFileName, int maxsize) :	_id(0)
		{
			if (NULL == pszFileName)
				return;

			_ofs.open(pszFileName, std::ios::app);
			_ofs.seekp(0, std::ios_base::end);
			if (_ofs.tellp() > maxsize)
			{
				char newName[MAX_PATH];
				sprintf_s(newName, sizeof(newName), "%s.%s", pszFileName, "bak");
				_ofs.close();
				MoveFileA(pszFileName, newName);
				_ofs.open(pszFileName, std::ios::trunc);
			}

			if (_ofs.tellp() > 0)
				*this << "\n\n\n";	// 新启动的进程，新log空三行.
		}

		void SetID(__int64 id) { _id = id; *this << "Set log id: " << _id; }
		__int64 GetID() { return _id; }

		template<typename T>
		LogFile& operator << (T outdata)
		{
			_ofs << outdata;
			_ofs.flush();
			return *this;
		}

		LogFile& operator << (std::string outdata)
		{
			_ofs << outdata.c_str();
			_ofs.flush();
			return *this;
		}

		LogFile& operator << (const wchar_t* wstr)
		{
			int size = WideCharToMultiByte(CP_ACP, 0, wstr, wcslen(wstr), NULL, 0, NULL, NULL)+1;
			char* buf = new char[size];
			size = WideCharToMultiByte(CP_ACP, 0, wstr, wcslen(wstr), buf, size, NULL, NULL);
			buf[size] = 0;
			return *this << buf;
			delete[] buf;
		}

		LogFile& operator ()(const char * fmt, ...)
		{
			char buf[1024] = {0};
			strcpy_s(buf, 1024, fmt);
			va_list ap;
			va_start(ap, fmt);
			DWORD nWritten = vsprintf_s(buf, 1024, fmt, ap);
			va_end(ap);
			return *this << buf;
		}


	private:
		std::wofstream	_ofs;
		__int64			_id;
	};


// def3. define log name.
#ifdef LOG_NAME
#define GetLog1(name) name##_getLog
#define GetLog0(name) GetLog1(name)
#define GetLog GetLog0(LOG_NAME)
#endif


#ifdef _DEBUG
#define LOGCONSOLE	// 控制是否用控制台显示log
#endif

#ifdef LOGCONSOLE
	extern LogConsole& GetLog();
#else
	extern LogFile& GetLog();
#endif

#define LOG	biz_utility::GetLog()
};
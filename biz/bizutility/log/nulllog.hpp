namespace biz_utility
{
#ifdef _DEBUG
	LogConsole& GetLog()

	{
		static LogConsole null_log_console(true);
		return null_log_console;
	}
#else
	LogFile& GetLog()
	{
		static LogFile null_log_file(NULL, 0);
		return null_log_file;
	}
#endif
}
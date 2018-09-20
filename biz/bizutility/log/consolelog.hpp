namespace biz_utility
{
	LogConsole& GetLog()
	{
		static LogConsole* pLog = NULL;
		if (NULL == pLog)
			pLog = new LogConsole(FALSE);  //xiewb 2014.12.30
		else
			*pLog << "\n" << LogColor::Default();
		return *pLog;
	}
}
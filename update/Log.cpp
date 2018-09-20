#include "stdafx.h"
#include "Log.h"
#include <time.h>
#include <io.h> 

#pragma warning(disable : 4996)

static int W2M(const wchar_t * src, int cch, char * dest, int cbByte, UINT cp)
{
	if (src == NULL || dest == NULL || cbByte <= 0)
		return 0;

	int len = WideCharToMultiByte(cp, 0, src, cch, NULL, 0, NULL, NULL);
	if (len == 0)
		return 0;
	if (len > cbByte)
		return 0;

	return WideCharToMultiByte(cp, 0, src, cch, dest, cbByte, NULL, NULL);
}

//log全局对象
CLog g_log;
CLog g_log_ak;
CLog g_log_vt;

CLog::CLog() : m_pPipe(NULL)
{
}

CLog::~CLog()
{
}

bool CLog::Initialize(
		LOG_TYPE logLevel, 
		const char * pszLogTool,
		bool debug,
		int maxsize)
{
	m_LogLevel = logLevel;
	m_bDebug = debug;
	
	if( (!debug) && pszLogTool && pszLogTool[0] )
	{
		if ( 0 == maxsize )
		{
			m_pPipe = fopen(pszLogTool, "w");
			if( m_pPipe == NULL )
			{
				printf("log::initialize fopen %s error: %s\n", pszLogTool, strerror(GetLastError()));
				return false;
			}
		}
		else
		{
			m_pPipe = fopen(pszLogTool, "ab");
			if( m_pPipe == NULL )
			{
				printf("log::initialize fopen %s error: %s\n", pszLogTool, strerror(GetLastError()));
				return false;
			}
			int size = _filelength(fileno(m_pPipe));
			if ( size > maxsize )
			{
				//如果文件大小超过了最大的大小, 清空文件并重新打开
				fclose(m_pPipe);
				m_pPipe = fopen(pszLogTool, "w");
				if( m_pPipe == NULL )
				{
					printf("log::initialize fopen %s error: %s\n", pszLogTool, strerror(GetLastError()));
					return false;
				}

			}
		}

	}
	else
	{
		m_pPipe = stdout;
	}
	
	return true;
}

void CLog::Shutdown()
{
	if( m_pPipe && m_pPipe != stdout )
	{
		fclose(m_pPipe);
	}
	m_pPipe = NULL;
}

void CLog::logA(LOG_TYPE logType, const char * fmt, ...)
{
	va_list ap;
	
	const char * p;        
    	time_t t;
	struct tm * pt;
	char buf[1024] = {0};
	
	if( logType < m_LogLevel )
		return;
	
	switch(logType)
    {
	case L_DBG:				//< 调试信息,正式运行时不应输出    		
		p = "DEBUG";
		break;
	case L_INFO:			//< 一般信息			
		p = "INFO";
		break;
	case L_WARNING:	//< 警告,可能存在问题
		p = "WARNING";
		break;
	case L_ERR:				//< 出错,但系统还可能继续运行
		p = "ERROR";
		break;
	case L_FATAL:			//< 致命错误,系统无法继续运行
		p = "FATAL";
		break;
	case L_DATA:
		p = "DATA";
		break;
	default:
		p = "UNKNOWN";
		break;
	}
        
	time(&t);
	pt = localtime(&t);
	pt->tm_year += 1900;
	pt->tm_mon++;
	_snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\r\n", pt->tm_year, pt->tm_mon, pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec, p, fmt);

	if (m_pPipe)
	{
		va_start(ap, fmt);
		vfprintf(m_pPipe, buf, ap);
		va_end(ap);
		fflush(m_pPipe);
	}
}

void CLog::logW(LOG_TYPE logType, const wchar_t * fmt, ...)
{
	va_list ap;

	const wchar_t * p;
	time_t t;
	struct tm * pt;
	wchar_t buf[1024] = {0};
	wchar_t tBuf[1024] = {0};

	if( logType < m_LogLevel )
		return;

	switch(logType)
	{
	case L_DBG:				//< 调试信息,正式运行时不应输出    		
		p = L"DEBUG";
		break;
	case L_INFO:			//< 一般信息			
		p = L"INFO";
		break;
	case L_WARNING:	//< 警告,可能存在问题
		p = L"WARNING";
		break;
	case L_ERR:				//< 出错,但系统还可能继续运行
		p = L"ERROR";
		break;
	case L_FATAL:			//< 致命错误,系统无法继续运行
		p = L"FATAL";
		break;
	case L_DATA:
		p = L"DATA";
		break;
	default:
		p = L"UNKNOWN";
		break;
	}

	time(&t);
	pt = localtime(&t);
	pt->tm_year += 1900;
	pt->tm_mon++;
	swprintf_s(buf, 1024, L"%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\r\n", pt->tm_year, pt->tm_mon, pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec, p, fmt);

	va_start(ap, fmt);
	vswprintf_s(tBuf, 1024, buf, ap);
	va_end(ap);

	char szBuf[4096] = {0};
	W2M(tBuf, -1, szBuf, 4096, CP_ACP);
	
	if (m_pPipe)
	{
		fputs(szBuf, m_pPipe);
		fflush(m_pPipe);
	}
}

#ifndef CLOG_H_
#define CLOG_H_

#include <stdio.h>

/**
 * 日志级别
 **/
typedef enum
{
	L_DBG,				//< 调试信息,正式运行时不应输出
	L_INFO,				//< 一般信息
	L_WARNING,			//< 警告,可能存在问题
	L_ERR,				//< 出错,但系统还可能继续运行
	L_FATAL,			//< 致命错误,系统无法继续运行
	L_DATA
} LOG_TYPE;

#ifdef _DEBUG
#define ISDEBUG TRUE
#else
#define ISDEBUG FALSE
#endif

class CLog
{
public:
	CLog();
	~CLog();
	
	/**
	 * 初始化LOG
	 * logLevel日志级别,用作日志输出过滤
	 * pszLogTool日志分割工具
	 * debug,是否debug,如果debug=true,不写文件,直接通过控制台输出 
	 * maxsize,日志文件大小的上限, 默认为0, 即每次都要重写文件
	 **/
	bool Initialize(
		LOG_TYPE logLevel, 
		const char * pszLogTool,
		bool debug = ISDEBUG,
		int  maxsize = 0);

	void Shutdown();
	void logA(LOG_TYPE logType, const char * fmt, ...);
	void logW(LOG_TYPE logType, const wchar_t * fmt, ...);

private:
	LOG_TYPE m_LogLevel;
	bool m_bDebug;
	FILE * m_pPipe;
};


extern CLog g_log;
extern CLog g_log_ak;
extern CLog g_log_vt;

#ifdef _UNICODE
#define LOG g_log.logW
#define LOGV	g_log_vt.logW
#define LOGH	g_log_ak.logW
#else
#define LOG g_log.logA
#define LOGV	g_log_vt.logA
#define LOGH	g_log_ak.logA
#endif

#endif /*CLOG_H_*/

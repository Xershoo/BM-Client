#ifndef CLOG_H_
#define CLOG_H_

#include <stdio.h>

/**
 * ��־����
 **/
typedef enum
{
	L_DBG,				//< ������Ϣ,��ʽ����ʱ��Ӧ���
	L_INFO,				//< һ����Ϣ
	L_WARNING,			//< ����,���ܴ�������
	L_ERR,				//< ����,��ϵͳ�����ܼ�������
	L_FATAL,			//< ��������,ϵͳ�޷���������
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
	 * ��ʼ��LOG
	 * logLevel��־����,������־�������
	 * pszLogTool��־�ָ��
	 * debug,�Ƿ�debug,���debug=true,��д�ļ�,ֱ��ͨ������̨��� 
	 * maxsize,��־�ļ���С������, Ĭ��Ϊ0, ��ÿ�ζ�Ҫ��д�ļ�
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

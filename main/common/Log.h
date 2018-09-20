//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�log.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.1
//	˵������־�ඨ��ͷ�ļ�
//  �ӿڣ�
//      1.����ͨ��LOG_DEBUG()��LOG_WARN()��LOG_INFO()��LOG_ERROR()��¼��ͬ���͵���־
//      2.����ͨ��qDebug()��qWarning()��qInfo()��qCritial()��¼��ͬ���͵���־
//      3.DEBUG������־��Ϣ��ʾ��console�У�Release�汾��¼������Ŀ¼��LOGĿ¼����־�ļ���
//      
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef LOG_H
#define LOG_H

#ifndef _DEBUG
#ifndef QT_MESSAGELOGCONTEXT
#define QT_MESSAGELOGCONTEXT
#endif
#endif

#include <string>
#include <QtCore/QFile>
#include <QtCore\QTime>
#include <QtCore\QMutex>
#include <QtCore\QDebug>

using namespace std;

enum LOG_LEVEL
{
    LOG_LEVEL_NULL = 0x0,
    LOG_LEVEL_DEBUG = 0x1,
    LOG_LEVEL_WARN = 0x2,
    LOG_LEVEL_INFO = 0x4,
    LOG_LEVEL_ERROR = 0x8,

    LOG_LEVEL_ALL = 0xf,
};

#define LOG_DEBUG()         (getLog(LOG_LEVEL_DEBUG)<<"Debug: "<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz") <<  \
                                " File:" << __FILE__ << " Line:" << __LINE__ << " information: ") 

#define LOG_WARN()          (getLog(LOG_LEVEL_WARN) <<"Warn:  "<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz") <<  \
                                " File:" << __FILE__ << " Line:" << __LINE__ << " information: ")

#define LOG_INFO()          (getLog(LOG_LEVEL_INFO) <<"Info:  "<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz") <<  \
                                " File:" << __FILE__ << " Line:" << __LINE__ << " information: ")

#define LOG_ERROR()         (getLog(LOG_LEVEL_ERROR)<<"Error: "<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz") <<  \
                                " File:" << __FILE__ << " Line:" << __LINE__ << " information: ")

class CLog : public QFile
{
    Q_OBJECT
public:
    CLog(unsigned int logLevel = 0);
    ~CLog();
   
public:
    void writeLog(QString qstrLog,LOG_LEVEL logLevel = LOG_LEVEL_DEBUG);
    void writeLog(const char* pszLog,int nLength,LOG_LEVEL logLevel = LOG_LEVEL_DEBUG);

    inline void setLogLevel(unsigned int nLevel)
    {
        m_logLevel = nLevel;
    };

    inline unsigned int getLogLevel()
    {
        return m_logLevel;
    };

public:
    CLog& operator<<(signed int);
    CLog& operator<<(unsigned int);
    CLog& operator<<(const char*);
    CLog& operator<<(const QString&);
    CLog& operator<<(bool);
    CLog& operator<<(char);
    CLog& operator<<(const void*);
    CLog& operator<<(const string&);

protected:
    void createLogPath();
    void openLogFile();

private:
    unsigned int m_logLevel;
    QMutex  m_logMutex;
};

extern CLog& getLog(LOG_LEVEL logLevel);
extern void freeLog();

#endif
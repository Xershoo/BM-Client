//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：log.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.1
//	说明：日志类定义头文件
//  接口：
//      1.可以通过LOG_DEBUG()，LOG_WARN()，LOG_INFO()，LOG_ERROR()记录不同类型的日志
//      2.可以通过qDebug()，qWarning()，qInfo()，qCritial()记录不同类型的日志
//      3.DEBUG下面日志信息显示在console中，Release版本记录在运行目录下LOG目录的日志文件中
//      
//	修改记录：
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
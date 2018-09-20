//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：log.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.1
//	说明：日志记录类实现源文件
//      
//	修改记录：
//      
//**********************************************************************

#include "Log.h"
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include "Util.h"
#include "setdebugnew.h"

#ifdef _DEBUG
#pragma comment(linker,"/subsystem:console")
#endif

CLog   g_pLog = NULL;
CLog   g_logNull;

CLog& getLog(LOG_LEVEL logLevel)
{
    if(LOG_LEVEL_NULL == logLevel)
    {
        return g_logNull;
    }

    if(!g_pLog.getLogLevel())
    {
        g_pLog.setLogLevel(LOG_LEVEL_ALL);
    }

    if(!g_pLog.getLogLevel() && (logLevel & g_pLog.getLogLevel()))
    {
        return g_pLog;
    }

    return g_logNull;
}

void freeLog()
{
//     if(NULL == g_pLog)
//     {
//         return;
//     }
// 
//     delete g_pLog;
//     g_pLog = NULL;
}

void debugMessageHandler(QtMsgType msgType, const QMessageLogContext & msgLogContext, const QString & msg)
{
    LOG_LEVEL logLevel = LOG_LEVEL_NULL;
    switch(msgType)
    {
    case QtDebugMsg:
        {
            logLevel = LOG_LEVEL_DEBUG;
            getLog(logLevel)<<"Debug: ";
        }
        break;
    case QtWarningMsg:
        {
            logLevel = LOG_LEVEL_WARN;
            getLog(logLevel)<<"Warn:  ";
        }
        break;
    case QtInfoMsg:
    case QtCriticalMsg:
        {
            logLevel = LOG_LEVEL_INFO;
            getLog(logLevel)<<"Info:  ";
        }
        break;
    case QtFatalMsg:
        {
            logLevel = LOG_LEVEL_ERROR;
            getLog(logLevel)<<"Error: ";
        }
        break;    
    };

    getLog(logLevel)<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz") << 
        " File:" << msgLogContext.file << 
        " Line:" << msgLogContext.line << 
        " information: " << msg << " .\n";

}

CLog::CLog(unsigned int logLevel /* = 0 */):QFile()
{
    m_logLevel = logLevel;
    qInstallMessageHandler(debugMessageHandler);

#ifndef _DEBUG
    if(m_logLevel>LOG_LEVEL_NULL)
    {
        openLogFile();
    }
#endif

}

CLog::~CLog()
{
    close();
}

void CLog::createLogPath()
{
    QString strPath = QCoreApplication::applicationDirPath();

    QDir dirCur(strPath);
    strPath.append("/Log");

    QFileInfo infoPath(strPath);
    if(infoPath.exists())
    {
        if(infoPath.isDir())
        {
            return;
        }

        dirCur.remove("Log");
    }
    
    dirCur.mkdir("Log");
}

void CLog::openLogFile()
{
    createLogPath();

    QString strFile;
    QString strPath = QCoreApplication::applicationDirPath();
    strPath.append("/Log/");
    strFile = QString("%1%2.log").arg(strPath,QDateTime::currentDateTime().toString("yyyy-MM-dd"));

    setFileName(strFile);
    open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
}

void CLog::writeLog(QString qstrLog,LOG_LEVEL logLevel /* = LOG_LEVEL_DEBUG */)
{
    if(!(logLevel & m_logLevel))
    {
        return;
    }

    if(qstrLog.isEmpty())
    {
        return;
    }

    QByteArray byteArray = qstrLog.toUtf8();
        
    writeLog((const char*)byteArray.data(),byteArray.length());
}

void CLog::writeLog(const char* pszLog,int nLength,LOG_LEVEL logLevel /* = LOG_LEVEL_DEBUG */)
{
    QMutexLocker autoLock(&m_logMutex);

    if(!(logLevel & m_logLevel))
    {
        return;
    }
    
    if(NULL == pszLog || nLength <= 0)
    {
        return;
    }

#ifndef _DEBUG
    if(!this->isOpen())
    {
        return;
    }

    qint64 nWrite = this->write((const char*)pszLog,nLength);
    if(nWrite != nLength)
    {
        return;
    }

    this->flush();
#else
    printf((const char*)pszLog);
#endif

    return;
}

CLog& CLog::operator<<(signed int t)
{
    char szLog[128] = { 0 };

    sprintf(szLog,"%d",t);

    writeLog(szLog);

    return *this;
}

CLog& CLog::operator<<(unsigned int t)
{
    char szLog[128] = { 0 };

    sprintf(szLog,"%u",t);

    writeLog(szLog);

    return *this;
}

CLog& CLog::operator<<(const char* t)
{
    if(NULL == t || NULL == t[0])
    {
        return *this;
    }
    
    writeLog(t,strlen(t));
        
    return *this;
}

CLog& CLog::operator<<(const QString& t)
{
    QString qstrLog = t;

    writeLog(qstrLog);

    return *this;
}

CLog& CLog::operator<<(bool t)
{  
    char szLog[128] = { 0 };

    strcpy(szLog,t?"true":"false");
    
    writeLog(szLog);

    return *this;
}

CLog& CLog::operator<<(char t)
{
    char szLog[3]={t,'\n','\0'};
    
    writeLog(szLog,2);

    return *this;
}

CLog& CLog::operator<<(const void *t)
{
    char szLog[128] = { 0 };
    sprintf(szLog,"0x%x",t);

    writeLog(szLog);

    return *this;
}

CLog& CLog::operator<<(const string& t)
{
    string strLog(t);    
    
    writeLog((const char*)strLog.c_str(),strLog.length());

    return *this;
}
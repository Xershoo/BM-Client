//CL8Config.h
#pragma once

#include <QString>

enum HttpConfigType
{
    HTTP_CONFIG_TYPE_IP = 0,
    HTTP_CONFIG_TYPE_WEB,
};

class CL8Config
{
public:
    CL8Config();

    ~CL8Config();


public:
    static CL8Config *Getinst();

    bool InitGloabConfig();

protected:

public:
    QString     m_strServerIP;
    QString     m_strServerPort;

    int         m_HttpType; //≈‰÷√¿‡–Õ£®IP°¢Web£©
    QString     m_strHttpTransType;
    //ip ≈‰÷√
    QString     m_strHttpIP;
    QString     m_strHttpUpPort;
    QString     m_strHttpUpFold;
    QString     m_strHttpDownPort;
    QString     m_strHttpDownFold;

    //”Ú√˚ ≈‰÷√
    QString     m_strHttpUpUrl;
    QString     m_strHttpUpWebFold;
    QString     m_strHttpDownUrl;
    QString     m_strHttpDownWebFold;

    QString     m_strClassUrl;
    QString     m_strImageDownUrl;

    QString     m_strChatUpUrl;
    QString     m_strChatUpFold;
    QString     m_strChatDownUrl;
    QString     m_strChatDownFold;

    __int64     m_nUserId;
    bool        m_bISLocalTrans;
};

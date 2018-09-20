//CL8Config.cpp

#include "CConfig.h"
#include <QFile>
#include <QtXml/QDomComment>
#include <QtXml/QDomElement>
#include <QtXml/QDomAttr>
#include <QtXml/QDomNode>
#include <QtXml/QDomNodeList>
#include "Env.h"

CL8Config::CL8Config()
{
    m_strServerIP = "";
    m_strServerPort = "";

    m_strHttpIP = "";
    m_strHttpUpPort = "";
    m_strHttpUpFold = "";
    m_strHttpDownPort = "";
    m_strHttpDownFold = "";
    m_strHttpTransType = "";

    m_strHttpUpUrl = "";
    m_strHttpUpWebFold = "";
    m_strHttpDownUrl = "";
    m_strHttpDownWebFold = "";

    m_strClassUrl = "";
    m_strImageDownUrl = "";

    m_strChatUpUrl = "";
    m_strChatUpFold = "";
    m_strChatDownUrl = "";
    m_strChatDownFold = "";

    m_nUserId = 0;
    m_HttpType = 0;
    m_bISLocalTrans = false;
    InitGloabConfig();
}

CL8Config::~CL8Config()
{

}

CL8Config * CL8Config::Getinst()
{
    static CL8Config pConfig;
    return &pConfig;
}

bool CL8Config::InitGloabConfig()
{
    QString strPath(":/config/config/config.xml");
    QFile file(strPath);

    if (strPath.isEmpty())
    {
        return false;
    }
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    QDomDocument document;
    QString	strError;
    int nErrLin = 0, nErrorID = 0;
    if (!document.setContent(&file, false, &strError, &nErrLin, &nErrorID))
    {
        file.close();
        return false;
    }
    if (document.isNull())
    {
        file.close();
        return false;
    }
    QDomNodeList list = document.firstChild().childNodes();
    for (int i = 0; i < list.count(); i++)
    {
        QDomNode node = list.at(i);
        if (node.nodeName() == "ip")
        {
            m_strServerIP = node.toElement().attributeNode("value").value();
            m_strServerPort = node.toElement().attributeNode("port").value();
        }

        if (node.nodeName() == "httpip")
        {
            m_strHttpIP = node.toElement().attributeNode("value").value();
            m_strHttpUpPort = node.toElement().attributeNode("upport").value();
            m_strHttpUpFold = node.toElement().attributeNode("foldup").value();
            m_strHttpDownPort = node.toElement().attributeNode("downport").value();
            m_strHttpDownFold = node.toElement().attributeNode("folddown").value();
            m_strHttpTransType = node.toElement().attributeNode("localtrans").value();
            if (m_strHttpTransType == QString("0"))
            {
                m_bISLocalTrans = false;
            }
            else
            {
                m_bISLocalTrans = true;
            }
            m_HttpType = HTTP_CONFIG_TYPE_IP;
        }
        else if (node.nodeName() == "httpweb")
        {
            m_strHttpUpUrl = node.toElement().attributeNode("up").value();
            m_strHttpUpWebFold = node.toElement().attributeNode("foldup").value();
            m_strHttpDownUrl = node.toElement().attributeNode("down").value();
            m_strHttpDownWebFold = node.toElement().attributeNode("folddown").value();
            m_strHttpTransType = node.toElement().attributeNode("localtrans").value();
            if (m_strHttpTransType == QString("0"))
            {
                m_bISLocalTrans = false;
            }
            else
            {
                m_bISLocalTrans = true;
            }
            m_HttpType = HTTP_CONFIG_TYPE_WEB;
        }

        if (node.nodeName() == "classurl")
        {
            m_strClassUrl = node.toElement().attributeNode("value").value();
            m_strImageDownUrl = node.toElement().attributeNode("imgdown").value();
        }

        if (node.nodeName() == "chat")
        {
            m_strChatUpUrl = node.toElement().attributeNode("up").value();
            m_strChatUpFold = node.toElement().attributeNode("foldup").value();
            m_strChatDownUrl = node.toElement().attributeNode("down").value();
            m_strChatDownFold = node.toElement().attributeNode("folddown").value();
        }
    }
    file.close();
    return true;
}

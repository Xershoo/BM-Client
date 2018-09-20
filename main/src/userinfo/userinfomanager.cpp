//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�userinfomanager.cpp
//	�汾�ţ�1.0
//	���ߣ�������
//	ʱ�䣺2015.11.28
//	˵�����û�������Ϣ��
//	�޸ļ�¼��
//**********************************************************************

#include "userinfomanager.h"
#include "Env.h"
#include <QTextStream>
#include <QDir>

UserInfoManager::UserInfoManager(int userID)
{
    QString filePath = Env::getExePath();
    filePath = QString("%1\\..\\..\\Users\\%2").arg(filePath).arg(userID);
    QDir dir(filePath);
    if (!dir.exists())
    {
        dir.mkpath(filePath);
    }
    filePath += "/usersetting.xml";
    m_file.setFileName(filePath);
    m_file.open(QFileDevice::ReadWrite);
    m_domDoc.setContent(&m_file);
}

UserInfoManager::~UserInfoManager(void)
{
    if (m_file.isOpen())
    {
        m_file.resize(0);
        QTextStream out(&m_file);
        m_domDoc.save(out, 4);
        m_file.close();
    }
}

QString UserInfoManager::getHotkeyValue(const QString &name)
{
    if (!m_file.isOpen())
    {
        return "";
    }

    QDomNodeList elementList = m_domDoc.elementsByTagName(name);
    if (elementList.count() > 0)
    {
        return elementList.at(0).toElement().text();
    }
    
    return "";
}

bool UserInfoManager::updateHotkey(const QString &name, const QString &value)
{
    if (!m_file.isOpen())
    {
        return false;
    }

    QDomNodeList list = m_domDoc.childNodes();

    bool findNode = false;
    QDomNode node;
    for (int i = 0; i != list.count(); i++)
    {
        node = list.at(i);
        if (node.nodeName() == "HotKeySetting")
        {
            findNode = true;
            break;
        }
    }

    if (!findNode)
    {
        node = m_domDoc.createElement("HotKeySetting");
        m_domDoc.appendChild(node);
        
    }
    findNode = false;

    QDomNode nodeKey;
    QDomNodeList elementList = m_domDoc.elementsByTagName(name);
    if (elementList.count() > 0)
    {
        findNode = true;
        nodeKey = elementList.at(0);
    }
    if (findNode)
    {
        QDomElement newValue = m_domDoc.createElement(name);
        newValue.appendChild(m_domDoc.createTextNode(value));
        node.replaceChild(newValue, nodeKey);
    }
    else
    {
        QDomElement element = m_domDoc.createElement(name);
        element.appendChild(m_domDoc.createTextNode(value));
        node.appendChild(element);
    }
    
    return true;
}

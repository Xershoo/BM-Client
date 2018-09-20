//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�userinfomanager.h
//	�汾�ţ�1.0
//	���ߣ�������
//	ʱ�䣺2015.11.28
//	˵�����û�������Ϣ��
//	�޸ļ�¼��
//  ��ע��
//      1.������¼�û�����������Ϣ  �磺��ݼ�������ͷѡ��Ƥ���ȵ�
//      
//**********************************************************************

#pragma once
#include <QFile>
#include <QDomDocument>

class UserInfoManager
{
public:
    UserInfoManager(int userID);
    virtual ~UserInfoManager(void);

    QString getHotkeyValue(const QString &name);
    bool updateHotkey(const QString &name, const QString &value);

private:
    QFile m_file;
    QDomDocument m_domDoc;
};


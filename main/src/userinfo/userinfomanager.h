//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：userinfomanager.h
//	版本号：1.0
//	作者：郭鹏程
//	时间：2015.11.28
//	说明：用户个人信息类
//	修改记录：
//  备注：
//      1.用来记录用户个人设置信息  如：快捷键，摄像头选择，皮肤等等
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


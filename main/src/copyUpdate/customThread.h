//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：customThread.h
//	版本号：1.0
//	作者：潘良
//	时间：2016.3.4
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#pragma once

#include <QThread>


class CustomThread : public QThread
{
public:

    CustomThread(){}
    virtual ~CustomThread(){}

public:

    bool startthread();
    bool stopthread();

protected:
    virtual void run() = 0;

};

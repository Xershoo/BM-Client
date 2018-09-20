//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�customThread.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2016.3.4
//	˵����
//	�޸ļ�¼��
//  ��ע��
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

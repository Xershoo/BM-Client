//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�customThread.cpp
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2016.3.4
//	˵����
//	�޸ļ�¼��
//  ��ע��
//**********************************************************************


#include "customThread.h"


bool CustomThread::startthread()
{
    start();
    return true;
}

bool CustomThread::stopthread()
{
    exit();
    return true;
}


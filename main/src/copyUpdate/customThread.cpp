//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：customThread.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2016.3.4
//	说明：
//	修改记录：
//  备注：
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


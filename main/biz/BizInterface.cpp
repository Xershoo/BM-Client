//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：BizInterface.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：biz库的初始化接口和反初始化接口的实现
//	修改记录：    
//**********************************************************************

#include "BizInterface.h"
#include "setdebugnew.h"
CBizCallBack* g_bizCallback = NULL;

void initBizLibray()
{
    if(NULL == g_bizCallback)
    {
        g_bizCallback = new CBizCallBack();
    }

    GetBizInterface()->SetCallback(g_bizCallback);

    return;    
}

void unitBizLibray()
{
	GetBizInterface()->Stop();
	GetBizInterface()->SetCallback(NULL);

    if(g_bizCallback)
    {
        delete g_bizCallback;
        g_bizCallback = NULL;        
    }

    DelBizInterface();
}

CBizCallBack* getBizCallBack()
{
    return g_bizCallback;
}
//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�BizInterface.cpp
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.23
//	˵����biz��ĳ�ʼ���ӿںͷ���ʼ���ӿڵ�ʵ��
//	�޸ļ�¼��    
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
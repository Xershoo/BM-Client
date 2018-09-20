//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：LoginTokenMgr.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：提供登录Token获取和管理的类定义
//	修改记录：
//**********************************************************************

#include "LoginTokenMgr.h"
#include "biz/interface/interface.h"
#include "BizInterface.h"
#include <QtCore/QTimerEvent>

CLoginTokenMgr* CLoginTokenMgr::m_pLoginTokenMgr = NULL;

CLoginTokenMgr::CLoginTokenMgr():m_timerRetry(0)
	, m_userId(0)
{
	memset(m_wszToken,NULL,sizeof(m_wszToken));
    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        connect(pCallback,SIGNAL(RecvAutoLoginToken(AutoLoginTokenInfo)),this,SLOT(OnRecvLoginToken(AutoLoginTokenInfo)));
    }
}

CLoginTokenMgr::~CLoginTokenMgr()
{	
}

CLoginTokenMgr * CLoginTokenMgr::GetInstance()
{
	if(NULL == m_pLoginTokenMgr)
	{
		m_pLoginTokenMgr = new CLoginTokenMgr;
	}

	return m_pLoginTokenMgr;
}

void CLoginTokenMgr::FreeInstance()
{
	if(NULL == m_pLoginTokenMgr)
	{
		return;
	}

    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        disconnect(pCallback,NULL,m_pLoginTokenMgr,NULL);
    }
	delete m_pLoginTokenMgr;
	m_pLoginTokenMgr = NULL;

	return;
}

bool CLoginTokenMgr::GetLoginToken(__int64 nUserId,LPWSTR pwszToken)
{
	if(NULL != m_wszToken[0])
	{
		if(NULL == pwszToken)
		{
			return false;
		}

		wcscpy(pwszToken,m_wszToken);
		memset(m_wszToken,NULL,sizeof(m_wszToken));

		return true;
	}

	biz::GetBizInterface()->QueryAutoLoginToken(nUserId);
	m_timerRetry = this->startTimer(3000);
	m_userId = nUserId;

	return false;
}

void CLoginTokenMgr::OnRecvLoginToken(AutoLoginTokenInfo info)
{
	if(  0 == info._retCode || NULL== info._pwszToken || NULL == info._pwszToken[0])
	{
		return;
	}

	if(m_timerRetry>=0){
		this->killTimer(m_timerRetry);
		m_timerRetry = 0;
	}

	wcscpy_s(m_wszToken,info._pwszToken);
    emit recvLoginToken(QString::fromWCharArray(m_wszToken));

	return;
}

void CLoginTokenMgr::timerEvent(QTimerEvent *event)
{
	if(NULL==event){
		return;
	}

	int tid=event->timerId();
	if(tid==m_timerRetry){
		biz::GetBizInterface()->QueryAutoLoginToken(m_userId);
	}
}

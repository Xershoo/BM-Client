//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：UploadTokenMgr.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：提供文件上传Token获取和管理的类定义
//	修改记录：   
//**********************************************************************

#include "UploadTokenMgr.h"
#include "biz/interface/interface.h"
#include "runtime.h"
#include "BizInterface.h"

#define DEF_SAVE_TOKEN_NUM		(10)

CUploadTokenMgr* CUploadTokenMgr::m_pInstance = NULL;

CUploadTokenMgr::CUploadTokenMgr()
{
    m_nTimerId = this->startTimer(10000);
    m_nRecvIdBase = 0;
    m_nUserId = 0;
    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        connect(pCallback,SIGNAL(RecvUploadToken(UploadTokenInfo)),this,SLOT(OnRecvToken(UploadTokenInfo)));
    }
}

CUploadTokenMgr::~CUploadTokenMgr()
{
	FreeTokenInfo();

	if(m_nTimerId!= 0)
	{
		this->killTimer(m_nTimerId);
		m_nTimerId = 0;
	}	
}

CUploadTokenMgr* CUploadTokenMgr::GetInstance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CUploadTokenMgr();
	}

	return m_pInstance;
}

void CUploadTokenMgr::Release()
{
	if(NULL == m_pInstance)
	{
		return;
	}

    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        disconnect(pCallback,NULL,m_pInstance,NULL);
    }

	delete m_pInstance;
	m_pInstance = NULL;

	return;
}

void CUploadTokenMgr::FreeTokenInfo()
{
	QMutexLocker autoLock(&m_lockToken);

	for(int i=0;i<m_vecTokenMsg.size();i++)
	{
		UINT nId = m_vecTokenMsg.at(i);

        emit recvToken(nId,GET_TOKEN_FAILED,QString(""));
	}

	m_vecTokenMsg.clear();
	m_vecTokenInfo.clear();
}

int CUploadTokenMgr::GetToken(char* pszToken,__int64 nUserId /* = 0 */)
{
	QMutexLocker autoLock(&m_lockToken);
	if(!m_vecTokenInfo.empty())
	{
		if(NULL == pszToken)
		{
			return false;
		}

		TOKENINFO ti = m_vecTokenInfo.front();
		strcpy(pszToken,ti.szToken);
		m_vecTokenInfo.erase(m_vecTokenInfo.begin());
		
		return 0;
	}

    m_nRecvIdBase ++;
    
    if(nUserId != 0)
    {
        m_nUserId = nUserId;
    }

    if(m_nUserId != 0)
	{
        biz::GetBizInterface()->QueryUploadToken(m_nUserId);
    }

	m_vecTokenMsg.push_back(m_nRecvIdBase);
	return m_nRecvIdBase;
}

void CUploadTokenMgr::timerEvent(QTimerEvent *event)
{
    if(m_nUserId == 0)
    {
        return;
    }

	QMutexLocker autoLock(&m_lockToken);
	
	// 删除失效的TOKEN
	for(TokenInfoVector::iterator itr = m_vecTokenInfo.begin();itr!=m_vecTokenInfo.end();)
	{
		TOKENINFO ti = *itr;
		DWORD    tNow = run_time::getRunTime();
		if(tNow - ti.nObtainTime >= ti.nValidTime * 1000)
		{
			itr = m_vecTokenInfo.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	if(m_vecTokenInfo.size() >= DEF_SAVE_TOKEN_NUM )
	{
		return;
	}

	biz::GetBizInterface()->QueryUploadToken(m_nUserId);
}

void CUploadTokenMgr::OnRecvToken(UploadTokenInfo tokenInfo)
{
	if(tokenInfo._retCode != 0 || NULL == tokenInfo._pszToken || NULL ==tokenInfo._pszToken[0])
	{
		biz::GetBizInterface()->QueryUploadToken(tokenInfo._nUserId);
		return;
	}

	QMutexLocker autoLock(&m_lockToken);
	if(!m_vecTokenMsg.empty())
	{
		UINT nId = m_vecTokenMsg.front();
        emit recvToken(nId,GET_TOKEN_OK,QString(tokenInfo._pszToken));
		
        m_vecTokenMsg.erase(m_vecTokenMsg.begin());

		return;
	}

	TOKENINFO ti;

	ti.nObtainTime = run_time::getRunTime();
	ti.nValidTime = tokenInfo._nValidateTime;
	strcpy_s(ti.szToken,tokenInfo._pszToken);

	m_vecTokenInfo.push_back(ti);
}
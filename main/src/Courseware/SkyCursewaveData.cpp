//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：SkyCursewaveData.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.7
//	说明：资料库数据管理类
//	修改记录：
//  备注：
//**********************************************************************

#include "SkyCursewaveData.h"
#include "session/classsession.h"
#include "BizInterface.h"
#include "macros.h"

CSkyCursewaveData* CSkyCursewaveData::m_instance = NULL;

CSkyCursewaveData* CSkyCursewaveData::getInstance()
{
    if(NULL==m_instance)
    {
        m_instance = new CSkyCursewaveData;
    }

    return m_instance;
}

void CSkyCursewaveData::freeInstance()
{
    SAFE_DELETE(m_instance);
}

CSkyCursewaveData::CSkyCursewaveData()
{
    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        connect(pCallback, SIGNAL(RecvPrevCoursewareList(PrevCoursewareListInfo)), this, SLOT(addCoursewareList(PrevCoursewareListInfo)));
    }
}

CSkyCursewaveData::~CSkyCursewaveData()
{
    ClearAll();
}

void CSkyCursewaveData::addCoursewareList(PrevCoursewareListInfo pInfo)
{
    if(0 == pInfo._cwList.size())
    {
        return;
    }

    if(pInfo._nCourseId != ClassSeeion::GetInst()->_nCourseId)
    {
        return;
    }

    for (size_t i = 0; i < pInfo._cwList.size(); i++)
    {
        AddSkyCursewave(pInfo._cwList[i]._wszName, pInfo._cwList[i]._wszFile);
    }
}

bool CSkyCursewaveData::AddSkyCursewave(LPCWSTR pszFileName, LPCWSTR pszFileUrl)
{
	if (NULL == pszFileName || NULL == pszFileUrl)
	{
		return false;
	}
	LPSKYCURSEWAVE pData = new SKYCURSEWAVE;
	
    wcscpy_s(pData->m_szFileName, pszFileName);
    wcscpy_s(pData->m_szFileUrl,  pszFileUrl);
	
	m_vecSkyCursewave.push_back(pData);
	
	return true;
}

bool CSkyCursewaveData::SetSel(LPCWSTR pszFileName, bool bSel)
{
	if (NULL == pszFileName)
	{
		return false;
	}

	LPSKYCURSEWAVE pData = NULL;
	for (size_t i = 0; i < m_vecSkyCursewave.size(); i++)
	{
        pData = m_vecSkyCursewave.at(i);
        if(NULL == pData)
        {
            continue;
        }

		if (0 == wcscmp(pszFileName, pData->m_szFileName))
		{
            if(pData->m_bISSel)
            {
                pData = NULL;
            }
            else
			{
                pData->m_bISSel = bSel;			
            }

			break;
		}

        pData = NULL;
	}

	return pData ? true : false;
}

bool CSkyCursewaveData::ClearAll()
{
	if (m_vecSkyCursewave.size() == 0)
	{
		return false;
	}

	for (size_t i = 0; i < m_vecSkyCursewave.size(); i++)
	{
		LPSKYCURSEWAVE pData = m_vecSkyCursewave[i];
		SAFE_DELETE(pData);
	}

	m_vecSkyCursewave.clear();

	return true;
}

bool CSkyCursewaveData::ClearSelAll()
{
	int nKey = 0;
	
	for (size_t i = 0; i < m_vecSkyCursewave.size(); i++)
	{
		m_vecSkyCursewave[i]->m_bISSel = false;
	}

	return true;
}

SkyCursewaveVector * CSkyCursewaveData::GetSkyCoursewareVector()
{
    if (0 == m_vecSkyCursewave.size())
    {
        return NULL;
    }

    return &m_vecSkyCursewave;
}

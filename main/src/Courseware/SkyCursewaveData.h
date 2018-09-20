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

#pragma once

#include <vector>
#include <map>
#include <QObject>
#include "../varname.h"
#include "BizCallBack.h"

using namespace std;

typedef struct tagSkyCursewaveData
{
	tagSkyCursewaveData()
	{
		memset(m_szFileName, 0, sizeof(m_szFileName));
		memset(m_szFileUrl , 0, sizeof(m_szFileUrl) );
		m_bISSel = false;
	}

	wchar_t	m_szFileName[MAX_PATH];
	wchar_t	m_szFileUrl[MAX_PATH];
	bool	m_bISSel;
	
}SKYCURSEWAVE, *LPSKYCURSEWAVE;


typedef vector <LPSKYCURSEWAVE> SkyCursewaveVector;

class CSkyCursewaveData : public QObject
{
    Q_OBJECT
public:
    static CSkyCursewaveData* getInstance();
    static void freeInstance();

	CSkyCursewaveData();
	virtual ~CSkyCursewaveData();

	bool AddSkyCursewave(LPCWSTR pszFileName, LPCWSTR pszFileUrl);
	bool SetSel(LPCWSTR pszFileName, bool bSel);
	bool ClearAll();
	bool ClearSelAll();

    SkyCursewaveVector *GetSkyCoursewareVector();
public slots:
    void addCoursewareList(PrevCoursewareListInfo pInfo);

protected:    
	SkyCursewaveVector	m_vecSkyCursewave;

private:
    static CSkyCursewaveData* m_instance;
};
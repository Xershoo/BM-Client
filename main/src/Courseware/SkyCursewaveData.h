//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�SkyCursewaveData.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.7
//	˵�������Ͽ����ݹ�����
//	�޸ļ�¼��
//  ��ע��
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
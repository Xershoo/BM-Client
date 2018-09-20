//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：CLSLoginUserManager.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.11.25
//	说明：历史用户管理
//	修改记录：
//  备注：
//**********************************************************************

#include "CLSLoginUserManager.h"
#include <algorithm>
#include <time.h>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QMutexLocker>
#include "././common/Util.h"
#include "common/Env.h"
#include "common/Util.h"
#include "setdebugnew.h"
#define MAX_PATH  260

typedef unsigned char       BYTE;
typedef unsigned long		DWORD;


typedef struct tagSLHistoryFileHeader 
{
	tagSLHistoryFileHeader()
	{
		nVersion = 0;
		nLastWriteTime = 0;
		nCount = 0;
		nBegin = 0;
		memset(szMac, 0, sizeof(szMac));
		memset(this, 0, sizeof(SLHistoryFileHeader));
	}
	BYTE	nVersion;	
	time_t	nLastWriteTime;
	BYTE	szMac[6];
	int		nCount;
	BYTE	nBegin;
}SLHistoryFileHeader;

const int g_nLoginSaveAccountMax	= 10;	//保存账号数，以最近登录时间为排序
const int g_nAccountErrorTimes		= 3;	//账号只让错误三次
const int g_nForbidTimeLen			= 300;	//账号封锁时间，单位秒

#define CLSUSERFILE		("cls8loginhistory.tmp")

bool SortAccount(LPCLSLOGINHISTORY account1, LPCLSLOGINHISTORY account2)
{
	if(account1->nLastLoginTime == account2->nLastLoginTime)
	{
		return strncmp(account1->szAccount, account2->szAccount, MAX_ACCOUNT_LEN) < 0;
	}
	else
		return account1->nLastLoginTime > account2->nLastLoginTime;
}

CLoginUserManager::CLoginUserManager()
{
	LoadLoginHistory();
}

CLoginUserManager::~CLoginUserManager()
{
	//SaveLoginHistory();
    Util::releaseArray(m_vecLoginHistory);
    Util::releaseArray(m_vecErrorUser);
}

bool CLoginUserManager::LoadLoginHistory()
{
    QMutexLocker AutoLock(&m_mutex);
	Util::releaseArray(m_vecLoginHistory);
    AutoLock.unlock();

    Util::IsExistDir(QString("%1\\Users").arg(Env::getExePath()));
    QString strPath = QString("%1\\Users\\\%2").arg(Env::getExePath()).arg(CLSUSERFILE);

	QFile file(strPath);
	if (!file.open(QIODevice::ReadWrite))
	{
		qDebug() << "open file(" << strPath << ") failed.";
		return false;
	}
	SLHistoryFileHeader FileHeader;
	DWORD dwRead = 0;
	bool bDelete = false;
	
	dwRead = file.read((char*)(&FileHeader),sizeof(SLHistoryFileHeader));
	if (dwRead != sizeof(SLHistoryFileHeader))
	{
		bDelete = true;
	}
	if (memcmp(FileHeader.szMac, "", 6) != 0)
	{
		bDelete = true;
	}
	for (int i = 0; i < FileHeader.nCount; i++)
	{
		CLSLOGINHISTORY *data = new CLSLOGINHISTORY;
		dwRead = file.read((char*)(data),sizeof(CLSLOGINHISTORY));
		if (dwRead != sizeof(CLSLOGINHISTORY))
		{
			qDebug() << "user history read error";
			break;
		}
        AutoLock.relock();
		m_vecLoginHistory.push_back(data);
        AutoLock.unlock();
	}
	file.close();
	
    AutoLock.relock();
	std::sort(m_vecLoginHistory.begin(), m_vecLoginHistory.end(), SortAccount);
    AutoLock.unlock();

	return true;
}

bool CLoginUserManager::SaveLoginHistory()
{
    QMutexLocker AutoLock(&m_mutex);
	std::sort(m_vecLoginHistory.begin(), m_vecLoginHistory.end(), SortAccount);
	if (m_vecLoginHistory.size() > g_nLoginSaveAccountMax)
    {
        for (auto it = m_vecLoginHistory.begin() + g_nLoginSaveAccountMax; it != m_vecLoginHistory.end(); it++)
        {
            delete (*it);
        }
        m_vecLoginHistory.resize(g_nLoginSaveAccountMax);
    }
    AutoLock.unlock();

	Util::IsExistDir(QString("%1\\Users").arg(Env::getExePath()));
    QString strPath = QString("%1\\Users\\\%2").arg(Env::getExePath()).arg(CLSUSERFILE);    
	
	QFile file(strPath);
	if (!file.open(QIODevice::ReadWrite))
	{
		qDebug() << "open file(" << strPath << ") failed.";
		return false;
	}
	SLHistoryFileHeader FileHeader;
	FileHeader.nCount = m_vecLoginHistory.size();
	FileHeader.nLastWriteTime = time(NULL);
	FileHeader.nVersion = 0;
	memcmp(FileHeader.szMac, "", 6);
	DWORD dwWrite = 0;
	
	dwWrite = file.write((char*)(&FileHeader), sizeof(SLHistoryFileHeader));
	
    AutoLock.relock();
	for (int i = 0; i < FileHeader.nCount; i++)
	{
		LPCLSLOGINHISTORY data = m_vecLoginHistory[i];
		file.write((char*)(data), sizeof(CLSLOGINHISTORY));
	}
    AutoLock.unlock();

	file.close();
	return true;
}

bool CLoginUserManager::UpdateAccountInfo(LPCLSLOGINHISTORY sInfo)
{
	bool bFind = false;
    LPCLSLOGINHISTORY accountInfo;
    QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecLoginHistory.size(); i++)
	{
		if (strcmp(sInfo->szAccount, m_vecLoginHistory[i]->szAccount) == 0)
		{
			memcpy(m_vecLoginHistory[i], sInfo, sizeof(CLSLOGINHISTORY));
			bFind = true;
			break;
		}
		/*else
		{
			//if (m_vecLoginHistory[i].loginStyle > UserLogStyle_PW)
			m_vecLoginHistory[i].loginStyle = m_vecLoginHistory[i].loginStyle > UserLogStyle_PW ? UserLogStyle_PW : m_vecLoginHistory[i].loginStyle;
		}*/
	}
	if (!bFind)
    {
        accountInfo = new CLSLOGINHISTORY(*sInfo);
		m_vecLoginHistory.push_back(accountInfo);
    }
    AutoLock.unlock();
	return true;
}

bool CLoginUserManager::DeleteAccountInfo(char * pszAccount)
{
	if (NULL == pszAccount)
	{
		return false;
	}
	
    QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecLoginHistory.size(); i++)
	{
		if (strcmp(pszAccount, m_vecLoginHistory[i]->szAccount) == 0)
		{
            delete m_vecLoginHistory.at(i);
			m_vecLoginHistory.erase(m_vecLoginHistory.begin() + i);
            AutoLock.unlock();
			return true;
		}
	}
    AutoLock.unlock();
	return false;
}

bool CLoginUserManager::UpdateErrorAccount(char * pszAccount)
{
	LPCLSERRORUSERLOGIN pErrorInfo = GetErrorAccountInfo(pszAccount);
	if (pErrorInfo)
	{
		pErrorInfo->nErrorTiems++;
		pErrorInfo->nLastLoginTime = time(NULL);
	}
	else
	{
		LPCLSERRORUSERLOGIN NewErrorInfo = new CLSERRORUSERLOGIN;
        NewErrorInfo->nErrorTiems = 1;
        NewErrorInfo->nLastLoginTime = time(NULL);
		sprintf(NewErrorInfo->szAccount, "%s", pszAccount);
        QMutexLocker AutoLock(&m_mutex);
		m_vecErrorUser.push_back(NewErrorInfo);
        AutoLock.unlock();
	}
	return true;
}

bool CLoginUserManager::AccountIsForbid(char * pszAccount)
{
	LPCLSERRORUSERLOGIN pErrorInfo = GetErrorAccountInfo(pszAccount);
	if (pErrorInfo)
	{
		if (pErrorInfo->nErrorTiems < g_nAccountErrorTimes)
			return false;
		time_t nowTime = time(NULL);
		if (nowTime - pErrorInfo->nLastLoginTime > g_nForbidTimeLen)
		{
			DeleteErrorAccount(pszAccount);
			return false;
		}
		return true;
	}
	return false;
}

void CLoginUserManager::DeleteErrorAccount(char * pszAccount)
{
	if (pszAccount == NULL)
		return;
	char szAccount[MAX_ACCOUNT_LEN] = {0};
	strcpy_s(szAccount, pszAccount);

    QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecErrorUser.size(); i++)
	{
		LPCLSERRORUSERLOGIN pErrorInfo = m_vecErrorUser[i];
		if (strcmp(pErrorInfo->szAccount, szAccount)==0)
		{
			m_vecErrorUser.erase(m_vecErrorUser.begin()+i);
            AutoLock.unlock();
            delete pErrorInfo;
            pErrorInfo = NULL;
			return;
		}
	}
    AutoLock.unlock();
}

LPCLSLOGINHISTORY CLoginUserManager::GetAccountByIndex(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_vecLoginHistory.size())
		return NULL;
	else
		return m_vecLoginHistory[nIndex];
}

LPCLSLOGINHISTORY CLoginUserManager::GetAccount(char * pszAccount)
{
	if (pszAccount == NULL)
		return NULL;
	char szAccount[MAX_ACCOUNT_LEN] = {0};
	strcpy_s(szAccount, pszAccount);
    QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecLoginHistory.size(); i++)
	{
		int nres = strcmp(szAccount, m_vecLoginHistory[i]->szAccount);
		if ( nres == 0)
		{
            AutoLock.unlock();
			return m_vecLoginHistory[i];
		}
	}
    AutoLock.unlock();
	return NULL;
}

LPCLSERRORUSERLOGIN CLoginUserManager::GetErrorAccountInfo(char * pszAccount) 
{
	if (pszAccount == NULL)
		return NULL;
	char szAccount[MAX_ACCOUNT_LEN] = {0};
	strcpy_s(szAccount, pszAccount);
    QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecErrorUser.size(); i++)
	{
		if (strcmp(szAccount, m_vecErrorUser[i]->szAccount) == 0)
		{
            AutoLock.unlock();
			return m_vecErrorUser[i];
		}
	}
    AutoLock.unlock();
	return NULL;
}
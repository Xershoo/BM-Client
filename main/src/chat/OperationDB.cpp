//ChatMGR.cpp
#include<stdio.h>
#include "OperationDB.h"


COperationDB::COperationDB()
{
	m_sqlConnect = nullptr;
}

COperationDB::~COperationDB()
{
	m_sqlConnect = NULL;
}

bool COperationDB::Init(LPSTR pszDBName, LPSTR pszError)
{
	if (NULL == pszDBName)
	{
		m_sqlConnect = NULL;
		return false;
	}
	//open database
	int nRes = sqlite3_open(pszDBName, &m_sqlConnect);
	if (SQLITE_OK != nRes)
	{
		sprintf(pszError, "open database failed");
		m_sqlConnect = NULL;
		return false;
	}
	return true;
}

bool COperationDB::UnInit()
{
	if (NULL == m_sqlConnect)
	{
		return false;
	}
	if (SQLITE_OK != sqlite3_close(m_sqlConnect))
	{
		return false;
	}
	m_sqlConnect = NULL;
	return true;
}

int COperationDB::Write(LPSTR pszSQL, LPSTR pszError)
{
	if (NULL == pszSQL || NULL == m_sqlConnect)
	{
		return 1;
	}
	int nRes = 0;
	nRes = sqlite3_exec(m_sqlConnect, pszSQL, 0, 0, &pszError);
	if (nRes)
	{
		sprintf(pszError,"%s",sqlite3_errmsg(m_sqlConnect));
		return 1;
	}
	return 0;
}

int COperationDB::Read(LPSTR pszSQL, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError)
{
	if (NULL == pszSQL || NULL == CallBackFunc || NULL == m_sqlConnect)
	{
		return 1;
	}
	int nRes = 0;
	nRes = sqlite3_exec(m_sqlConnect, pszSQL, CallBackFunc, pUserData, &pszError);
	if (nRes)
	{
		sprintf(pszError,"%s",sqlite3_errmsg(m_sqlConnect));
		return 1;
	}
	return 0;
}


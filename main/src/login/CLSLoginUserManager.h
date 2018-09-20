//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：CLSLoginUserManager.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.11.25
//	说明：历史用户管理
//	修改记录：
//  备注：
//**********************************************************************

#pragma once

//#include "GTEnumDefs_im.h"
#include <vector>
#include <QMutex>
#include "CL8_EnumDefs.h"

#define MAX_ACCOUNT_LEN		(64)
#define MAX_PASSWORD_LEN	(128)


//登录用户
typedef struct tagLoginHistory
{
	tagLoginHistory()
	{
		memset(szAccount, 0, sizeof(szAccount));
		memset(szPassword, 0, sizeof(szPassword));

		nLastLoginTime = 0;
		loginStyle = UserLogStyle_None;
	}

    tagLoginHistory(const tagLoginHistory &other)
    {        
        loginStyle = other.loginStyle;
        nLastLoginTime = other.nLastLoginTime;

        strcpy_s(szAccount, other.szAccount);
        strcpy_s(szPassword, other.szPassword);
    }
	
    time_t	nLastLoginTime;
	char	szAccount[MAX_ACCOUNT_LEN];
	char	szPassword[MAX_PASSWORD_LEN];	

	EnmuserLoginStyle loginStyle;

}CLSLOGINHISTORY, *LPCLSLOGINHISTORY;

//记录错误用户登陆的
typedef struct tagCLSErrorUserLogin 
{
	tagCLSErrorUserLogin()
	{
		memset(szAccount, 0, sizeof(szAccount));
		nErrorTiems = 0;
		nLastLoginTime = 0;
	}
	tagCLSErrorUserLogin(const tagCLSErrorUserLogin &other)
    {
        nErrorTiems = other.nErrorTiems;
        nLastLoginTime = other.nLastLoginTime;
        memcpy(szAccount, other.szAccount, MAX_ACCOUNT_LEN);
    }
	int			nErrorTiems;
	__int64		nLastLoginTime;
    char		szAccount[MAX_ACCOUNT_LEN];
}CLSERRORUSERLOGIN, *LPCLSERRORUSERLOGIN;

typedef std::vector <LPCLSLOGINHISTORY> ClsLoginHistoryVec;
typedef std::vector <LPCLSERRORUSERLOGIN> ClsErrorUserLogin;


class CLoginUserManager
{
public:
	CLoginUserManager();
	~CLoginUserManager();

public:
	bool SaveLoginHistory();
	bool LoadLoginHistory();

	bool UpdateAccountInfo(LPCLSLOGINHISTORY sInfo);
	bool DeleteAccountInfo(char * pszAccount);
	bool UpdateErrorAccount(char * pszAccount);
	bool AccountIsForbid(char * pszAccount);
	void DeleteErrorAccount(char * pszAccount);

	ClsLoginHistoryVec&	GetLoginHistory() {return m_vecLoginHistory; }
	LPCLSLOGINHISTORY	GetAccountByIndex(int nIndex);
	LPCLSLOGINHISTORY	GetAccount(char * pszAccount);

protected:


private:
	CLSERRORUSERLOGIN* GetErrorAccountInfo(char * pszAccount);

private:
	ClsLoginHistoryVec	m_vecLoginHistory;
	ClsErrorUserLogin	m_vecErrorUser;
    QMutex  m_mutex;
};
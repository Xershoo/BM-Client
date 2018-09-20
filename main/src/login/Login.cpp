//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：Login.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.11.25
//	说明：用户用户登录
//	修改记录：
//  备注：
//**********************************************************************

#include "Login.h"
#include <QDebug>
#include <QDateTime>
#include "login/CLSLoginUserManager.h"
#include "CL8_EnumDefs.h"
#include "biz/interface/data.h"
#include "biz/interface/interface.h"
#include "./././common/Util.h"

#include "setdebugnew.h"
//#include "StringHelper.h"

CLogin::CLogin()
{
    m_bisAutoLogin = false;
    m_bisRemePWD = false;
    m_bHisAutoLogin = false;
    m_loginByTokenUid = false;
    m_strUserName = "";
    m_strUserPWD = "";
}

CLogin::~CLogin()
{

}

bool CLogin::BeginLoginToWeb(__int64 uid , QString token)
{
    if (token.isEmpty() || uid <= 0)
    {
        return false;
    }
    
    biz::GetBizInterface()->Login(wstring((wchar_t*)(token).unicode()).data(), uid);
    return true;
}

bool CLogin::BeginLoginToDlg(QString strUser, QString strPwd, QString &strError)
{
	m_strUserName = strUser;
	m_strUserPWD = strPwd;
	
	if (m_strUserName.isEmpty() || m_strUserPWD.isEmpty())
	{
		//不允许用户和密码为空
        strError = QString::fromWCharArray(L"不允许用户和密码为空");
		return false;
	}
	
	if (m_strUserPWD.length() < g_nPWDMinLength)
	{
		//密码不少于（g_nPWDMinLength）位
		QString strContent = QString(QString::fromWCharArray(L"密码不少于%1位")).arg(g_nPWDMinLength);
        strError = strContent;
	
		return false; 
	}

    char szUser[MAX_ACCOUNT_LEN] = {0};
    
	Util::QStringToChar(strUser, szUser,MAX_ACCOUNT_LEN);
	strcpy_s(m_nowLoginUser.szAccount,szUser);
	
	int nLoginStyle;
	QString strPassword;
	if (findHistoryLogin(nLoginStyle, strPassword) && m_strUserPWD == strPassword.left(g_nShowPwdLen))
	{
        char szPwd[MAX_PASSWORD_LEN] = {0};
        Util::QStringToChar(strPassword, szPwd,MAX_PASSWORD_LEN);
		
		strcpy_s(m_nowLoginUser.szPassword, szPwd);		
	}
	else
	{
		//密码加密处理
		char szMD5[MAX_PATH] = {0};
        QString strMd5;

        strMd5 = Util::CaleBufferMd5(m_strUserPWD);
		Util::QStringToChar(strMd5, szMD5,MAX_PATH);

        strcpy_s(m_nowLoginUser.szPassword, szMD5);		
	}

	if (m_bisAutoLogin)
	{
		m_nowLoginUser.loginStyle = UserLogStyle_Auto;
	}
	else if(m_bisRemePWD)
	{
		m_nowLoginUser.loginStyle = UserLogStyle_PW;
	}
	else
	{
		m_nowLoginUser.loginStyle = UserLogStyle_None;
	}

	m_nowLoginUser.nLastLoginTime = (time_t)QDateTime::currentDateTime().toTime_t();

	//发送登录消息
    WCHAR wszUserName[MAX_ACCOUNT_LEN+1] = { 0 };
	WCHAR wszUserPwd[MAX_PASSWORD_LEN+1] = { 0 };
	
	Util::AnsiToUnicode(m_nowLoginUser.szAccount,strlen(m_nowLoginUser.szAccount),wszUserName,MAX_ACCOUNT_LEN);
	Util::AnsiToUnicode(m_nowLoginUser.szPassword,strlen(m_nowLoginUser.szPassword),wszUserPwd,MAX_PASSWORD_LEN);

    biz::GetBizInterface()->Login(wszUserName, wszUserPwd);
    
	return true;
}

bool CLogin::loginOk()
{
    bool bRes = false;
    if (m_bHisAutoLogin)
    {
        m_bHisAutoLogin = false;
    }

    char szUser[MAX_ACCOUNT_LEN] = {0};
    Util::QStringToChar(m_strUserName, szUser,MAX_ACCOUNT_LEN);
    LPCLSLOGINHISTORY login = m_logMag.GetAccount(szUser);
    if (NULL == login)
    {
        bRes = true;
    }
    m_logMag.UpdateAccountInfo(&m_nowLoginUser);
    m_logMag.SaveLoginHistory();
         
    return bRes;
}

bool CLogin::loginError(int nErrorCode, QString &strError)
{
    switch (nErrorCode)
	{
	case biz::AU_INVALID_USERNAME:
	case biz::AU_INVALID_PASSWORD:
		{
			//您输入的用户名或密码错误，请重新输入
			qDebug() << QString::fromWCharArray(L"您输入的用户名或密码错误，请重新输入");
			strError = QString::fromWCharArray(L"您输入的用户名或密码错误，请重新输入");
		}
		break;

	case biz::AU_SERVERERROR:
		{
			//服务器连接失败，请稍后再试
			qDebug() << QString::fromWCharArray(L"服务器连接失败，请稍后再试");
            strError = QString::fromWCharArray(L"服务器连接失败，请稍后再试");
		}
		break;

	case biz::Had_Teacher:
		{
			//您已登录此账号，不能重复登录
			qDebug() << QString::fromWCharArray(L"您已登录此账号，不能重复登录");
            strError = QString::fromWCharArray(L"您已登录此账号，不能重复登录");
		}
		break;

	case biz::AU_ACCOUNT_FREEZE:
		{
			//您的账号已被冻结，详情请联系管理员
			qDebug() << QString::fromWCharArray(L"您的账号已被冻结，详情请联系管理员");
            strError = QString::fromWCharArray(L"您的账号已被冻结，详情请联系管理员");
		}
		break;

	case biz::AU_ACCOUNT_LOCKING:
		{
			//您的密码错误已达到5次，账号已被锁定
			qDebug() << QString::fromWCharArray(L"您的密码错误已达到5次，账号已被锁定");
            strError = QString::fromWCharArray(L"您的密码错误已达到5次，账号已被锁定");
		}
		break;

	default:
        {
            qDebug() << QString::fromWCharArray(L"其他错误");
            strError = QString::fromWCharArray(L"其他错误");
        }
		break;
	}

    return true;
}

bool CLogin::logout()
{
    return true;
}

bool CLogin::findHistoryLogin(int &nLoginStyle, QString &strPWD)
{
    char szUser[MAX_ACCOUNT_LEN] = {0};

    Util::QStringToChar(m_strUserName, szUser,MAX_ACCOUNT_LEN);
    LPCLSLOGINHISTORY lpLogin = m_logMag.GetAccount(szUser);

    if (lpLogin == NULL || strlen(lpLogin->szPassword) < 1 )
    {
        return false;
    }

    char szPwd[MAX_PASSWORD_LEN] = {0};
    Util::QStringToChar(m_strUserPWD, szPwd,MAX_PASSWORD_LEN);

    if (strncmp(lpLogin->szPassword, szPwd, g_nShowPwdLen) != 0)
    {
        strPWD = "";
        return false;
    }

    strPWD = lpLogin->szPassword;
    nLoginStyle = lpLogin->loginStyle;

    return true;
}

bool CLogin::DeleteUser(QString strUser)
{
    if (strUser.isEmpty())
    {
        return false;
    }

    char szUser[MAX_ACCOUNT_LEN] = {0};
    Util::QStringToChar(strUser, szUser,MAX_ACCOUNT_LEN);
    CLSLOGINHISTORY *LogUser = m_logMag.GetAccount(szUser);
    if (LogUser == NULL)
    {
        m_bisAutoLogin = false;
        m_bisRemePWD = false;
        return false;
    }
    m_logMag.DeleteAccountInfo(szUser);
    m_logMag.SaveLoginHistory();
    return true;
}

bool CLogin::SelectUser(QString strUser, QString &strPwd, bool &bISAutoLogin, bool &bISKeepPwd)
{
    bISAutoLogin = bISKeepPwd = false;
    if (strUser.isEmpty())
    {
        return false;
    }
    char szUser[MAX_ACCOUNT_LEN] = {0};
    Util::QStringToChar(strUser, szUser,MAX_ACCOUNT_LEN);

    CLSLOGINHISTORY *LogUser = m_logMag.GetAccount(szUser);
    if (LogUser == NULL)
    {
        return false;
    }
    strPwd = LogUser->szPassword;
    if (LogUser->loginStyle == UserLogStyle_Auto)
    { 
        bISAutoLogin = true;
        bISKeepPwd = true;
    }
    else if (LogUser->loginStyle == UserLogStyle_PW)
    {
        bISKeepPwd = true;
    }
    return true;
}

bool CLogin::InitHistoryUser(QString (&strArr)[g_nRecordUserNum], int &nSize)
{
    ClsLoginHistoryVec vecLogHis = m_logMag.GetLoginHistory();
    for (size_t i = 0; i < vecLogHis.size() && i < g_nRecordUserNum; i++)
    {
        QString strUser(vecLogHis[i]->szAccount);

        strArr[i] = strUser;
    }
    nSize = vecLogHis.size();
    return true;
}

void CLogin::SetAutoLogin(bool bISAuto)
{
    m_bisAutoLogin = bISAuto;
    m_bisRemePWD = bISAuto == true ? true : m_bisRemePWD;
}

void CLogin::SetRemPwd(bool bISRem)
{
    m_bisRemePWD = bISRem;
    m_bisAutoLogin = bISRem == false ? false : m_bisAutoLogin;
}

bool CLogin::ISHistoryAccount(QString strUser)
{
    if (strUser.isEmpty())
    {
        return false;
    }
    char szUser[MAX_ACCOUNT_LEN] = {0};
    Util::QStringToChar(strUser, szUser,MAX_ACCOUNT_LEN);

    CLSLOGINHISTORY *LogUser = m_logMag.GetAccount(szUser);
    if (NULL == LogUser)
    {
        return false;
    }
    return true;
}

bool CLogin::GetNowUser(QString &strUser, QString &strPwd)
{
    strUser = m_strUserName;
    strPwd = m_strUserPWD;
    return true;
}
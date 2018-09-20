//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�Login.cpp
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.11.25
//	˵�����û��û���¼
//	�޸ļ�¼��
//  ��ע��
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
		//�������û�������Ϊ��
        strError = QString::fromWCharArray(L"�������û�������Ϊ��");
		return false;
	}
	
	if (m_strUserPWD.length() < g_nPWDMinLength)
	{
		//���벻���ڣ�g_nPWDMinLength��λ
		QString strContent = QString(QString::fromWCharArray(L"���벻����%1λ")).arg(g_nPWDMinLength);
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
		//������ܴ���
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

	//���͵�¼��Ϣ
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
			//��������û����������������������
			qDebug() << QString::fromWCharArray(L"��������û����������������������");
			strError = QString::fromWCharArray(L"��������û����������������������");
		}
		break;

	case biz::AU_SERVERERROR:
		{
			//����������ʧ�ܣ����Ժ�����
			qDebug() << QString::fromWCharArray(L"����������ʧ�ܣ����Ժ�����");
            strError = QString::fromWCharArray(L"����������ʧ�ܣ����Ժ�����");
		}
		break;

	case biz::Had_Teacher:
		{
			//���ѵ�¼���˺ţ������ظ���¼
			qDebug() << QString::fromWCharArray(L"���ѵ�¼���˺ţ������ظ���¼");
            strError = QString::fromWCharArray(L"���ѵ�¼���˺ţ������ظ���¼");
		}
		break;

	case biz::AU_ACCOUNT_FREEZE:
		{
			//�����˺��ѱ����ᣬ��������ϵ����Ա
			qDebug() << QString::fromWCharArray(L"�����˺��ѱ����ᣬ��������ϵ����Ա");
            strError = QString::fromWCharArray(L"�����˺��ѱ����ᣬ��������ϵ����Ա");
		}
		break;

	case biz::AU_ACCOUNT_LOCKING:
		{
			//������������Ѵﵽ5�Σ��˺��ѱ�����
			qDebug() << QString::fromWCharArray(L"������������Ѵﵽ5�Σ��˺��ѱ�����");
            strError = QString::fromWCharArray(L"������������Ѵﵽ5�Σ��˺��ѱ�����");
		}
		break;

	default:
        {
            qDebug() << QString::fromWCharArray(L"��������");
            strError = QString::fromWCharArray(L"��������");
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
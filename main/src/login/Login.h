//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：Login.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.11.25
//	说明：用户用户登录
//	修改记录：
//  备注：
//**********************************************************************

#pragma once

#include "CLSLoginUserManager.h"
#include "BizCallBack.h"

const int g_nShowPwdLen = 10;
const int g_nPWDMinLength = 6;
const int g_nRecordUserNum = 10;

class CLogin
{
public:
    CLogin();

    ~CLogin();

public:
    bool InitHistoryUser(QString (&strArr)[g_nRecordUserNum], int &nSize);
    bool BeginLoginToWeb(__int64 uid, QString token);
    bool BeginLoginToDlg(QString strUser, QString strPwd, QString &strError);
    bool loginOk();
    bool loginError(int nErrorCode, QString &strError);
    bool logout();
    bool findHistoryLogin(int &nLoginStyle, QString &strPWD);
    bool DeleteUser(QString strUser);
    bool SelectUser(QString strUser, QString &strPwd, bool &bISAutoLogin, bool &bISKeepPwd);

    void SetAutoLogin(bool bISAuto);
    void SetRemPwd(bool bISRem);
    bool ISHistoryAccount(QString strUser);
    bool GetNowUser(QString &strUser, QString &strPwd);

protected:

private:
    bool    m_bisAutoLogin;	    //自动登录
    bool    m_bisRemePWD;		//记住密码
    bool    m_bHisAutoLogin;	    //自动登录(历史账号状态)

    bool    m_loginByTokenUid;	//是否是网页登录

    QString m_strUserName;         //当前登录用户
    QString m_strUserPWD;          //当前登录密码

    CLoginUserManager	m_logMag;
    CLSLOGINHISTORY		m_nowLoginUser;

};


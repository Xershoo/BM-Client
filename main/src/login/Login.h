//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�Login.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.11.25
//	˵�����û��û���¼
//	�޸ļ�¼��
//  ��ע��
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
    bool    m_bisAutoLogin;	    //�Զ���¼
    bool    m_bisRemePWD;		//��ס����
    bool    m_bHisAutoLogin;	    //�Զ���¼(��ʷ�˺�״̬)

    bool    m_loginByTokenUid;	//�Ƿ�����ҳ��¼

    QString m_strUserName;         //��ǰ��¼�û�
    QString m_strUserPWD;          //��ǰ��¼����

    CLoginUserManager	m_logMag;
    CLSLOGINHISTORY		m_nowLoginUser;

};


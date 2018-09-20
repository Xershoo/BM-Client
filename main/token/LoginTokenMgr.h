//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：LoginTokenMgr.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：提供登录Token获取和管理的类定义
//	修改记录：
//  备注：
//      1.其他模块通过GetLoginToken接口获取登录Web页面Token，返回值为true时，表示成功取得Token,返回值为false时，表示需要等待网络获取Token，
//        通过connect recvLoginToken信号，获得Token值，并进行下一步操作
//      2.该类的初始化和GetLoginToken的调用，请在BIZ连接服务器和登录成功后
//      
//**********************************************************************

#ifndef LOGIN_TOKEN_MGR_H
#define LOGIN_TOKEN_MGR_H

#include "varname.h"
#include <QObject>
#include <vector>
#include "BizInterface.h"

using namespace std;
class CLoginTokenMgr : public QObject
{
    Q_OBJECT
public:
	CLoginTokenMgr();
	virtual ~CLoginTokenMgr();

	static CLoginTokenMgr* GetInstance();
	static void FreeInstance();

	bool	GetLoginToken(__int64 nUserId,LPWSTR pwszToken);
public slots:
	void	OnRecvLoginToken(AutoLoginTokenInfo info);

signals:
    void    recvLoginToken(QString);

private:
	virtual void timerEvent(QTimerEvent *event);
protected:
	WCHAR	m_wszToken[MAX_PATH];
	int		m_timerRetry;
	__int64	m_userId;

	static CLoginTokenMgr*	m_pLoginTokenMgr;
};

#endif
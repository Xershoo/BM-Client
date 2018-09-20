//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：UploadTokenMgr.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：提供文件上传Token获取和管理的类定义
//	修改记录：
//  备注：
//      1.其他模块通过GetToken接口获取文件上传Token，返回值为0时，表示成功取得Token,大于0时，表示需要等待网络获取Token，
//        保存返回值，并且connect recvToken信号，并判断第一个参数是否与返回值相等，如果相等，则取得该Token值，并进行下一步操作
//      2.该类的初始化和GetToken的调用，请在BIZ连接服务器和登录成功后
//      
//**********************************************************************
#ifndef UPLOAD_TOKEN_MGR_H
#define UPLOAD_TOKEN_MGR_H

#include "varname.h"
#include <vector>
#include <QtCore\QObject>
#include <QtCore\QMutex>
#include <QtCore\QString>
#include "BizInterface.h"

using namespace std;

#define TOKEN_LEN		(128)
typedef struct TokenInfo
{
public:
	TokenInfo()
	{
		memset(this,NULL,sizeof(TokenInfo));
	}

	TokenInfo& operator=(TokenInfo& ti)
	{
		this->nValidTime = ti.nValidTime;
		this->nObtainTime = ti.nObtainTime;

		strcpy_s(this->szToken,ti.szToken);

		return *this;
	}

	char		szToken[TOKEN_LEN];
	int			nValidTime;
	DWORD		nObtainTime;
}TOKENINFO,*LPTOKENINFO;

typedef vector<TOKENINFO>		TokenInfoVector;
typedef vector<UINT>		    GTokenMsgVector;

class CUploadTokenMgr : public QObject
{
    Q_OBJECT
public:
	enum
	{
		GET_TOKEN_OK =0,
		GET_TOKEN_FAILED
	};

public:
	CUploadTokenMgr();
	virtual ~CUploadTokenMgr();

	static CUploadTokenMgr* GetInstance();
	static void Release();

    inline void SetUserId(__int64 nUserId)
    {
        m_nUserId = nUserId;
    };

	int GetToken(char* pszToken,__int64 nUserId = 0);

protected slots:
	void OnRecvToken(UploadTokenInfo tokenInfo);

protected:
	void FreeTokenInfo();

signals:
    void recvToken(UINT nId,int ret, QString strToken);

protected:
    void timerEvent(QTimerEvent *event); 

protected:
	QMutex  m_lockToken;
    int     m_nTimerId;
    UINT    m_nRecvIdBase;
    __int64 m_nUserId;
    
	TokenInfoVector				m_vecTokenInfo;
	GTokenMsgVector				m_vecTokenMsg;

	static CUploadTokenMgr*	    m_pInstance;
};

#endif
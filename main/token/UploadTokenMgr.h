//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�UploadTokenMgr.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.23
//	˵�����ṩ�ļ��ϴ�Token��ȡ�͹�����ඨ��
//	�޸ļ�¼��
//  ��ע��
//      1.����ģ��ͨ��GetToken�ӿڻ�ȡ�ļ��ϴ�Token������ֵΪ0ʱ����ʾ�ɹ�ȡ��Token,����0ʱ����ʾ��Ҫ�ȴ������ȡToken��
//        ���淵��ֵ������connect recvToken�źţ����жϵ�һ�������Ƿ��뷵��ֵ��ȣ������ȣ���ȡ�ø�Tokenֵ����������һ������
//      2.����ĳ�ʼ����GetToken�ĵ��ã�����BIZ���ӷ������͵�¼�ɹ���
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
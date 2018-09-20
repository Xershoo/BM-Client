//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�LoginTokenMgr.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.23
//	˵�����ṩ��¼Token��ȡ�͹�����ඨ��
//	�޸ļ�¼��
//  ��ע��
//      1.����ģ��ͨ��GetLoginToken�ӿڻ�ȡ��¼Webҳ��Token������ֵΪtrueʱ����ʾ�ɹ�ȡ��Token,����ֵΪfalseʱ����ʾ��Ҫ�ȴ������ȡToken��
//        ͨ��connect recvLoginToken�źţ����Tokenֵ����������һ������
//      2.����ĳ�ʼ����GetLoginToken�ĵ��ã�����BIZ���ӷ������͵�¼�ɹ���
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
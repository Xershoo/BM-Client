// -------------------------------------------------------------------------
//	�ļ���		��	HttpDownloadUIInterface.h
//	������		��	yuyu
//	����ʱ��	��	2004-2-13 10:00:00
//	��������	��	�����ļ���UI������
//			
//			�ṩ�򵥵Ľӿڸ�ʹ����ʵ�ֽ������
//	
//  Updated: 2004-11-29 Mal �����ض���ص��ӿڣ��޸�ʹ��˵��
//-------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//
//	CHttpDownloadUIInterfaceʹ��˵��
//		
//		�ӿ���, ��Ҫ�̳�ʵ����Ҫ�Ľӿ�
//
//		1.��ʼ���ӷ�����
//			virtual void OnConnecting(LPCTSTR lpszServerAddr)
//
//			����:	lpszServerAddr	��������ַ
//
//		2.���ӷ������ɹ�
//			virtual void OnConnected()
//
//		3.�����ض���
//			virtual void OnRedirected(LPCTSTR lpszRedirectedAddr)
//
//			������lpszRedirectedAddr	�ض����Ŀ�ĵ�ַ
//
//		4.��ʼ�����ļ�
//			virtual void OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes)
//
//			����:	dwResumeBytes	��ʼ���ص��ļ�λ��. �ϵ�����ʱ��ʹ��
//					dwFileBytes		�ļ��Ĵ�С
//
//		5.���ؽ���
//			virtual void OnProgress(DWORD dwProgress, DWORD dwProgressMax)
//
//			����:	dwProgress		�Ѿ����صĴ�С. 
//					dwProgressMax	�ļ����ܴ�С
//		
//		6.�������
//			virtual void OnDownloadComplete(DWORD nErrorCode, LPCTSTR lpszSaveFile)
//
//			������nErrorCode	״̬���룬ȡֵERRORCODE_NULL��ERRORCODE_NOMODIFIED
//								��ʾHTTP���سɹ����������������޸Ķ�������������
//								ȡֵͬOnError��nCodeȡֵ����ʾ���������������
//			 ��   lpszSaveFile	���浽�ı����ļ���
//
//		7.���س��ֳ���
//			virtual void OnError(int nCode)
//
//			����:	nCode			Ϊ�������
//
//	 				DOWNLOAD_ERRORCODE_GENERAL:			����ʧ��
//					DOWNLOAD_ERRORCODE_SERVERCLOSE:		�������ر�����
//					DOWNLOAD_ERRORCODE_WRITEFAILED:		д���ļ�����, �ļ����򿪻��ߴ�������
//					DOWNLOAD_ERRORCODE_NETWORKFAILED:	�޷����ӷ�����
//
//////////////////////////////////////////////////////////////////////////


#if !defined(AFX_HTTPDOWNLOADUIINTERFACE_H__0F48BB62_B003_4B34_A9BE_1ABA4080368B__INCLUDED_)
#define AFX_HTTPDOWNLOADUIINTERFACE_H__0F48BB62_B003_4B34_A9BE_1ABA4080368B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHttpDownloadUIInterface
{	
public:
	virtual void OnConnecting(LPCTSTR lpszServerAddr) = 0;
	virtual void OnConnected() = 0;
	virtual void OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes) = 0;
	virtual void OnProgress(DWORD dwProgress, DWORD dwProgressMax,LPCTSTR lpszSaveFile) = 0;
	virtual void OnDownloadComplete(DWORD nErrorCode, LPCTSTR lpszSaveFile/*, LPCTSTR lpszResponseFileName*/) = 0;
	virtual void OnRedirected(LPCTSTR lpszRedirectedAddr) = 0;
};
#endif
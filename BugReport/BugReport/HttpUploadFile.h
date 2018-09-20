// HttpUploadFile.h: interface for the CHttpUploadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPUPLOADFILE_H__23C0420E_D345_4533_A496_1DA039899859__INCLUDED_)
#define AFX_HTTPUPLOADFILE_H__23C0420E_D345_4533_A496_1DA039899859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** CHttpUploadFile ͨ��HTTPЭ��ÿ���ϴ�һ���ļ����࣬����CHttpDownloadʵ��
 *  
 *  Mal Fox 2004.11
 *
 *  ʹ��˵��
 *  =========================
 *  1����һ����̳�CHttpDownloadUIInterface�������п�ѡ���Լ̳�On������
 *    
 *  2����ĳ������CHttpUploadFile�Ķ��󣬲�����SetUIInterfaceָ��֮ǰ׼���õ�
 *     CHttpDownloadUIInterface�ӿ�ʵ�������
 *
 *  3��ʹ��ResetFormData�����һ���ϴ����µ�Form����
 *
 *  4��ʹ��AddFormData����ϴ�ʱ������Form����
 *
 *  5��ʹ��UploadFile��ʼ�ϴ��ļ�������䷵��FALSE����ʾ�ϴ�ʧ��
 *
 *  6�����UploadFileû�з���FALSE�����CHttpDownloadUIInterface�ӿ�ʵ����
 *     �����OnDownloadCompleteӦ�ᱻ���ã������У���nErrorCode�����жϣ���
 *     ��֪�ϴ������Ƿ�ɹ���������Server�Ƿ����˻���Ŀǰ��Ҫ���յ������ݰ�
 *     ͷ�����ݽ��з����ſɵ�֪��
 *
 *  6.1����Server���ص���Ϣ��������һ��QueryInfo������ȡ��Ϣ��֧�ֳ�ȡ��
 *       ���ֶ����� + ��:�� + ��ֵ����ʽ��֯����Ϣ��֧��String��ԭʼ���ݣ���
 *       DWORD��BOOL�Լ�Buffer�ĳ�ȡ������ο�����˵����
 *
 *  7�����ϣ���ж��ϴ�������ʹ��CancelUpload������ע����ô˷����󲻻��ߵ�
 *     CHttpDownloadUIInterface�ӿ�ʵ��������OnDownloadComplete������
 *
 *  8�����ϣ����������FlashGetʹ�ø��࣬�ڹ������������
 *     �� _STANDALONE_USE_ ���ɡ�
 *
 */

#include "HttpDownload.h"

namespace CHTTPUPLOADFILE
{
	/// �ϴ�ʱʹ�õ�Buffer��С(bytes)
	const DWORD c_dwBufferSize = 10240; 

	// !!!�����޸Ĵ˲��ֵĸ�ʽ����!!!
	const TCHAR c_szFormRootFormat[]		= _T("Content-Type: multipart/form-data; boundary=%s\r\n");
	const TCHAR c_szFormDataFormat[]		= _T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n");
	const TCHAR c_szFormFileHeaderFormat[]	= _T("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: application/octet-stream\r\nContent-Transfer-Encoding: binary\r\n\r\n");
	const TCHAR c_szFormCompletionFormat[]	= _T("\r\n--%s--\r\n\r\n");
};

class CHttpUploadFile : protected CHttpDownload  
{
public:
	CHttpUploadFile();
	virtual ~CHttpUploadFile();
	/// �����ϴ��ļ���С���ƣ���λ��byte��
	void	SetFileSizeLimit(DWORD dwSize);
	/// ���Form����
	void	ResetFormData();
	/// ��Form���һ������ֵ
	void	AddFormData(const CString &strFieldName, 
						const CString &strValue);
	/// �ϴ��ļ�
	BOOL	UploadFile(	const CString &strField, 
						const CString &strURL, 
						const CString &strFile);
	/// �ж��ϴ�����
	void	CancelUpload();

	/// ��HTTP�ظ�ͷ����ȡ���ָ�ʽ�����ı�����ת������Ӧ��ʽ����ο�CHttpDownload
	CHttpDownload::QueryInfo;

	/// ��ȡ������Ϣͷ�� BOOL (CString &strResponseHeader);
	CHttpDownload::GetRawResponseHeader;

	/// �����¼��ص��ӿ� BOOL (CHttpDownloadUIInterface *pUIInterface);
	CHttpDownload::SetUIInterface;

	CHttpDownload::SetUseIEProxy;

	/// ������������
	CHttpDownload::SetNetwork;

	//////////////////////////////////////////////////////////////////////////
	// �ڲ�ʵ��
protected:
	/// ̽������������Ƿ���Ҫ�û�������
	BOOL	ProbeProxyAuthentication(CString &strProxyUserName, CString &strProxyPassword);
	CString	GetFormCompletion();
	int		GetFormCompletionLength();
	CString	GenerateRandomReadableString(int nLength = 6);
	CString	GenerateTempFileName();
	CString GetSystemTempPath();

	UINT	InternetWriteFile(BYTE *byarBuf, DWORD dwBufLen);
	
	void	ReadString(BYTE *byarBuf, CString &strTmp, DWORD *pdwReadSize);
	
	BOOL	OpenFile();
	inline	void CloseFileIfOpened();

	virtual UINT HttpSendRequest();
	
	UINT	BeginRequest();
	UINT	WriteRequest();
	UINT	EndRequest();

	UINT	IsAsyncIOSucceeded();

	virtual void OnProgress(DWORD dwIncrement);

	BYTE	m_byarBuf[CHTTPUPLOADFILE::c_dwBufferSize]; ///< �ϴ�������
	CString m_strForm;									///< Form����
	CString m_strBoundary;								///< �ָ���
	CString m_strFileToUpload;							///< �ϴ��ļ���
	HANDLE	m_hFile;									///< �ϴ��ļ�Handle
	DWORD	m_dwFileSizeLimit;							///< �ļ���С����(byte)
	
	DWORD	m_dwTotalWriteSize;							///< Ԥ��д���С
	DWORD	m_dwCurrentWriteSize;						///< Ŀǰд���С

	BOOL	m_bUploading;								///< �ϴ����

};

#endif // !defined(AFX_HTTPUPLOADFILE_H__23C0420E_D345_4533_A496_1DA039899859__INCLUDED_)

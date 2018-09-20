// -------------------------------------------------------------------------
//	�ļ���		��	HttpDownload.h
//	������		��	yuyu
//	����ʱ��	��	2004-2-13 10:00:00
//	��������	��	����Http Server���ļ�
//			
//	
//			֧�ֶϵ�����
//			֧�ֶ��ʧ������
//			֧���ض�������
//			֧��˫����, �ӳ�д��
//			֧�ֶ���ļ�ͬʱ����
//
//-------------------------------------------------------------------------


/**
 * 
 *   CHttpDownloadʹ��˵��:
 * 
 * 		1.�����ļ�
 * 		UINT	Download(LPCTSTR lpszDownloadUrl, SYSTEMTIME *tFileTime = NULL)
 * 		
 * 		����:	lpszDownloadUrl	Ҫ���ص�URL
 * 				tFileTime		������ʱ�䣬�Ա�Server���бȶ�
 * 
 * 		˵������ʹ��DLL��ʽ�ṩ��CHttpDownload��ʹ���ߣ��Ƽ�ʹ�ô˷����������Ȱ�
 * 			���ļ����ص�һ����ʱ�ļ��С���OnDownloadCompleteʱ�����Ի���䱣���
 * 			����ʱ�ļ������ٸ��Ƶ�Ŀ��λ�á�
 * 
 * 		-----------------------------------------------------------------------
 * 		UINT	Download(LPCTSTR lpszDownloadUrl, 
 * 						 LPCTSTR lpszSaveFile, 
 * 						 SYSTEMTIME *tFileTime = NULL);
 * 
 * 		����:	lpszDownloadUrl	Ҫ���ص�URL
 * 				lpszSaveFile	�����ڱ��ص��ļ�
 * 				tFileTime		������ʱ�䣬�Ա�Server���бȶ�
 * 
 * 		˵������ʹ�÷�DLL��ʽ�ṩ��CHttpDownload��ʹ���ߣ�ֻ��ʹ�ô˷������˷���
 * 			����Ҫ�Լ�ָ�����ر����ļ�����
 * 
 *       =========================================================================
 * 
 * 		2.ֹͣ����
 * 
 * 		void	CancelDownload()
 * 
 * 		����:	��
 * 
 * 		=========================================================================
 * 		3.������Ӧ��UI
 * 		����������������ǰ���ò���Ч. ����UI�������˵���뿴��CHttpDownloadUIInterface
 * 
 * 		����:	pUIInterface	UI������
 * 	
 * 		void	SetUIInterface(CHttpDownloadUIInterface *pUIInterface)
 * 
 * 		=========================================================================
 * 		5.����proxy
 * 		����������������ǰ���ò���Ч.
 * 	
 * 		������	bUseIEProxy		TRUE:	ʹ��IE���õĴ��� 
 * 								FALSE:	��ʹ���κδ���
 * 
 * 								Ĭ��ֵΪʹ��
 * 
 * 		void	SetUseIEProxy(BOOL bUseIEProxy)
 * 
 * 		=========================================================================
 * 		6.�Զ��ϵ�����
 * 		����:	bAutoResume		����ļ������Ѿ����ڣ� ���������� 
 * 								�Ƿ��Զ������ļ�β���ϵ�����
 * 
 * 		void	SetAutoResume(BOOL bAutoResume)
 * 
 * 		˵����Ĭ�ϴ�
 * 
 * 		=========================================================================
 * 		7.�ֹ��ϵ������ļ�
 * 		����:	dwResumeBytes	�ϵ�λ��
 * 				bForceDownloadWithResumeError	
 * 								����ϵ���ִ��� �Ƿ�ǿ��ȡ���ϵ㣬��������
 * 
 * 		UINT	DownloadResume(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, DWORD dwResumeBytes, BOOL bForceDownloadWithResumeError = TRUE);
 * 
 * 		˵����Ŀǰ�������⣬�޷���0�ϵ㡣����ǿ�ƴ�ͷ���أ�������SetAutoResume(FALSE)
 * 			  ��ɾ��Ŀ���ļ�������
 * 
 * 		=========================================================================
 * 		8.�����߳�
 * 		���������̱߳�־�������ļ������󣬲����Ϲر��̣߳����ȴ��¸�Download������
 * 		ʹ��Canceldownlad���̹߳ر�.�����´�ʹ��Download�����Զ��������߳�
 * 
 * 		����:	bResue			�Ƿ������߳�
 * 				
 * 		void	SetResueThread(BOOL bResue = TRUE);
 * 
 * 		=========================================================================
 * 		9. ��Http����ͷ������Զ�����Ϣ
 * 
 * 		void ClearRequestHeader();
 * 
 * 		˵�������HTTP����ͷ�������û�������Ϣ��
 * 
 * 		------------------------------------------------
 * 		CString GetRawUserRequestHeader();
 * 		˵������ȡHTTP����ͷ�������û�������Ϣ�����Ա�˶�
 * 	
 * 		------------------------------------------------
 * 		BOOL AddInfo(const CString &strContent);
 * 	
 * 		������strContent	�û�������Ϣ
 * 		˵�������ڷǱ�׼��ʽ���ݼ�������Ϣ����ӡ�ע��Ǳ�׼�����ݿ��ܻ���
 * 			����������ʱ���³�����׼�����ݾ���<field-name><:><value><\r\n>
 * 			���ĸ�ʽ��
 * 
 * 		------------------------------------------------
 * 		BOOL AddInfo(const CString &strParamName, const CString &strValue);
 * 		
 * 		������	strParamName	������������ð��
 * 				strValue		�ַ���
 * 		˵�����÷�����������ַ�����ʽ����Ϣ
 * 
 * 		-------------------------------------------------
 * 		BOOL AddInfo(const CString &strParamName, DWORD	dwValue);
 * 	
 * 		������	strParamName	������������ð��
 * 				dwValue			DWORD
 * 		˵�����÷����������DWORD��ʽ����Ϣ��DWORD���Զ�ת����ʮ��������ֵ��
 * 			����1234�ᱻת����"1234"
 * 
 * 		--------------------------------------------------
 * 		BOOL AddInfo(const CString &strParamName, const BYTE *byarBuf, int dwLength);
 * 
 * 		������	strParamName	������������ð��
 * 				byarBuf			��д�����Ϣ�Ļ�����
 * 				dwLength		�������Ĵ�С��byte
 * 		˵�����÷�������������ݿ���ʽ����Ϣ�����ݻ��Զ�ת����ʮ����������ֵ��
 * 			����{0x12, 0x3A}�ᱻת����"123A"
 * 
 * 		=======================================================================
 * 		10.��HTTP��Ӧͷ����ȡ�Զ�����Ϣ
 * 		
 * 		BOOL QueryInfo(const CString &strParamName, CString &strRet, BOOL bTrim = TRUE);
 * 
 * 		������	strParamName	������������ð��
 * 				strRet			�����HTTPͷ���õ����ַ���
 * 				bTrim			�Ƿ�ѵõ����ַ�����ͷβ�ո��Զ�ȥ���ŷ���
 * 
 * 		˵������HTTP�ظ�ͷ����ȡ�ַ�������ѡ���Ƿ�ȥ��ǰ��ո�Ĭ��Ϊȥ��ǰ��ո�
 * 
 * 		------------------------------------------------------
 * 		BOOL QueryInfo(const CString &strParamName,	DWORD &dwRet);
 * 
 * 		������	strParamName	������������ð��
 * 				dwRet			�����HTTPͷ���õ���DWORDֵ
 * 		˵������HTTP�ظ�ͷ����ȡ�����ַ�������ת����DWORD����
 * 
 * 		--------------------------------------------------------
 * 		BOOL QueryInfo(const CString &strParamName,	BOOL &bRet);
 * 
 * 		������	strParamName	������������ð��
 * 				bRet			�����HTTPͷ���õ���BOOLֵ
 * 		˵������HTTP�ظ�ͷ����ȡ��YES/NO��ʾ���ı�����ֵ����ת��ΪBOOLֵ����
 * 
 * 		--------------------------------------------------------
 * 		BOOL QueryInfo(const CString &strParamName, BYTE *byarBuf, DWORD dwLength);
 * 
 * 		������	strParamName	������������ð��
 * 				byarBuf			�����HTTPͷ���õ������ݿ�
 * 				dwLength		Ҫ������ݿ��С����λbyte
 * 		˵������HTTP�ظ�ͷ����ȡ16���������ı�������ԭ��Byte���鷵�أ���
 * 			��"123A"����ת��Ϊ{0x12, 0x3A}
 * 
 * 
 * 		==========================================================
 
 * ʹ��ע�����
 *
 * 1. ����ͨDLL�л�����ģ����ʹ����չDLL��BaseClassInDll.dll���е�HTTPDownload
 *    ���ʱ��HttpUIInterface�������¼��ص��ӿ���Ҫ����ģ��״̬�л�����Ҫ���¼�
 *    ������ڴ������л�״̬:
 *    ���롣
 *    a������ͨ��ģ�飩dll��ʹ��AFX_MANAGE_STATE(AfxGetStaticModuleState())�л�


 */


#if !defined(AFX_HTTPDOWNLOAD_H__FDF6AFAE_1CDC_4E13_B0CA_3399973E98ED__INCLUDED_)
#define AFX_HTTPDOWNLOAD_H__FDF6AFAE_1CDC_4E13_B0CA_3399973E98ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WININET.H>
#include <afxmt.h>
#include "ThreadModel.h"
#include "HttpDownloadUIInterface.h"

typedef DWORD  DWORD_PTR ; 
class CHttpDownloadUIInterface;

class CHttpDownload : public CThreadModel
{
// public 
public:
	enum DOWNLOAD_ERRORCODE
	{
		ERRORCODE_NULL			= 0,//// SUCCESS
		ERRORCODE_NOMODIFIED	= 1,//// No modified . so no update
		// error
		ERRORCODE_GENERAL		= 2,
		ERRORCODE_SERVERCLOSE	= 3,
		ERRORCODE_WRITEFAILED	= 4,
		ERRORCODE_NETWORKFAILED	= 5,
		ERRORCODE_FILESIZEERROR	= 6,
		ERRORCODE_FILENOFOUND	= 7,
		ERRORCODE_SERVERUNAVAIL	= 8,	
	};
	enum DOWNLOAD_EVENTMASK
	{
		EVENT_CONNECTING			= 1,
		EVENT_CONNECTED				= 2,
		EVENT_DOWNLOADSTART			= 4,
		EVENT_DOWNLOADCOMPLETE		= 8,
		EVENT_PROGRESS				= 16,
		EVENT_ERROR					= 32,

		EVENT_ALL = EVENT_CONNECTING | EVENT_CONNECTED |
					EVENT_DOWNLOADSTART | EVENT_DOWNLOADCOMPLETE |
					EVENT_PROGRESS | EVENT_ERROR
	};

	enum DOWNLOAD_REQUEST_METHOD
	{
		REQUEST_METHOD_GET = 0,
		REQUEST_METHOD_POST = 1
	};

	enum DOWNLOAD_PROXY_TYPE
	{
		PROXY_NONE = 0,
		PROXY_IE = 1,
		PROXY_CUSTOM = 2
	};

public:
	CHttpDownload();
	virtual ~CHttpDownload();
	void	SetUIInterface(CHttpDownloadUIInterface *pUIInterface);
	void	SetUseIEProxy(BOOL bUseIEProxy);
	void    SetAutoAddCookieToHeadFlag( BOOL bChoice );

	/// ������������
	BOOL	SetNetwork(	BYTE byAccessType,
						LPCTSTR szProxyIP = NULL, 
						WORD wProxyPort = 0, 
						LPCTSTR szProxyUserName = NULL,
						LPCTSTR szProxyPassword = NULL);

	/// �Ƿ��������Ի���ѯ���û�������������û���������
	void	EnablePopupAuthenticDialog(BOOL bEnable = TRUE);
	
	/// �Ƿ�ʹ��ȫ����������
	void	EnableGlobalNetworkSettings(BOOL bEnable = TRUE);
	void	SetAutoResume(BOOL bAutoResume);
	void	SetEventMask(BYTE chEventMask);
	void	SetRequestMethod(int nMethod);

	BOOL	GetResponseFileName(CString &strResponseFileName);
	BOOL	GetRawResponseHeader(CString &strResponseHeader);
	
	// Deprecated
	//UINT	PostDownload(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, LPCTSTR lpszPostHeader);
	
	// ����ļ����ڣ�����ļ��Ƿ���Ҫ���£� ��Ҫ���������ء��ļ�������ͬ������
	// ע�⣬ ���ﲻ��ϵ�
	UINT	Download(LPCTSTR lpszDownloadUrl, LPCTSTR lpszSaveFile, SYSTEMTIME *tFileTime = NULL);
	
	// ��ָ�������ļ����� �����ص���ʱ�ļ��С�
	
	BOOL	GetLastModifyTime(SYSTEMTIME &tTime);

	///MD �ϵ�����ʱ�����⣬������Ŀǰ����ǿ�ƴ������أ���ɾ�������ļ�������
	UINT	DownloadResume(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, DWORD dwResumeBytes, BOOL bForceDownloadWithResumeError = TRUE);

	void	CancelDownload();


	void	SetDownloadTempDir(LPCTSTR strDir);
	CString &GetDownloadTempDir();

	// �����̣߳�������Ϻ󣬲����Ϲر��̣߳����ȴ��¸�Download�ļ���)
	// ʹ��Canceldownlad()���̹߳ر�     yuyu 2004.4.7
	void	SetResueThread(BOOL bResue = TRUE);	

	//kongdong added
	DWORD GetStatusCode();	//��ȡHTTP���صķ�����
	void SetMaxRetryTimes(int nMaxTimes); //����������Դ���,Ĭ��3��
	
	//////////////////////////////////////////////////////////////////////////
	// ����ͷ����

	/// ���HTTP����ͷ�������û�������Ϣ��
	void ClearRequestHeader();

	/// ��ȡHTTP����ͷ�������û�������Ϣ��
	CString GetRawUserRequestHeader();
	
	/// ��HTTP����ͷ�������������
	BOOL AddInfo(const CString &strContent);

	/// ��HTTP����ͷ������ַ���
	BOOL AddInfo(const CString &strParamName, const CString &strValue);

	/// ��HTTP����ͷ�����DWORD���Զ�ת���������ַ���
	BOOL AddInfo(const CString &strParamName, DWORD	dwValue);

	/// ��HTTP����ͷ��������飬�Զ�ת����16���������ı�
	BOOL AddInfo(const CString &strParamName, const BYTE *byarBuf, int dwLength);

	/////////////////////////////////////////////////////////////////////////
	// �ظ�ͷ����
	
	/// ��HTTP�ظ�ͷ����ȡ�ַ�������ѡ���Ƿ�ȥ��ǰ��ո�Ĭ��Ϊȥ��ǰ��ո�
	BOOL QueryInfo(const CString &strParamName, CString &strRet, BOOL bTrim = TRUE);

	/// ��HTTP�ظ�ͷ����ȡ�����ַ�������ת����DWORD
	BOOL QueryInfo(const CString &strParamName,	DWORD &dwRet);

	/// ��HTTP�ظ�ͷ����ȡ��YES/NO��ʾ���ı�����ֵ����ת��ΪBOOL
	BOOL QueryInfo(const CString &strParamName,	BOOL &bRet);

	/// ��HTTP�ظ�ͷ����ȡ16���������ı�������ԭ��Byte����
	BOOL QueryInfo(const CString &strParamName, BYTE *byarBuf, DWORD dwLength);

//////////////////////////////////////////////////////////////////////////

protected:
	enum DOWNLOAD_RESULT
	{
		DOWNLOAD_RESULT_OK,
		DOWNLOAD_RESULT_TRY,
		DOWNLOAD_RESULT_CONTINUE,
		DOWNLOAD_RESULT_FAILED,
		DOWNLOAD_RESULT_EXIT,
		DOWNLOAD_RESULT_ABORT,
		DOWNLOAD_RESULT_USER_ABORT,
	};
	

	enum DOWNLOAD_STATUS
	{
		DOWNLOAD_STATUS_PREPAREING,
		DOWNLOAD_STATUS_DOWNLOADING,
		DOWNLOAD_STATUS_COMPLETE,
		DOWNLOAD_STATUS_FAILED,
		DOWNLOAD_STATUS_STOPED,
	};

	struct  tagDownloadFileInfo
	{

		CString		m_strDownloadURL;
		CString		m_strSavePath;
		DWORD		dwResumeBytes;
		int		nStatus;		// download status
		int		nErrorCode;		// 
		DWORD	dwDownloadedSize;
		DWORD	dwFileSize;
		BOOL		m_bRequestModified;
		SYSTEMTIME	m_tIfModifiedSince;
	
		DWORD		dwRequestResumeBytes;
		BOOL		bForceDownloadWithResumeError;

		tagDownloadFileInfo()
		{
			dwResumeBytes = nStatus = nErrorCode = dwDownloadedSize = dwFileSize = 0;
			dwRequestResumeBytes = 0;
			bForceDownloadWithResumeError = TRUE;

			m_bRequestModified = FALSE;
			memset(&m_tIfModifiedSince, 0, sizeof(SYSTEMTIME));
		}
	};

	
protected:
	// ԭpublic�����������ط���ʹ��
	BOOL	QueryInfoBase(const char* szParamName, CString& strInfo);
	
	// ת��16������������bufferΪByte����
	BOOL	EncodeBuffer(BYTE *byarBuf, CString &strBuf);

	// ת��Byte����Ϊ16������������buffer
	BOOL	DecodeBuffer(CString &strBuf, const BYTE *byarBuf, DWORD dwBufLen);

	void	OnMessageError(LPCSTR lpszMsg);

	UINT	DownloadBase(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile);

	UINT	InitDownload();
	UINT	DownloadRun();
	void	ExitDownload();

	BOOL	OpenInternetSession();
	void	CloseInternetSession();
	UINT	OpenInternetConnection();
	void	CloseInternetConnection();

	BOOL	GetUserAgent(CString& strUserAgent);
	BOOL	SetInternetOptions();
	DWORD	HttpQueryInfoStatusCode();
	void	HttpQueryResponseHeader();

	void	HttpPreSetFilePointer(DWORD dwOffSet);
	void	HttpSetExternalHeader(CString strHeader);

	void	HttpHandleModifiedTime();
	void	HttpSetRequestModifiedHeader(SYSTEMTIME &pTime);

	UINT	OpenInternetFile();
	void	CloseInternetFile();

	virtual UINT HttpSendRequest();
	
	UINT	DownloadInternetFile();

	void	QueryInternetFileInfo(CString& strInetFileName, 
												DWORD& dwInetFileSize, 
												SYSTEMTIME& stInetFileLastModifiedTime);



	void	SetDownloadSize(int nSize);
	void	SetFileSize(int nSize);

	void	SetStatusAndErrorCode(DOWNLOAD_STATUS nStatus, DOWNLOAD_ERRORCODE nErrorCode = ERRORCODE_NULL);

	BOOL	InitReceiveBuffer();
	void	ReleaseReceiveBuffer();

	BOOL	OpenSaveFile();
	void	CloseSaveFile();
	BOOL	FlushSaveFile();
	BOOL	WriteSaveFile(LPVOID pBuffer, DWORD dwBytes);

	BOOL	GetFileCurrentPos(HANDLE hSaveFileHandle, DWORD &dwCurrentPos);
	BOOL	MoveFilePosition(DWORD dwCurrentPos, DWORD dwFileSize);

	void	InitMemberData();

protected:
	UINT	Run();
	void	BeforeThread();
	void	AfterThread();


	virtual void OnStatusCallback(HINTERNET hInternet, DWORD dwInternetStatus,
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	

	static void AFXAPI InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext,
			DWORD dwInternetStatus, LPVOID lpvStatusInformation,
			DWORD dwStatusInformationLength);

#ifdef _DEBUG	
 static void AFXAPI CHttpDownload::InternetStatusCallbackDebug(HINTERNET hInternet,
			DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation,
			DWORD dwStatusInformationLength);
#endif

	// ui
protected:
	// heap helper
	BOOL	CreateHeapData(const void *Data, DWORD dwSize, LPVOID &pMem);


	BOOL	CreateUIProxy();
	void	DeleteUIProxy();	

	virtual void OnProgress(DWORD dwBytesRead);
	void	OnConnecting();
	void	OnConnected();	
	void	OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes);
	void	OnDownloadComplete(DWORD nErrorCode);
	void	OnError(DWORD nErrorCode);	
	
	BOOL	OnGetProxyUserPassword();

	// Events
protected:
	void	OnInetHandleCreated(HINTERNET hInternet, LPINTERNET_ASYNC_RESULT lpInetStatusResult);
	void	OnInetConnectionClosed(HINTERNET hInternet);
	void	OnInetClosingConnection(HINTERNET hInternet);
	void	OnInetRequestComplete(HINTERNET hInternet, 
			LPINTERNET_ASYNC_RESULT lpInetStatusResult);

	void	OnInetResponceReceived(HINTERNET hInternet, 
			LPVOID lpvStatusInformation);


	UINT	OnHttpStatusOK(DWORD dwCurrentPos);
	UINT	OnHttpStatusRedirect(DWORD dwCurrentPos);
	UINT	OnHttpStatusPartialContent(DWORD dwCurrentPos);
	UINT	OnHttpStatusOther(DWORD dwStatusCode);


	virtual void OnSupportResume(BOOL bSupport);
	void OnRedirect(LPCTSTR plszRedirectUrl);

	BOOL CreateAllDirectory(const CString& strPath);
	BOOL CheckDirectoryExist(const CString& strFileName);

	/// ��Internet�������ô�����Ϣ
	UINT ApplyNetworkSettings();
	
protected:
	BOOL		m_bUsePostVerb;
	CString		m_strPostHeader;
	CString		m_strResponseHeader;
	
	int			m_nMaxRetryTime;
	int			m_nRetryTimes;

	BOOL		m_bForceDownload;
	BOOL		m_bSupportResume;

	BOOL		m_bAutoResume;		// �Զ�ȡ�ļ���С�� �ϵ�����

	BYTE		m_chEventMask;

	CString		m_strRef;
	CString		m_strObject;

	CString		m_strServer;

	DWORD		m_dwReceiveBufferLen;
	BYTE	*	m_ReceiveBuffer;

	BOOL		m_bUseWriteBuffer;
	DWORD		m_dwWriteBufferLen;

	DWORD		m_dwWriteBufferOffset;
	LPVOID		m_pWriteBuffer;
	

	HINTERNET	m_hInetSession;
	HINTERNET	m_hHttpConnect;
	HINTERNET	m_hHttpFile;
	HANDLE		m_hSaveFileHandle;

	INTERNET_BUFFERS m_buffersOut;

	tagDownloadFileInfo	m_DownloadFileInfo;


	HANDLE		m_hUIHeap;
	HWND		m_hwndUIProxy;
	CHttpDownloadUIInterface	*m_pUIInterface;

	BOOL			m_bIfModified;
	SYSTEMTIME		m_tLastModified;

	CString			m_strDownloadTempDir;

	CString		m_strResponseFileName;			///< ���������ص��ļ���

	BOOL		m_bReuseThread;					///<  �����̱߳�־

	BOOL		m_bPopAuthenticDlg;				///< �Ƿ񵯳�ѯ���û�������ĶԻ���

	// ʵ����������
	DWORD		m_dwAccessType;			///< ��������
	CString		m_strProxyIP;			///< ����IP
	WORD		m_wProxyPort;			///< ����˿�
	BOOL		m_bProxyAuthentic;		///< �����Ƿ���Ҫ�����֤
	CString		m_strProxyUserName;		///< �����û���
	CString		m_strProxyPassword;		///< ��������

	BOOL		m_bUseGlobalNetworkSettings; ///< �Ƿ�ʹ��ȫ����������

	DWORD m_dwStatusCode;		//kongdong added
	
	// ȫ����������
	static DWORD	s_dwAccessType;		///< ��������
	static CString	s_strProxyIP;		///< ����IP
	static WORD		s_wProxyPort;		///< ����˿�
	static BOOL		s_bProxyAuthentic;	///< �����Ƿ���Ҫ�����֤
	static CString	s_strProxyUserName;	///< �����û���
	static CString	s_strProxyPassword;	///< ��������

protected:

	DWORD			m_dwError;
	HANDLE			m_hEvtRequestComplete;			///< ��������¼����첽���ƣ�
	HANDLE			m_hEvtHandleCreated;			///< HANDLE�Ѵ����¼����첽���ƣ�
	HANDLE			m_hEvtUIExit;					///< UI�Ƿ�ͬ���˳�

	HANDLE			m_hEvtWakeUpThread;				///< �����̼߳���
	BOOL            m_bAutoAddCookie;               ///�Ƿ��Զ���cookie
	
// CallBack
private:
	class CThisMapPtrToPtr : public CMapPtrToPtr
	{
	private:
		CCriticalSection m_sect;
	public:

		CThisMapPtrToPtr() { }
		~CThisMapPtrToPtr() { UnlockMap();}		

		void SetAt(CHttpDownload* pThis)		
		{
			CSingleLock cs(&m_sect);
			cs.Lock();
			if (cs.IsLocked())
			{						
				CMapPtrToPtr::SetAt(pThis, NULL);
				cs.Unlock();
			}
		}		

		void RemoveKey(CHttpDownload* pThis)
		{
			CSingleLock cs(&m_sect);
			cs.Lock();
			if (cs.IsLocked())
			{					
				CMapPtrToPtr::RemoveKey(pThis);
				cs.Unlock();
			}
			
		}		

		BOOL Lookup(CHttpDownload* pThis)
		{
			BOOL bRet = FALSE;
			void* pNULL;

			CSingleLock cs(&m_sect);
			cs.Lock();
			if (cs.IsLocked())
			{	
				bRet = CMapPtrToPtr::Lookup(pThis, pNULL);
				cs.Unlock();
			}

			return bRet;
		}

		BOOL LockMap()
		{
			return m_sect.Lock();
		}

		BOOL UnlockMap()
		{
			return m_sect.Unlock();
		}
	};
	
	static CThisMapPtrToPtr m_thisMap;
};

#endif // !defined(AFX_HTTPDOWNLOAD_H__FDF6AFAE_1CDC_4E13_B0CA_3399973E98ED__INCLUDED_)

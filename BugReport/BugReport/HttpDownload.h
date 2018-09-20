// -------------------------------------------------------------------------
//	文件名		：	HttpDownload.h
//	创建者		：	yuyu
//	创建时间	：	2004-2-13 10:00:00
//	功能描述	：	下载Http Server的文件
//			
//	
//			支持断点续传
//			支持多次失败重试
//			支持重定向下载
//			支持双缓存, 延迟写盘
//			支持多个文件同时下载
//
//-------------------------------------------------------------------------


/**
 * 
 *   CHttpDownload使用说明:
 * 
 * 		1.下载文件
 * 		UINT	Download(LPCTSTR lpszDownloadUrl, SYSTEMTIME *tFileTime = NULL)
 * 		
 * 		参数:	lpszDownloadUrl	要下载的URL
 * 				tFileTime		最后更新时间，以便Server进行比对
 * 
 * 		说明：对使用DLL形式提供的CHttpDownload的使用者，推荐使用此方法，它会先把
 * 			　文件下载到一个临时文件夹。在OnDownloadComplete时，可以获得其保存的
 * 			　临时文件名，再复制到目标位置。
 * 
 * 		-----------------------------------------------------------------------
 * 		UINT	Download(LPCTSTR lpszDownloadUrl, 
 * 						 LPCTSTR lpszSaveFile, 
 * 						 SYSTEMTIME *tFileTime = NULL);
 * 
 * 		参数:	lpszDownloadUrl	要下载的URL
 * 				lpszSaveFile	保存在本地的文件
 * 				tFileTime		最后更新时间，以便Server进行比对
 * 
 * 		说明：对使用非DLL形式提供的CHttpDownload的使用者，只能使用此方法。此方法
 * 			　需要自己指定本地保存文件名。
 * 
 *       =========================================================================
 * 
 * 		2.停止下载
 * 
 * 		void	CancelDownload()
 * 
 * 		参数:	无
 * 
 * 		=========================================================================
 * 		3.设置响应的UI
 * 		本函数必须在下载前调用才有效. 关于UI处理类的说明请看类CHttpDownloadUIInterface
 * 
 * 		参数:	pUIInterface	UI处理类
 * 	
 * 		void	SetUIInterface(CHttpDownloadUIInterface *pUIInterface)
 * 
 * 		=========================================================================
 * 		5.设置proxy
 * 		本函数必须在下载前调用才有效.
 * 	
 * 		参数：	bUseIEProxy		TRUE:	使用IE设置的代理。 
 * 								FALSE:	不使用任何代理
 * 
 * 								默认值为使用
 * 
 * 		void	SetUseIEProxy(BOOL bUseIEProxy)
 * 
 * 		=========================================================================
 * 		6.自动断点下载
 * 		参数:	bAutoResume		如果文件本地已经存在， 但不完整， 
 * 								是否自动接着文件尾部断点续传
 * 
 * 		void	SetAutoResume(BOOL bAutoResume)
 * 
 * 		说明：默认打开
 * 
 * 		=========================================================================
 * 		7.手工断点下载文件
 * 		参数:	dwResumeBytes	断点位置
 * 				bForceDownloadWithResumeError	
 * 								如果断点出现错误， 是否强行取消断点，重新下载
 * 
 * 		UINT	DownloadResume(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, DWORD dwResumeBytes, BOOL bForceDownloadWithResumeError = TRUE);
 * 
 * 		说明：目前存在问题，无法置0断点。如需强制从头下载，可设置SetAutoResume(FALSE)
 * 			  或删除目标文件再下载
 * 
 * 		=========================================================================
 * 		8.重用线程
 * 		设置重用线程标志后，下载文件结束后，不马上关闭线程，　等待下个Download来激活
 * 		使用Canceldownlad后，线程关闭.　但下次使用Download，　自动启动新线程
 * 
 * 		参数:	bResue			是否重用线程
 * 				
 * 		void	SetResueThread(BOOL bResue = TRUE);
 * 
 * 		=========================================================================
 * 		9. 向Http请求头部添加自定义信息
 * 
 * 		void ClearRequestHeader();
 * 
 * 		说明：清除HTTP请求头部（的用户附加信息）
 * 
 * 		------------------------------------------------
 * 		CString GetRawUserRequestHeader();
 * 		说明：获取HTTP请求头部（的用户附加信息），以便核对
 * 	
 * 		------------------------------------------------
 * 		BOOL AddInfo(const CString &strContent);
 * 	
 * 		参数：strContent	用户附加信息
 * 		说明：用于非标准形式数据及整块信息的添加。注意非标准的数据可能会在
 * 			　启动下载时导致出错。标准的数据具有<field-name><:><value><\r\n>
 * 			　的格式。
 * 
 * 		------------------------------------------------
 * 		BOOL AddInfo(const CString &strParamName, const CString &strValue);
 * 		
 * 		参数：	strParamName	参数名，不带冒号
 * 				strValue		字符串
 * 		说明：该方法用于添加字符串形式的信息
 * 
 * 		-------------------------------------------------
 * 		BOOL AddInfo(const CString &strParamName, DWORD	dwValue);
 * 	
 * 		参数：	strParamName	参数名，不带冒号
 * 				dwValue			DWORD
 * 		说明：该方法用于添加DWORD形式的信息，DWORD会自动转成其十进制字面值，
 * 			　如1234会被转换成"1234"
 * 
 * 		--------------------------------------------------
 * 		BOOL AddInfo(const CString &strParamName, const BYTE *byarBuf, int dwLength);
 * 
 * 		参数：	strParamName	参数名，不带冒号
 * 				byarBuf			需写入的信息的缓冲区
 * 				dwLength		缓冲区的大小，byte
 * 		说明：该方法用于添加数据块形式的信息，数据会自动转成其十六进制字面值，
 * 			　如{0x12, 0x3A}会被转换成"123A"
 * 
 * 		=======================================================================
 * 		10.从HTTP回应头部获取自定义信息
 * 		
 * 		BOOL QueryInfo(const CString &strParamName, CString &strRet, BOOL bTrim = TRUE);
 * 
 * 		参数：	strParamName	参数名，不带冒号
 * 				strRet			保存从HTTP头部得到的字符串
 * 				bTrim			是否把得到的字符串的头尾空格自动去除才返回
 * 
 * 		说明：从HTTP回复头部获取字符串，可选择是否去除前后空格，默认为去除前后空格
 * 
 * 		------------------------------------------------------
 * 		BOOL QueryInfo(const CString &strParamName,	DWORD &dwRet);
 * 
 * 		参数：	strParamName	参数名，不带冒号
 * 				dwRet			保存从HTTP头部得到的DWORD值
 * 		说明：从HTTP回复头部获取数字字符串，并转换成DWORD返回
 * 
 * 		--------------------------------------------------------
 * 		BOOL QueryInfo(const CString &strParamName,	BOOL &bRet);
 * 
 * 		参数：	strParamName	参数名，不带冒号
 * 				bRet			保存从HTTP头部得到的BOOL值
 * 		说明：从HTTP回复头部获取以YES/NO表示的文本布尔值，并转换为BOOL值返回
 * 
 * 		--------------------------------------------------------
 * 		BOOL QueryInfo(const CString &strParamName, BYTE *byarBuf, DWORD dwLength);
 * 
 * 		参数：	strParamName	参数名，不带冒号
 * 				byarBuf			保存从HTTP头部得到的数据块
 * 				dwLength		要求的数据块大小，单位byte
 * 		说明：从HTTP回复头部获取16进制数字文本，并还原成Byte数组返回，如
 * 			　"123A"将被转换为{0x12, 0x3A}
 * 
 * 
 * 		==========================================================
 
 * 使用注意事项：
 *
 * 1. 在普通DLL中或在主模块中使用扩展DLL（BaseClassInDll.dll）中的HTTPDownload
 *    组件时，HttpUIInterface的所有事件回调接口需要进行模块状态切换，需要在事件
 *    函数入口处加入切换状态:
 *    代码。
 *    a、在普通（模块）dll中使用AFX_MANAGE_STATE(AfxGetStaticModuleState())切换


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

	/// 设置连接类型
	BOOL	SetNetwork(	BYTE byAccessType,
						LPCTSTR szProxyIP = NULL, 
						WORD wProxyPort = 0, 
						LPCTSTR szProxyUserName = NULL,
						LPCTSTR szProxyPassword = NULL);

	/// 是否允许弹出对话框询问用户代理服务器的用户名与密码
	void	EnablePopupAuthenticDialog(BOOL bEnable = TRUE);
	
	/// 是否使用全局网络设置
	void	EnableGlobalNetworkSettings(BOOL bEnable = TRUE);
	void	SetAutoResume(BOOL bAutoResume);
	void	SetEventMask(BYTE chEventMask);
	void	SetRequestMethod(int nMethod);

	BOOL	GetResponseFileName(CString &strResponseFileName);
	BOOL	GetRawResponseHeader(CString &strResponseHeader);
	
	// Deprecated
	//UINT	PostDownload(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, LPCTSTR lpszPostHeader);
	
	// 如果文件存在，检查文件是否需要更新， 需要则重新下载。文件不存在同样下载
	// 注意， 这里不会断点
	UINT	Download(LPCTSTR lpszDownloadUrl, LPCTSTR lpszSaveFile, SYSTEMTIME *tFileTime = NULL);
	
	// 不指定保存文件名， 则下载到临时文件夹。
	
	BOOL	GetLastModifyTime(SYSTEMTIME &tTime);

	///MD 断点置零时有问题，待处理，目前如想强制从零下载，可删除本地文件再下载
	UINT	DownloadResume(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, DWORD dwResumeBytes, BOOL bForceDownloadWithResumeError = TRUE);

	void	CancelDownload();


	void	SetDownloadTempDir(LPCTSTR strDir);
	CString &GetDownloadTempDir();

	// 重用线程（下载完毕后，不马上关闭线程，　等待下个Download的激活)
	// 使用Canceldownlad()后，线程关闭     yuyu 2004.4.7
	void	SetResueThread(BOOL bResue = TRUE);	

	//kongdong added
	DWORD GetStatusCode();	//获取HTTP下载的返回码
	void SetMaxRetryTimes(int nMaxTimes); //设置最大重试次数,默认3次
	
	//////////////////////////////////////////////////////////////////////////
	// 请求头处理

	/// 清除HTTP请求头部（的用户附加信息）
	void ClearRequestHeader();

	/// 获取HTTP请求头部（的用户附加信息）
	CString GetRawUserRequestHeader();
	
	/// 向HTTP请求头部添加请求内容
	BOOL AddInfo(const CString &strContent);

	/// 向HTTP请求头部添加字符串
	BOOL AddInfo(const CString &strParamName, const CString &strValue);

	/// 向HTTP请求头部添加DWORD，自动转换成数字字符串
	BOOL AddInfo(const CString &strParamName, DWORD	dwValue);

	/// 向HTTP请求头部添加数组，自动转换成16进制数字文本
	BOOL AddInfo(const CString &strParamName, const BYTE *byarBuf, int dwLength);

	/////////////////////////////////////////////////////////////////////////
	// 回复头处理
	
	/// 从HTTP回复头部获取字符串，可选择是否去除前后空格，默认为去除前后空格
	BOOL QueryInfo(const CString &strParamName, CString &strRet, BOOL bTrim = TRUE);

	/// 从HTTP回复头部获取数字字符串，并转换成DWORD
	BOOL QueryInfo(const CString &strParamName,	DWORD &dwRet);

	/// 从HTTP回复头部获取以YES/NO表示的文本布尔值，并转换为BOOL
	BOOL QueryInfo(const CString &strParamName,	BOOL &bRet);

	/// 从HTTP回复头部获取16进制数字文本，并还原成Byte数组
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
	// 原public方法，由重载方法使用
	BOOL	QueryInfoBase(const char* szParamName, CString& strInfo);
	
	// 转换16进制数字文字buffer为Byte数组
	BOOL	EncodeBuffer(BYTE *byarBuf, CString &strBuf);

	// 转换Byte数组为16进制数字文字buffer
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

	/// 向Internet连接设置代理信息
	UINT ApplyNetworkSettings();
	
protected:
	BOOL		m_bUsePostVerb;
	CString		m_strPostHeader;
	CString		m_strResponseHeader;
	
	int			m_nMaxRetryTime;
	int			m_nRetryTimes;

	BOOL		m_bForceDownload;
	BOOL		m_bSupportResume;

	BOOL		m_bAutoResume;		// 自动取文件大小， 断点续传

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

	CString		m_strResponseFileName;			///< 服务器返回的文件名

	BOOL		m_bReuseThread;					///<  重用线程标志

	BOOL		m_bPopAuthenticDlg;				///< 是否弹出询问用户名密码的对话框

	// 实例网络设置
	DWORD		m_dwAccessType;			///< 连接类型
	CString		m_strProxyIP;			///< 代理IP
	WORD		m_wProxyPort;			///< 代理端口
	BOOL		m_bProxyAuthentic;		///< 代理是否需要身份验证
	CString		m_strProxyUserName;		///< 代理用户名
	CString		m_strProxyPassword;		///< 代理密码

	BOOL		m_bUseGlobalNetworkSettings; ///< 是否使用全局网络设置

	DWORD m_dwStatusCode;		//kongdong added
	
	// 全局网络设置
	static DWORD	s_dwAccessType;		///< 连接类型
	static CString	s_strProxyIP;		///< 代理IP
	static WORD		s_wProxyPort;		///< 代理端口
	static BOOL		s_bProxyAuthentic;	///< 代理是否需要身份验证
	static CString	s_strProxyUserName;	///< 代理用户名
	static CString	s_strProxyPassword;	///< 代理密码

protected:

	DWORD			m_dwError;
	HANDLE			m_hEvtRequestComplete;			///< 请求完成事件（异步控制）
	HANDLE			m_hEvtHandleCreated;			///< HANDLE已创建事件（异步控制）
	HANDLE			m_hEvtUIExit;					///< UI是否同意退出

	HANDLE			m_hEvtWakeUpThread;				///< 重用线程激活
	BOOL            m_bAutoAddCookie;               ///是否自动添cookie
	
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

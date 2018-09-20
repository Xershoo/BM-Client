// HttpUploadFile.h: interface for the CHttpUploadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPUPLOADFILE_H__23C0420E_D345_4533_A496_1DA039899859__INCLUDED_)
#define AFX_HTTPUPLOADFILE_H__23C0420E_D345_4533_A496_1DA039899859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** CHttpUploadFile 通过HTTP协议每次上传一个文件的类，基于CHttpDownload实现
 *  
 *  Mal Fox 2004.11
 *
 *  使用说明
 *  =========================
 *  1、令一个类继承CHttpDownloadUIInterface，在其中可选择性继承On方法，
 *    
 *  2、在某处声明CHttpUploadFile的对象，并调用SetUIInterface指向之前准备好的
 *     CHttpDownloadUIInterface接口实现类对象
 *
 *  3、使用ResetFormData清除上一次上传留下的Form数据
 *
 *  4、使用AddFormData添加上传时附带的Form数据
 *
 *  5、使用UploadFile开始上传文件，如果其返回FALSE，表示上传失败
 *
 *  6、如果UploadFile没有返回FALSE，最后CHttpDownloadUIInterface接口实现类
 *     对象的OnDownloadComplete应会被调用，在其中，对nErrorCode进行判断，可
 *     得知上传动作是否成功。而具体Server是否收了货，目前需要对收到的数据包
 *     头部数据进行分析才可得知。
 *
 *  6.1、对Server返回的信息，可以用一组QueryInfo方法抽取信息。支持抽取以
 *       “字段名” + “:” + “值”形式组织的信息。支持String（原始数据），
 *       DWORD，BOOL以及Buffer的抽取，具体参考方法说明。
 *
 *  7、如果希望中断上传动作，使用CancelUpload方法。注意调用此方法后不会走到
 *     CHttpDownloadUIInterface接口实现类对象的OnDownloadComplete方法。
 *
 *  8、如果希望不依赖于FlashGet使用该类，在工程设置中添加
 *     宏 _STANDALONE_USE_ 即可。
 *
 */

#include "HttpDownload.h"

namespace CHTTPUPLOADFILE
{
	/// 上传时使用的Buffer大小(bytes)
	const DWORD c_dwBufferSize = 10240; 

	// !!!切勿修改此部分的格式常量!!!
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
	/// 设置上传文件大小限制（单位：byte）
	void	SetFileSizeLimit(DWORD dwSize);
	/// 清空Form内容
	void	ResetFormData();
	/// 向Form添加一个域及其值
	void	AddFormData(const CString &strFieldName, 
						const CString &strValue);
	/// 上传文件
	BOOL	UploadFile(	const CString &strField, 
						const CString &strURL, 
						const CString &strFile);
	/// 中断上传动作
	void	CancelUpload();

	/// 从HTTP回复头部获取各种格式数据文本，并转换成相应格式，请参考CHttpDownload
	CHttpDownload::QueryInfo;

	/// 获取返回信息头部 BOOL (CString &strResponseHeader);
	CHttpDownload::GetRawResponseHeader;

	/// 设置事件回调接口 BOOL (CHttpDownloadUIInterface *pUIInterface);
	CHttpDownload::SetUIInterface;

	CHttpDownload::SetUseIEProxy;

	/// 设置连接类型
	CHttpDownload::SetNetwork;

	//////////////////////////////////////////////////////////////////////////
	// 内部实现
protected:
	/// 探测浏览器代理是否需要用户名密码
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

	BYTE	m_byarBuf[CHTTPUPLOADFILE::c_dwBufferSize]; ///< 上传缓冲区
	CString m_strForm;									///< Form数据
	CString m_strBoundary;								///< 分隔符
	CString m_strFileToUpload;							///< 上传文件名
	HANDLE	m_hFile;									///< 上传文件Handle
	DWORD	m_dwFileSizeLimit;							///< 文件大小上限(byte)
	
	DWORD	m_dwTotalWriteSize;							///< 预期写入大小
	DWORD	m_dwCurrentWriteSize;						///< 目前写入大小

	BOOL	m_bUploading;								///< 上传标记

};

#endif // !defined(AFX_HTTPUPLOADFILE_H__23C0420E_D345_4533_A496_1DA039899859__INCLUDED_)

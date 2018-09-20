// -------------------------------------------------------------------------
//	文件名		：	HttpDownloadUIInterface.h
//	创建者		：	yuyu
//	创建时间	：	2004-2-13 10:00:00
//	功能描述	：	下载文件的UI表现类
//			
//			提供简单的接口给使用者实现界面表现
//	
//  Updated: 2004-11-29 Mal 新增重定向回调接口，修改使用说明
//-------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//
//	CHttpDownloadUIInterface使用说明
//		
//		接口类, 需要继承实现需要的接口
//
//		1.开始连接服务器
//			virtual void OnConnecting(LPCTSTR lpszServerAddr)
//
//			参数:	lpszServerAddr	服务器地址
//
//		2.连接服务器成功
//			virtual void OnConnected()
//
//		3.连接重定向
//			virtual void OnRedirected(LPCTSTR lpszRedirectedAddr)
//
//			参数：lpszRedirectedAddr	重定向的目的地址
//
//		4.开始下载文件
//			virtual void OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes)
//
//			参数:	dwResumeBytes	开始下载的文件位置. 断点续传时候使用
//					dwFileBytes		文件的大小
//
//		5.下载进度
//			virtual void OnProgress(DWORD dwProgress, DWORD dwProgressMax)
//
//			参数:	dwProgress		已经下载的大小. 
//					dwProgressMax	文件的总大小
//		
//		6.下载完成
//			virtual void OnDownloadComplete(DWORD nErrorCode, LPCTSTR lpszSaveFile)
//
//			参数：nErrorCode	状态代码，取值ERRORCODE_NULL或ERRORCODE_NOMODIFIED
//								表示HTTP下载成功（含服务器返回无改动），其他可能
//								取值同OnError的nCode取值，表示与服务器交互出错。
//			 　   lpszSaveFile	保存到的本地文件名
//
//		7.下载出现出错
//			virtual void OnError(int nCode)
//
//			参数:	nCode			为出错代码
//
//	 				DOWNLOAD_ERRORCODE_GENERAL:			下载失败
//					DOWNLOAD_ERRORCODE_SERVERCLOSE:		服务器关闭连接
//					DOWNLOAD_ERRORCODE_WRITEFAILED:		写入文件出错, 文件被打开或者磁盘已满
//					DOWNLOAD_ERRORCODE_NETWORKFAILED:	无法连接服务器
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
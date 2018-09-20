#if !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
#define AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// OleRichEditCtrl.h : header file
//
#include <richole.h>

#define __ATLTYPES_H__
#include <atltypes.h>

#include <map>

using namespace std;

#include "GTMenuHandle.h"
#include "Message.h"
#include "im_richedit/im_richedit_sdk.h"


/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl window

#define TIME_EVENT_REFRESH			8000

#define ID_RICH_CUT					(WM_USER+1)
#define ID_RICH_COPY				(WM_USER+2)
#define ID_RICH_PASTE				(WM_USER+3)
#define ID_RICH_CLEAR				(WM_USER+4)
#define ID_RICH_SELECTALL			(WM_USER+5)
#define ID_RICH_DEL					(WM_USER+6)
#define GTN_RICH_RBUTTON_DOWN		1

#define ES_READONLY_SIMPLE			0x0200L

class CGTRichEdit : public CWindowImpl<CGTRichEdit, CRichEditCtrl>
	, public CGTMenuHandle<CGTRichEdit>
	/*, public CMessageFilter*/
	, public im_richedit::IMRichEditHost
{
// Construction
public:
	CGTRichEdit();
	virtual ~CGTRichEdit();

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);

	WTL::CString GetSelTextW();

	virtual void OnEraseBackground(HDC dc, const RECT& rect);

	void InsertFace( WTL::CString strPicPath , UINT uData , int nGameId, int nEmotionId, int nPicId);
	void InsertFace(WTL::CString strPicPath);
	void InsertFaceMd5(WTL::CString strPicPath);

	long StreamInFromResource(int iRes, LPCTSTR sType);
	long StreamInFromMem(LPCSTR pszContent, int nType);
	long StreamOutToMem(LPSTR pszBuff, long lLen);

    void SetProtected();
    void SetLink();
	void InsertLink(LPCTSTR pText);
	void SetNoLink();
	void SetForbidWheel(bool bForbid);
	void SetColor(COLORREF crText);
	
	void SetAutoUrlDetect(BOOL bAutoUrl); //设置URL自动检测, false 是不检测

	void NotPopNext() { m_bPop = FALSE;}
	void SetPopMenu(bool bPop);

	inline void SetNotifyMenu(BOOL bNotify = TRUE)	{ m_bNotifyMenu = bNotify;}
    inline void AcceptInput(BOOL bAccept)			{ m_bInput = bAccept; }
    inline BOOL AcceptInput()						{ return m_bInput; }
	void UpdateGif(ATL::CString& strPic);
	void UpdateGif(ATL::CString& strPic, LPCTSTR strErrorPic);
	void SendLBDownToParent(bool bSendToParent);
	void SetAllowInsertPicFromClipboard(bool bAllow = false);
	void SetCutScreenPicSavePath(LPCTSTR pPath);
	ATL::CString GetMd5ByPicData(int nIndex);
	void ClearMd5PicList();
	im_richedit::IMRichEdit* GetOle() { return m_pOle;}
	void DeleteToTopNumMsg(int nCount);
	void AppendTextDetectUrl(ATL::CString& strText);
	bool AppendBitmap(HBITMAP hBitmap);
	bool AppendLocalPicture(ATL::CString& pPicPath);
	int LimitParagraphCount(int nMax);
private:
	BOOL m_bReFresh;
    BOOL m_bInput;
	BOOL m_bNotifyMenu;
	BOOL m_bPop;
	bool m_bPopMenu;
	bool m_bforbidWheel;
	im_richedit::IMRichEdit*	m_pOle;
	std::map<int, ATL::CString>	m_mapDownGifList;
	bool m_bInsertPicFormClipboard; //是否允许从剪切板中复制图片,默认不行
	TCHAR	m_szCutScreenPicSavePath[MAX_PATH];
	std::map<int, ATL::CString> m_mapMd5IndexList; //截图，图片的MD5对应的ID；
	int	m_nMaxParagraphCount;
protected:
	static DWORD CALLBACK readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead);			// number of bytes actually read

	static DWORD CALLBACK MyStreamOutCallback(DWORD dwCookie, 
		 LPBYTE pbBuff,	
		 LONG cb, 
		 LONG *pcb);

	interface IExRichEditOleCallback;	// forward declaration (see below in this header file)

	BOOL m_bCallbackSet;
	bool m_bLBDownSendToParent;


protected:
	void InsertPicture(WTL::CString& strPicPath,long nUserData,bool bZoom = false);
	void UpdatePicture(long obj_index,LPCWSTR lpwszPicPath);
	bool ParagraphCountIsMax();
    void CreateMenu(DWORD userData);
// Implementation
public:
	// Generated message map functions
	BEGIN_MSG_MAP(CGTRichEdit)
		MESSAGE_HANDLER(WM_CREATE,				OnCreate)
		MESSAGE_HANDLER(WM_RBUTTONDOWN,			OnRButtonDown)
		MESSAGE_HANDLER(WM_VSCROLL,				OnVScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL,			OnMouseWheel)
		MESSAGE_HANDLER(WM_TIMER,				OnTimer)
		MESSAGE_HANDLER(WM_DESTROY,				OnDestroy)
		MESSAGE_HANDLER(WM_CHAR,				OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN,				OnKeyDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,			OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,		OnLButtonDblClk)
		MESSAGE_HANDLER(WM_MOUSEMOVE,			OnMouseMove)
		MESSAGE_HANDLER(WM_DOWNLOAD_CHAT_IMAGE, OnDownloadChatImage)
		MESSAGE_HANDLER(WM_DROPFILES,			OnDropFiles)
		COMMAND_ID_HANDLER(ID_RICH_COPY,		OnCopy)
		COMMAND_ID_HANDLER(ID_RICH_CUT,			OnCut)
		COMMAND_ID_HANDLER(ID_RICH_PASTE,		OnPaste)
		COMMAND_ID_HANDLER(ID_RICH_SELECTALL,	OnSelectall)
		COMMAND_ID_HANDLER(ID_RICH_DEL,			OnDel)
		COMMAND_ID_HANDLER(ID_RICH_CLEAR,		OnClear)

		CHAIN_MSG_MAP(CGTMenuHandle<CGTRichEdit>)
	END_MSG_MAP()
public:
	//virtual BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnCreate     (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnVScroll    (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer      (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy     (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar     (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown	(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDownloadChatImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);    

	// 取得编辑框内文本的高度 [5/7/2010 何春龙]
	UINT GetTextHeight();

	LRESULT OnCopy(WORD, WORD, HWND, BOOL&) 
	{
		Copy(); 
		return 1;
	}

	LRESULT OnCut(WORD, WORD, HWND, BOOL&) 
	{ 
		Cut();
		return 1;
	}

	LRESULT OnPaste(WORD, WORD, HWND, BOOL&);

	LRESULT OnSelectall(WORD, WORD, HWND, BOOL&)
	{ 
		SetSel(0, -1); 
		return 1;
	}

	LRESULT OnDel(WORD, WORD, HWND, BOOL&)
	{ 
		Clear();
		return 1;
	}

	LRESULT OnClear(WORD, WORD, HWND, BOOL&)
	{
		CHARFORMAT cfdefualt;
		GetDefaultCharFormat(cfdefualt);
		SetSel(0,-1); 
		ReplaceSel(_T(""));
		SetDefaultCharFormat(cfdefualt);
		return 1;
	}
	int GetWindowText(CSimpleString& strText) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)

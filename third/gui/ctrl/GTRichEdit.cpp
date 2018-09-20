// OleRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GTRichEdit.h"
#include "Env.h"
#include "resource.h"

#include "richedit.h"
#include "richole.h"
#include "tom.h"
#include "DeComp.h"
#include "languageX/languageX.h"
#include "StringHelper.h"
#include "CLFile.h"
#include "skinloader.h"
#include "WndManager.h"
#include "HttpSessionMgr.h"
#include "../Session/EduSeeion.h"
#include "../classroom/ClassRoomDlg2.h"
#include "../../../biz/biz/interface/interface.h"
#include "../../../biz/biz/interface/ICourseInfoContainer.h"
#include "../../../biz/biz/interface/IUserInfoData.h"
#include "../../../biz/biz/interface/IClassRoomInfoContainer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
#define PICTURE_SHOW_BIG_CX   (120)
#define PICTURE_SHOW_BIG_CY   (90)

#define PICTURE_SHOW_SMALL_CX   (120)
#define PICTURE_SHOW_SMALL_CY   (90)


#define GROW_LENGTH		256		// 以256字节为单位分配内存

class CSimpleMemFile
{
private:
	PVOID	m_pData;			// 内部缓存
	DWORD	m_dwSize;			// 缓存大小
	DWORD	m_dwOffset;			// 当前位置
	BOOL	m_bOwner;			// 是否自己分配的

private:
	// 释放分配的内存
	void Release()
	{
		if (m_bOwner && m_pData)
		{
			free(m_pData);
		}
		m_pData		= NULL;
		m_dwSize	= 0;
		m_dwOffset	= 0;
		m_bOwner	= TRUE;
	}

	// 自动增长内存
	void Growth(DWORD dwLength)
	{
		DWORD dwNeedLength = m_dwOffset+dwLength;

		// 不是自己分配的或者长度足够都不用增长
		if (!m_bOwner || dwNeedLength <= m_dwSize)
			return ;

		// 计算增长后的缓存长度
		DWORD dwCount = dwNeedLength / GROW_LENGTH;
		dwCount += dwNeedLength % GROW_LENGTH > 0 ? 1: 0;
		DWORD dwRealSize = dwCount * GROW_LENGTH;

		// 分配新的内存
		PVOID pNewBuffer = malloc(dwRealSize);
		if (!pNewBuffer)
			return;

		// 复制原始数据并释放内存
		memset(pNewBuffer, 0, dwRealSize);
		if (m_pData)
		{
			memcpy(pNewBuffer, m_pData, m_dwSize);
			free(m_pData);
		}
		
		// 重设缓存和长度
		m_dwSize = dwRealSize;
		m_pData = pNewBuffer;
	}

public:
	CSimpleMemFile() : m_pData(NULL), m_dwSize(0), m_dwOffset(0), m_bOwner(TRUE)
	{
	};

	~CSimpleMemFile()
	{
		Release();
	};

	void Attach(PVOID pBuffer, DWORD dwSize)
	{
		Release();

		m_pData		= pBuffer;
		m_dwSize	= dwSize;
		m_dwOffset	= 0;
		m_bOwner	= FALSE;
	}

	// 移动内部偏移到缓存开头
	void SeekToBegin()
	{
		m_dwOffset	= 0;
	}

	// 把内部的数据读取到指定的缓存中
	DWORD Read(PVOID pReadBuffer, DWORD dwReadSize)
	{
		// 一系列的检查，判断是否可读
		if (!pReadBuffer || !dwReadSize || m_dwOffset >= m_dwSize || !m_pData)
			return 0;

		// 复制数据到指定缓存
		DWORD dwRead = m_dwOffset+dwReadSize > m_dwSize ? m_dwSize-m_dwOffset : dwReadSize;
		memcpy(pReadBuffer, (PBYTE)m_pData+m_dwOffset, dwRead);
		m_dwOffset += dwRead;
		return dwRead;
	}

	// 把指定的数据写入内部缓存
	DWORD Write(PVOID pWriteData, DWORD dwLength)
	{
		// 判断是否需要写入
		if (!pWriteData || !dwLength)
			return 0;

		// 自动增长内存
		Growth(dwLength);
		if (m_dwOffset >= m_dwSize)
			return 0;

		// 把指定数据复制到缓存
		DWORD dwWrite = m_dwOffset+dwLength > m_dwSize ? m_dwSize-m_dwOffset : dwLength;
		memcpy((PBYTE)m_pData+m_dwOffset, pWriteData, dwWrite);
		m_dwOffset += dwWrite;
		return dwWrite;
	}

};

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl

CGTRichEdit::CGTRichEdit()
    : m_bCallbackSet( FALSE )
    , m_bNotifyMenu( FALSE )
    , m_bReFresh( TRUE )
    , m_bInput( TRUE )
	, m_bforbidWheel(false)
{
	m_pOle = NULL;
	m_bPopMenu = false;
	m_bLBDownSendToParent = false;
	m_bInsertPicFormClipboard = false;
	m_nMaxParagraphCount = 0;
	ZeroMemory(m_szCutScreenPicSavePath, sizeof(m_szCutScreenPicSavePath));
}

CGTRichEdit::~CGTRichEdit()
{
}	

void CGTRichEdit::SetAllowInsertPicFromClipboard(bool bAllow)
{	
	m_bInsertPicFormClipboard = bAllow;
}

HWND CGTRichEdit::Create(HWND hWndParent, ATL::_U_RECT rect, LPCTSTR szWindowName ,
			DWORD dwStyle , DWORD dwExStyle ,
			ATL::_U_MENUorID MenuOrID , LPVOID lpCreateParam)
{
	HWND hWnd = CRichEditCtrl::Create(hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);

	// 避免ASSERT错误 [4/3/2010 何春龙]
	m_hWnd = NULL;
	SubclassWindow(hWnd);
	m_pOle = CreateIMRichEdit( GetOleInterface(), this);
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask |= CFM_FACE;
	_tcscpy(cf.szFaceName , L"Tahoma");//设置字体
	SetDefaultCharFormat(cf);
	return hWnd;
}

LRESULT CGTRichEdit::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
	m_pOle = CreateIMRichEdit( GetOleInterface(), this);
 	return 1;
}

LRESULT CGTRichEdit::OnDestroy     (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	if(m_pOle)
	{
		m_pOle->DeleteThis();
	}

	m_pOle = NULL;
	return 0;
}

void CGTRichEdit::OnEraseBackground(HDC dc, const RECT& rect)
{
	//保持与界面设置一致
	COLORREF cf = (COLORREF)xskin::QueryUIValue(SKINID(RichEdit.ChatBackgroupColor));

	COLORREF old_color = SetBkColor(dc,cf);
	if(old_color != CLR_INVALID)
	{
		ExtTextOut(dc, 0,0, ETO_OPAQUE, &rect, NULL, 0, NULL);
		SetBkColor(dc, old_color);
	}
}

void CGTRichEdit::SetAutoUrlDetect(BOOL bAutoUrl)
{
	unsigned mask = ::SendMessage(m_hWnd, EM_GETEVENTMASK, 0, 0);
	if(bAutoUrl)
	{
		::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
		::SendMessage(m_hWnd, EM_AUTOURLDETECT, TRUE, 0);   //自动检测URL
	}
	else
	{
		
		::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, mask & ~ENM_LINK);
		::SendMessage(m_hWnd, EM_AUTOURLDETECT, FALSE, 0);   
	}
}


void CGTRichEdit::InsertLink(LPCTSTR pText)
{
	m_pOle->InsertLink(pText);
}	

long CGTRichEdit::StreamInFromResource(int iRes, LPCTSTR sType)
{
	HINSTANCE hInst = _Module.GetModuleInstance();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);
	
	DWORD len = SizeofResource(hInst, hRsrc); 
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc); 
	ATLASSERT(lpRsrc); 
 
	CSimpleMemFile mfile;
	mfile.Attach(lpRsrc, len); 

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD) &mfile;

	return StreamIn( SF_RTF, es );
}

long CGTRichEdit::StreamInFromMem(LPCSTR pszContent, int nType)
{
	CSimpleMemFile mfile;
	BYTE* lpRsrc = (BYTE*)pszContent;
	UINT len = strlen(pszContent) ;
	mfile.Attach(lpRsrc, len); 

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD) &mfile;

	return StreamIn( nType, es );
}

long CGTRichEdit::StreamOutToMem(LPSTR pszBuff, long lLen)
{
	CSimpleMemFile cFile;
	EDITSTREAM es;

	es.dwCookie = (DWORD) &cFile;
	es.pfnCallback = MyStreamOutCallback; 
	long lOut = StreamOut(SF_RTFNOOBJS, es);
	UINT uRet  = 0;	
	if(lOut > lLen || lOut == 0)
		return -1;
	else
	{
		cFile.SeekToBegin();
		uRet = cFile.Read(pszBuff, lLen);
	}
	return uRet;
}

/* static */
DWORD CALLBACK CGTRichEdit::readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead)			// number of bytes actually read
{
	CSimpleMemFile* fp = (CSimpleMemFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

/* static */
DWORD CALLBACK CGTRichEdit::MyStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CSimpleMemFile* pFile = (CSimpleMemFile*) dwCookie;
	*pcb = pFile->Write(pbBuff, cb);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CGTRichEdit::SetPopMenu(bool bPop)
{
	m_bPopMenu = bPop;
}

void CGTRichEdit::CreateMenu(DWORD userData)
{
    if (!CWndManager::GetInst()->m_pClassRoomDlg->GetUserlistWnd()->userInList(userData))
        return;

    biz::SLUserInfo sMyInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(CEduSeeion::GetInst()->_nUserId);
	auto pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(CEduSeeion::GetInst()->_nClassRoomId);
	HMENU hMenu = CreatePopupMenu();		

	auto sRoomInfo = pRoom->GetRoomBaseInfo();
    
	do
	{	
		if (userData != CEduSeeion::GetInst()->_nUserId)
		{
			if( (sMyInfo._nUserState & biz::eUserState_user_mute || !(sRoomInfo._nClassMode & biz::Eclassroommode_Textable)) && 
				(!CEduSeeion::GetInst()->IsTeacher()))				
			{
				::InsertMenuW(hMenu,3,MF_BYPOSITION | MF_STRING | MF_GRAYED,ID_PRIVATE_CHAT, LANGUAGE::GetString(L"IDS_FRIENTITEM_TALK_WITH_TA"));
			}
			else
			{
				::InsertMenuW(hMenu,3,MF_BYPOSITION | MF_STRING ,ID_PRIVATE_CHAT, LANGUAGE::GetString(L"IDS_FRIENTITEM_TALK_WITH_TA"));
			}	
		}			
		else
		{
			//自己仅查看资料
			break;
		}
		::InsertMenuW(hMenu,4,MF_BYPOSITION | MF_STRING,ID_VIEW_STUDENT_INFO, LANGUAGE::GetString(L"IDS_FRIENTITEM_VIEW_STU_INFO"));
		::InsertMenuW(hMenu,5,MF_BYPOSITION | MF_SEPARATOR,0,L"");

		auto sRoomInfo = pRoom->GetRoomBaseInfo();
		if( sMyInfo.nUserAuthority != biz::UserAu_Teacher && sMyInfo.nUserAuthority != biz::UserAu_Assistant)
		{
			break;
		}
		
		biz::SLUserInfo sUInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userData);
		auto pSpeakUserInfo = pRoom->GetSpeakUserInfo();

		if( sMyInfo.nUserAuthority == biz::UserAu_Teacher)
		{
			if(sUInfo.nUserAuthority == biz::UserAu_Assistant)
			{
				::InsertMenuW(hMenu,0,MF_BYPOSITION,ID_CANCEL_ASSISTANT, LANGUAGE::GetString(L"IDS_FRIENTITEM_CANCEL_ASSISTANT"));				
			}
			else
			{
				::InsertMenuW(hMenu,0,MF_BYPOSITION,ID_SET_ASSISTANT, LANGUAGE::GetString(L"IDS_FRIENTITEM_SET_ASSISTANT"));
			}
		}

		if(sRoomInfo._nClassState != biz::EClassroomstate_Doing)
		{
			break;
		}

		if(sUInfo._nUserState & biz::eUserState_user_mute)
		{
			::InsertMenuW(hMenu,6,MF_BYPOSITION,ID_ENABLE_SPEAK, LANGUAGE::GetString(L"IDS_FRIENTITEM_ENABLE_SPEAK"));
		}
		else
		{
			::InsertMenuW(hMenu,6,MF_BYPOSITION,ID_DISABLE_SPEAK, LANGUAGE::GetString(L"IDS_FRIENTITEM_DISABLE_SPEAK"));
		}

		::InsertMenuW(hMenu,7,MF_BYPOSITION,ID_KICK_OUT, LANGUAGE::GetString(L"IDS_FRIENTITEM_KICKOUT"));

		if(sMyInfo.nUserAuthority != biz::UserAu_Teacher)
		{
			break;
		}

		if(sUInfo._nUserState & biz::eUserState_speak)
		{
			::InsertMenuW(hMenu,1,MF_BYPOSITION,ID_CANCEL_SPEAK, LANGUAGE::GetString(L"IDS_FRIENTITEM_CANCEL_SPEAK"));
			::InsertMenuW(hMenu,2,MF_BYPOSITION | MF_SEPARATOR,0,L"");
		}
		else if(NULL != pSpeakUserInfo && sUInfo.nUserId == pSpeakUserInfo->nUserId)
		{
			::InsertMenuW(hMenu,1,MF_BYPOSITION,ID_CANCEL_SPEAK, LANGUAGE::GetString(L"IDS_FRIENTITEM_CANCEL_SPEAK"));
			::InsertMenuW(hMenu,2,MF_BYPOSITION | MF_SEPARATOR,0,L"");
		}
		else
		{
			if(sUInfo._nUserState & biz::eUserState_ask_speak)
			{
				::InsertMenuW(hMenu,1,MF_BYPOSITION,ID_PREMISSION_SPEAK, LANGUAGE::GetString(L"IDS_FRIENTITEM_PREMISSION_SPEAK"));
				::InsertMenuW(hMenu,2,MF_BYPOSITION | MF_SEPARATOR,0,L"");
			}
			else
			{	
				if( sUInfo.nUserAuthority != biz::UserAu_Visitor &&
					sUInfo.nUserAuthority != biz::UserAu_Observer )
				{
					/*if (sUInfo.nDevice == 1)
					{
						::InsertMenuW(hMenu,1,MF_BYPOSITION | MF_STRING | MF_GRAYED,ID_PREMISSION_SPEAK,L"提问");
					}
					else*/
					{
						::InsertMenuW(hMenu,1,MF_BYPOSITION,ID_PREMISSION_SPEAK, LANGUAGE::GetString(L"IDS_FRIENTITEM_PREMISSION_ASK"));
					}
					::InsertMenuW(hMenu,2,MF_BYPOSITION | MF_SEPARATOR,0,L"");
				}
			};
		}

	}while(FALSE);
	
	CPoint pt;
	GetCursorPos( &pt );
	int nCMd = ::TrackPopupMenu(hMenu,TPM_LEFTALIGN | TPM_RETURNCMD,pt.x,pt.y,0,CWndManager::GetInst()->m_pClassRoomDlg->GetUserlistWnd()->m_hWnd,NULL);
	if(nCMd > 0)
	{
		::PostMessage(CWndManager::GetInst()->m_pClassRoomDlg->GetUserlistWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(nCMd, 0), (LPARAM)userData);
	}
}

LRESULT CGTRichEdit::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
	if(!m_bPopMenu)
	{
		bHandled = false;
		return 0; 
	}
	// 沿用老GT的处理 [5/4/2010 何春龙]
	m_bPop = TRUE;
	DefWindowProc(uMsg, wParam, lParam);
	if (!m_bPop)
		return 0;

    POINT pt =  {GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
    wchar_t wszPicPath[MAX_PATH + 1] = { 0 };
    DWORD userData = 0;
    if (m_pOle->PtInZoomPictureObjectWithUserData(pt, wszPicPath, MAX_PATH, userData))
    {
        CreateMenu(userData);
        return 0;
    }

    SetFocus();
	if(m_bNotifyMenu)
	{
		NMHDR nmh;
		nmh.code = GTN_RICH_RBUTTON_DOWN;    // Message type defined by control.
		nmh.idFrom = GetDlgCtrlID();
		nmh.hwndFrom = m_hWnd;
		GetParent().SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(),(LPARAM)&nmh);
	}
	else
	{
		CMenu popmenu;
		if(GetStyle() & ES_READONLY_SIMPLE)
		{
			popmenu.CreatePopupMenu();
			popmenu.AppendMenu( 0, ID_RICH_COPY, LANGUAGE::GetString(L"IDS_RICHEDIT_COPY") );
			popmenu.AppendMenu( MF_SEPARATOR );
			popmenu.AppendMenu( 0, ID_RICH_SELECTALL, LANGUAGE::GetString(L"IDS_RICHEDIT_SELALL") );
		}
		else if(GetStyle() & ES_READONLY)
		{
			popmenu.CreatePopupMenu();
			popmenu.AppendMenu( 0, ID_RICH_CLEAR, LANGUAGE::GetString(L"IDS_RICHEDIT_CLEAR"));
			popmenu.AppendMenu( MF_SEPARATOR );
			popmenu.AppendMenu( 0, ID_RICH_COPY, LANGUAGE::GetString(L"IDS_RICHEDIT_COPY") );

			popmenu.AppendMenu( MF_SEPARATOR );
			popmenu.AppendMenu( 0, ID_RICH_SELECTALL, LANGUAGE::GetString(L"IDS_RICHEDIT_SELALL") );

			UINT nSel=( ( GetSelectionType() != SEL_EMPTY ) ? 0 : MF_GRAYED );
			popmenu.EnableMenuItem( ID_RICH_COPY, MF_BYCOMMAND | nSel );

			UINT nPaste=( CanPaste() ? 0 : MF_GRAYED );
			popmenu.EnableMenuItem( ID_RICH_PASTE, MF_BYCOMMAND | nPaste );
		}
		else
		{
			if ( !popmenu.CreatePopupMenu() )
			{
				return false;
			}
			
			popmenu.AppendMenu( 0, ID_RICH_CUT, LANGUAGE::GetString(L"IDS_RICHEDIT_CUT") );
			popmenu.AppendMenu( 0, ID_RICH_COPY, LANGUAGE::GetString(L"IDS_RICHEDIT_COPY"));
			popmenu.AppendMenu( 0, ID_RICH_DEL, LANGUAGE::GetString(L"IDS_RICHEDIT_DEL"));
			popmenu.AppendMenu( 0, ID_RICH_PASTE, LANGUAGE::GetString(L"IDS_RICHEDIT_PASTE"));

			popmenu.AppendMenu( MF_SEPARATOR );
			popmenu.AppendMenu( 0, ID_RICH_SELECTALL, LANGUAGE::GetString(L"IDS_RICHEDIT_SELALL") );

			UINT nSel=( ( GetSelectionType() != SEL_EMPTY ) ? 0 : MF_GRAYED );
			popmenu.EnableMenuItem( ID_RICH_CUT, MF_BYCOMMAND | nSel );
			popmenu.EnableMenuItem( ID_RICH_COPY, MF_BYCOMMAND | nSel );
			popmenu.EnableMenuItem( ID_RICH_DEL, MF_BYCOMMAND | nSel );

			UINT nPaste=( CanPaste() ? 0 : MF_GRAYED );
			popmenu.EnableMenuItem( ID_RICH_PASTE, MF_BYCOMMAND | nPaste );
		}

		CPoint pt;
		GetCursorPos( &pt );
		popmenu.TrackPopupMenu( TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd );
		popmenu.DestroyMenu();
	}
	return 0;
}

//richedit20W 下 getseltext失效
WTL::CString CGTRichEdit::GetSelTextW()
{
	CHARRANGE cr;
	GetSel(cr);

	// Allocate memory for the buffer
	LONG len = 0; 
	if(-1 == cr.cpMax && 0 == cr.cpMin)
		len = GetTextLengthEx(GTL_NUMCHARS , 1200);
	else
		len = cr.cpMax - cr.cpMin;

	LPTSTR lptbuf = new TCHAR[len + 1];

	// Get the selected text from the Rich Edit
	SendMessage(EM_GETSELTEXT, 0, (LPARAM) lptbuf);

	WTL::CString strSelect(lptbuf);

	// Release allocated memory
	delete [] lptbuf;

	return strSelect;
}

void CGTRichEdit::InsertFace(WTL::CString strPicPath)
{
	WTL::CString strTempName(strPicPath);
	strTempName.Replace(_T('/'), _T('_'));
	strTempName.Replace(_T('\\'), _T('_'));
	WTL::CString strpicFullPath;
	strpicFullPath.Format(_T("%s%s"), CEnv::GetTempPath(), strTempName);
	bool bDownGif = false;
	DWORD theFileAttribute = GetFileAttributes(strpicFullPath);
	if( theFileAttribute == -1 )
	{
		void* buf = NULL;
		int len = 0;
		CDeComp::Inst()->ExtractToMem(CDeComp::Image, strPicPath, &buf, &len);
		if(buf && len > 0)
		{
			HANDLE hFile = ::CreateFile(strpicFullPath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			ATLASSERT(hFile);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				unsigned long nWrite = 0;
				::WriteFile(hFile, buf, len, &nWrite, NULL);
				::CloseHandle(hFile);
			}
			CDeComp::Inst()->FreeBuffer(buf);
		}
		else
			return;
	}
	
	
	InsertPicture(strpicFullPath, 0);
	::SendMessage(m_hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
}

void CGTRichEdit::InsertFaceMd5(WTL::CString strPicPath)
{
	if(CLFile::FileIsExist(strPicPath))
	{
		InsertPicture(strPicPath, 0,true);
		::SendMessage(m_hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
	}
	else
	{
		IRichEditOle* pRichEditOle = GetOleInterface();
		int nPicCount = pRichEditOle->GetObjectCount(); 
		m_mapDownGifList[nPicCount] = strPicPath;
		
		InsertFace(LANGUAGE::GetString(L"IDS_RICHEDIT_LOADINGPIC"));
	}
}

void CGTRichEdit::InsertFace(WTL::CString strPicPath, UINT uData, int nGameId, int nEmotionId, int nPicId)
{
	WTL::CString strTempName(strPicPath);
	strTempName.Replace(_T('/'), _T('_'));
	strTempName.Replace(_T('\\'), _T('_'));
	WTL::CString strpicFullPath;
	strpicFullPath.Format(_T("%s%s"), CEnv::GetTempPath(), strTempName);
	bool bDownGif = false;
	DWORD theFileAttribute = GetFileAttributes(strpicFullPath);
	if( theFileAttribute == -1 )
	{
		void* buf = NULL;
		int len = 0;
		CDeComp::Inst()->ExtractToMem(CDeComp::Faces, strPicPath, &buf, &len);
		if(buf && len > 0)
		{
			HANDLE hFile = ::CreateFile(strpicFullPath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			ATLASSERT(hFile);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				unsigned long nWrite = 0;
				::WriteFile(hFile, buf, len, &nWrite, NULL);
				::CloseHandle(hFile);
			}
			CDeComp::Inst()->FreeBuffer(buf);
		}
		else
		{
			IRichEditOle* pRichEditOle = GetOleInterface();
			int nPicCount = pRichEditOle->GetObjectCount(); 
			m_mapDownGifList[nPicCount] = strpicFullPath;			
			bDownGif = true;
			strpicFullPath = LANGUAGE::GetString(L"IDS_RICHEDIT_LOADINGPIC");
		}
	}
	
	if(bDownGif)
	{
		InsertFace(strpicFullPath);
	}
	else
	{
		InsertPicture(strpicFullPath, uData);		
	}
	
	::SendMessage(m_hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);	
}

LRESULT CGTRichEdit::OnPaste(WORD, WORD, HWND, BOOL&) 
{ 
	do 
	{
		if(m_bInsertPicFormClipboard) {
			break;
		};

		if(!OpenClipboard()) {
			break;
		};

		/*if(IsClipboardFormatAvailable(CF_BITMAP)) {
			CloseClipboard();
			break;
		};*/

		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
		bool	br = AppendBitmap(hBitmap);		
		CloseClipboard();

		if(br) {
			return 0;
		};

	} while (FALSE);

	Paste();
	if(m_nMaxParagraphCount)
	{
		int nCount = m_pOle->GetParagraphCount();
		if(nCount > m_nMaxParagraphCount)
		{
			m_pOle->DeleteParagraphs(m_nMaxParagraphCount, nCount);
			nCount = GetTextLengthEx();
			SetSel(nCount - 1, nCount);
			ReplaceSel(NULL, FALSE);
		}
	}
	return 0;

}

ATL::CString CGTRichEdit::GetMd5ByPicData(int nIndex)
{
	ATL::CString strMd5;
	std::map<int, ATL::CString>::iterator iter = m_mapMd5IndexList.find(nIndex);
	if(iter != m_mapMd5IndexList.end())
		strMd5 = iter->second;
	return strMd5;
}

void CGTRichEdit::ClearMd5PicList()
{
	m_mapMd5IndexList.clear();
}


void CGTRichEdit::SetCutScreenPicSavePath(LPCTSTR pPath)
{
	ZeroMemory(m_szCutScreenPicSavePath, sizeof(m_szCutScreenPicSavePath));
	if(pPath)
	{
		_tcsncpy(m_szCutScreenPicSavePath, pPath, _countof(m_szCutScreenPicSavePath) - 1);
	}
}


void CGTRichEdit::UpdateGif(ATL::CString& strPic)
{
	std::map<int, ATL::CString>::iterator iter = m_mapDownGifList.begin();
	while(iter != m_mapDownGifList.end())
	{
		if(strPic == iter->second)
		{
			UpdatePicture(iter->first, strPic);
			iter = m_mapDownGifList.erase(iter);
		}
		else
			iter++;
	}
	if(m_mapDownGifList.size() == 0 && m_pOle)
	{
		int nParagaphCount = m_pOle->GetParagraphCount();
		if(nParagaphCount > 200)
		{
			m_pOle->DeleteTopParagraphs(nParagaphCount - 200);
		}
	}
}

void CGTRichEdit::UpdateGif(ATL::CString& strPic, LPCTSTR strErrorPic)
{
	std::map<int, ATL::CString>::iterator iter = m_mapDownGifList.begin();
	while(iter != m_mapDownGifList.end())
	{
		if(strPic == iter->second)
		{
			UpdatePicture(iter->first, strErrorPic);
			iter = m_mapDownGifList.erase(iter);
		}
		else
			iter++;
	}
	if(m_mapDownGifList.size() == 0 && m_pOle)
	{
		int nParagaphCount = m_pOle->GetParagraphCount();
		if(nParagaphCount > 200)
		{
			m_pOle->DeleteTopParagraphs(nParagaphCount - 200);
		}
	}
}

void CGTRichEdit::DeleteToTopNumMsg(int nCount)
{
	if(m_mapDownGifList.size() == 0 && m_pOle)
	{
		int nParagaphCount = m_pOle->GetParagraphCount();
		if(nParagaphCount > nCount)
		{
			m_pOle->DeleteTopParagraphs(nParagaphCount - nCount);
		}
	}
}

LRESULT CGTRichEdit::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    GetParent().PostMessage(WM_VSCROLL, wParam, lParam) ;	
	bHandled = FALSE;

	return 0;
}

LRESULT CGTRichEdit::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(( GetKeyState( VK_CONTROL ) & 0x8000 ) != 0 )
		return 0;
	if(!m_bforbidWheel)
	{
		GetParent().PostMessage(WM_MOUSEWHEEL, wParam, lParam) ;
		bHandled = FALSE;
	}
	return 1;
}

void CGTRichEdit::SetProtected()
{ 
    CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_PROTECTED;

    DWORD dwSelMask = GetSelectionCharFormat(cf);
    // If selection is all the same toggle PROTECTED style
    // turn it on otherwise over the whole selection
    if( (cf.dwMask & CFM_PROTECTED) & (dwSelMask & CFM_PROTECTED) )
    { 
        cf.dwEffects ^= CFE_PROTECTED; 
    }
    else
    { 
        cf.dwEffects |= CFE_PROTECTED;
    }
    cf.dwMask = CFM_PROTECTED;
    SetSelectionCharFormat(cf);
}

void CGTRichEdit::SetLink()
{ 
    CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(CHARFORMAT2);
    cf.dwMask = CFM_LINK;

   // DWORD dwSelMask = GetSelectionCharFormat(cf);
    // If selection is all the same toggle PROTECTED style
    // turn it on otherwise over the whole selection
    
    cf.dwEffects |= CFE_LINK;
    
    SetSelectionCharFormat(cf);
}

void CGTRichEdit::SetNoLink()
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_LINK;

	DWORD dwSelMask = GetSelectionCharFormat(cf);
	cf.dwMask = CFM_LINK;
	cf.dwEffects &= ~CFE_LINK;
	SetSelectionCharFormat(cf);

}


LRESULT CGTRichEdit::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == TIME_EVENT_REFRESH)
	{
		m_bReFresh = TRUE;
	}

	bHandled = FALSE;
	return 1;
}

UINT CGTRichEdit::GetTextHeight()
{
	if (GetTextLength() == 0)
		return 0;

	// 第一个字符左上角坐标
	POINT ptStart = PosFromChar(0);

	// 最后一个字符左上角坐标
	POINT ptEnd = PosFromChar(GetLimitText());

	// 得到除最后一行之外的高度
	UINT nHeight = ptEnd.y - ptStart.y;

	// 取得IRichEditOle接口
	CComPtr<IRichEditOle> spRichEditOle;
	if (!SendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&spRichEditOle))
		return nHeight;

	// 取得ITextDocument接口
	CComPtr<ITextDocument> spTextDocument;
	spRichEditOle->QueryInterface(__uuidof(ITextDocument),(void**)&spTextDocument);
	if (!spTextDocument)
		return nHeight;

	// 取得ITextRange接口
	CComPtr<ITextRange> spTextRange;
	spTextDocument->Range(-1, -1, &spTextRange);

	// 计算最后一个字符左上右下坐标
	POINT ptFirst = {0}, ptLast = {0};
	spTextRange->GetPoint(TA_TOP|TA_LEFT, &ptFirst.x, &ptFirst.y);
	spTextRange->GetPoint(TA_BOTTOM|TA_RIGHT, &ptLast.x, &ptLast.y);

	// 返回真实的高度
	return (nHeight + ptLast.y - ptFirst.y);
}

void CGTRichEdit::SetForbidWheel(bool bForbid)
{
	m_bforbidWheel = bForbid;
}

void CGTRichEdit::SetColor(COLORREF crText)
{
	CHARFORMAT2 newColorText;	
	ZeroMemory(&newColorText, sizeof(CHARFORMAT2));
	newColorText.cbSize = sizeof(CHARFORMAT2);
	newColorText.dwMask = CFM_COLOR ;
	newColorText.crTextColor = crText;
	SetDefaultCharFormat(newColorText);
}

int CGTRichEdit::GetWindowText(CSimpleString& strText) const
{
	CRichEditCtrl::GetWindowText(strText);
	return 0;
}

void CGTRichEdit::SendLBDownToParent(bool bSendToParent)
{
	m_bLBDownSendToParent = bSendToParent;
}

LRESULT CGTRichEdit::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*
	if(GetSelectionType() == SEL_OBJECT || GetSelectionType() == SEL_MULTIOBJECT)
	{
		long nStart, nEnd;
		GetSel(nStart, nEnd);
		SetSel(nEnd, nEnd);
		return 0;
	}
	*/
	/*if(m_bLBDownSendToParent)
	{
		SetFocus();
		HWND hParent = GetParent();
		::SendMessage(hParent, WM_LBUTTONDOWN, (WPARAM)hParent, 0);
	}*/	

	bHandled = false;
	return 0 ;
}

int CGTRichEdit::LimitParagraphCount(int nMax)
{
	int nTemp = m_nMaxParagraphCount;
	m_nMaxParagraphCount = nMax;
	return nTemp;
}

bool CGTRichEdit::ParagraphCountIsMax()
{
	int nCount = m_pOle->GetParagraphCount();
	if(nCount >= m_nMaxParagraphCount)
		return true;
	else
		return false;
}

LRESULT CGTRichEdit::OnChar     (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*
	HWND hParent = GetParent();
	::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(::GetDlgCtrlID(m_hWnd),EN_CHANGE), lParam);
	*/
	
	bHandled = FALSE;
	return 0;
}

LRESULT CGTRichEdit::OnKeyDown	(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	switch (wParam)
	{
	case 'V':
		{
			if(GetKeyState(VK_CONTROL) & 0x8000)
			{
				OnPaste(0,0,0,bHandled);
				return 0;
			}
		}
		break;
	case 'C':
		{
			if(GetKeyState(VK_CONTROL) & 0x8000)
			{
				OnCopy(0,0,0,bHandled);
				return 0;
			}
		}
	default:
		break;
	}

	if(m_nMaxParagraphCount > 0)
	{
		if(wParam == VK_RETURN)
		{
			if(ParagraphCountIsMax())
				return 0;
		}
// 		else if(wParam == 'V')
// 		{
// 			if(GetKeyState(VK_CONTROL) & 0x8000)
// 			{
// 				OnPaste(0,0,0,bHandled);
// 				return 0;
// 			}
// 		}
	}
	HWND hParent = GetParent();
	::PostMessage(hParent, WM_KEYDOWN, wParam, lParam);
	bHandled = FALSE;
	return 0;
}

const int nUrlProtoclNum = 6;
const TCHAR* urlProtoclList[nUrlProtoclNum] = { L"ftp:", L"http:", L"https:", L"mailto", L"file:", L"www"};

void CGTRichEdit::AppendTextDetectUrl(ATL::CString& strText)
{
	int nTotalLen = strText.GetLength();
	if(nTotalLen == 0)
		return;
	int nPos = 0;
	ATL::CString strTemp(strText);
	strTemp.MakeLower();
	int nPos2 = 0;
	int nIndex = 0;
	while(nIndex < nUrlProtoclNum && nPos < nTotalLen)
	{
		nPos2 = strTemp.Find(urlProtoclList[nIndex], nPos);
		if(nPos2 >= 0)
		{
			AppendText(strText.Mid(nPos, nPos2 - nPos));
			int nTempPos = nPos2;
			while(nTempPos < nTotalLen)
			{
				TCHAR ch = strText.GetAt(nTempPos);
				if(ch >= L'!' && ch <= L'~')
					nTempPos++;
				else
					break;
			}
			InsertLink(strText.Mid(nPos2, nTempPos - nPos2));
			nPos = nTempPos;
			nIndex = 0;
		}
		else
			nIndex++;
	}
	if(nPos < nTotalLen)
		AppendText(strText.Mid(nPos));
}

LRESULT CGTRichEdit::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
/*	POINT pt =  {GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
	
	do 
	{
		wchar_t wszPicPath[MAX_PATH + 1] = { 0 };
		if(NULL == m_pOle) {
			break;
		};

		if( !m_pOle->PtInZoomPictureObject(pt,wszPicPath,MAX_PATH)) {
			break;
		};

		
		::SetCursor(::LoadCursor(NULL,IDC_ARROW));
		bHandled = true;
		
		return 0;
	} while (false);
*/
	bHandled = false;
	return 0 ;
}

LRESULT CGTRichEdit::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt =  {GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
	bHandled = false;
	wchar_t wszPicPath[MAX_PATH + 1] = { 0 };
	if(NULL == m_pOle) {
		return 0;
	};

    DWORD userData = 0;
    if (m_pOle->PtInZoomPictureObjectWithUserData(pt, wszPicPath, MAX_PATH, userData))
    {
        if (!CWndManager::GetInst()->m_pClassRoomDlg->GetUserlistWnd()->userInList(userData))
            return 0;

        if (CEduSeeion::GetInst()->_nUserId != userData)
        {
            DWORD ret = ::PostMessage(CWndManager::GetInst()->m_pClassRoomDlg->GetUserlistWnd()->m_hWnd, WM_COMMAND, ID_PRIVATE_CHAT, userData);
        }
        
        return 0;
    }

	if( !m_pOle->PtInZoomPictureObject(pt,wszPicPath,MAX_PATH))
    {
		return 0;
	};

	HWND hParent = GetParent().m_hWnd;
	CWndManager::GetInst()->ShowImageShowDlg(SW_SHOW,hParent,(ATL::CStringW)wszPicPath);
	bHandled = true;
	return 0;
}

bool CGTRichEdit::AppendBitmap(HBITMAP hBitmap)
{
	if(NULL==hBitmap) {
		return false;
	};
	BITMAP bm;
	::GetObject(hBitmap, sizeof(BITMAP), &bm);
	ATL::CString strPath;
	strPath.Format(L"%scutscreenqq.jpg", CEnv::GetTempPath());
	SaveBitmap2JPG(hBitmap, strPath);	

	CStringA md5;
	CaleFileMd5(strPath, md5);
	WTL::CString strMd5Path;
	WTL::CString strWMd5;
	strWMd5.Format(L"%s.jpg", CA2W(md5));
	strMd5Path.Format(L"%s%s",m_szCutScreenPicSavePath, strWMd5);								
	::CopyFile(strPath, strMd5Path, TRUE);
	int nIndex = m_mapMd5IndexList.size() + 1;
	
	InsertPicture(strMd5Path, nIndex,true);
	::DeleteFile(strPath);

	m_mapMd5IndexList.insert(std::make_pair(nIndex, strWMd5));

	return true;
	
}

bool CGTRichEdit::AppendLocalPicture(ATL::CString& pPicPath)
{	
	CStringA md5;
	CaleFileMd5(pPicPath, md5);
	WTL::CString strMd5Path;
	WTL::CString strWMd5;
	strWMd5.Format(L"%s.jpg", CA2W(md5));
	strMd5Path.Format(L"%s%s",m_szCutScreenPicSavePath, strWMd5);								
	::CopyFile(pPicPath, strMd5Path, TRUE);
	int nIndex = m_mapMd5IndexList.size() + 1;
	InsertPicture(strMd5Path, nIndex,true);

	m_mapMd5IndexList.insert(std::make_pair(nIndex, strWMd5));

	return true;

}

void CGTRichEdit::InsertPicture(WTL::CString& strPicPath,long nUserData,bool bZoom /* = false */)
{
	xskin::IResLoader * pResLoader = NULL;
	xgdi::IImage * pImage = NULL;
	long cx = 0;
	long cy = 0;

	do{
		if(!bZoom) {
			break;
		};

		pResLoader = xskin::CResLoader::inst();
		if(NULL == pResLoader) {
			break;
		};

		pImage = pResLoader->LoadImage((LPCTSTR)strPicPath);
		if(NULL == pImage) {
			break;
		};

		SIZE nImageSize = pImage->GetSize();

		long nStyle = GetWindowLong(GWL_STYLE);

		if(nStyle & ES_READONLY) {
			RECT rcWnd = { 0 };
			GetClientRect(&rcWnd);

			cx = PICTURE_SHOW_BIG_CX ;
			cy = PICTURE_SHOW_BIG_CY ;

		
			if(cy > (rcWnd.bottom - rcWnd.top - 40)) {
				cy = rcWnd.bottom - rcWnd.top - 40;
			};

		} else {

			cx = PICTURE_SHOW_SMALL_CX;
			cy = PICTURE_SHOW_SMALL_CY;
		};

		double image_scale = (double)nImageSize.cx / (double)nImageSize.cy;
		if(cx >= nImageSize.cx && cy >= nImageSize.cy) {
			cx = nImageSize.cx;
			cy = nImageSize.cy;
		} else {
			double draw_scale = (double)cx / (double)cy;
			if(image_scale > draw_scale) {
				cy = (long)(cx/image_scale);
			} else {
				cx = (long)(cy*image_scale);
			}
		};

	}while(false);

	if(pImage) {
		pResLoader->UnloadImage((LPCTSTR)strPicPath);
	};

	m_pOle->InsertPicture(strPicPath, nUserData,cx,cy);
	
	return;
}

void CGTRichEdit::UpdatePicture(long obj_index,LPCWSTR lpwszPicPath)
{
	if(NULL == lpwszPicPath || wcslen(lpwszPicPath) <= 0 || NULL == m_pOle) {
		return;
	};

	xskin::IResLoader * pResLoader = NULL;
	xgdi::IImage * pImage = NULL;
	ATL::CString strPicPath(lpwszPicPath);

	long cx = 0;
	long cy = 0;	

	do{
		pResLoader = xskin::CResLoader::inst();
		if(NULL == pResLoader) {
			break;
		};

		pImage = pResLoader->LoadImage((LPCTSTR)strPicPath);
		if(NULL == pImage) {
			break;
		};

		SIZE nImageSize = pImage->GetSize();

		long nStyle = GetWindowLong(GWL_STYLE);

		if(nStyle & ES_READONLY) {
			RECT rcWnd = { 0 };
			GetClientRect(&rcWnd);

			cx = PICTURE_SHOW_BIG_CX ;
			cy = PICTURE_SHOW_BIG_CY ;


			if(cy > (rcWnd.bottom - rcWnd.top - 40)) {
				cy = rcWnd.bottom - rcWnd.top - 40;
			};

		} else {

			cx = PICTURE_SHOW_SMALL_CX;
			cy = PICTURE_SHOW_SMALL_CY;
		};

		double image_scale = (double)nImageSize.cx / (double)nImageSize.cy;
		if(cx >= nImageSize.cx && cy >= nImageSize.cy) {
			cx = nImageSize.cx;
			cy = nImageSize.cy;
		} else {
			double draw_scale = (double)cx / (double)cy;
			if(image_scale > draw_scale) {
				cy = (long)(cx/image_scale);
			} else {
				cx = (long)(cy*image_scale);
			}
		};

	}while(false);

	if(pImage) {
		pResLoader->UnloadImage((LPCTSTR)strPicPath);
	};

	m_pOle->SetObjectPicture(obj_index,lpwszPicPath,cx,cy);

	return;
}

LRESULT CGTRichEdit::OnDownloadChatImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == CHttpSessionMgr::HTTP_SESSION_END)
	{
		LPHTTPSESSION pSess = (LPHTTPSESSION)lParam;
		if(!pSess)
			return 0;
		ATL::CString strFile(pSess->file.c_str());
		if(pSess->error.length())
		{
			WTL::CString strpicFullPath;
			strpicFullPath.Format(_T("%spicgetfail.png"), CEnv::GetTempPath());
			bool bDownGif = false;
			DWORD theFileAttribute = GetFileAttributes(strpicFullPath);
			if( theFileAttribute == -1 )
			{				void* buf = NULL;
				int len = 0;
				CDeComp::Inst()->ExtractToMem(CDeComp::Image, L"picgetfail.png", &buf, &len);
				if(buf && len > 0)
				{
					HANDLE hFile = ::CreateFile(strpicFullPath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
					ATLASSERT(hFile);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						unsigned long nWrite = 0;
						::WriteFile(hFile, buf, len, &nWrite, NULL);
						::CloseHandle(hFile);
					}
					CDeComp::Inst()->FreeBuffer(buf);
				}		
			}
			UpdateGif(strFile, strpicFullPath);
			
		}
		else
		{

			UpdateGif(strFile);
		}
		delete pSess;
		Invalidate();
		//::PostMessage(m_hWnd, WM_VSCROLL,SB_BOTTOM,0);
	}
	return 0;
}

LRESULT CGTRichEdit::OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WCHAR strFileName[MAX_PATH];
	HDROP hDropInfo = (HDROP)wParam;
	DragQueryFile(hDropInfo, 0, strFileName, MAX_PATH);  //获取拖放的文件路径

	//判断是否是文件夹
	if (GetFileAttributes(strFileName)&FILE_ATTRIBUTE_DIRECTORY)
	{
		//遍历文件夹,并上传文件夹中的文件 -- 暂时不实现

	}
	else
	{
		//上传该文件
		ATL::CString strFile(strFileName);
		int nPos = strFile.ReverseFind(L'.');
		if(nPos > 0)
		{
			ATL::CString strExtName = strFile.Mid(nPos);
			ATL::CString strAllImageFile(".png .jpg .jpeg bmp");
			if(strAllImageFile.Find(strExtName) >= 0)
			{
				AppendLocalPicture(strFile);
			}
		}
		
	}
	return 0;
}
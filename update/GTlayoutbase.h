#pragma once

#include <csslayoutwnd.h>
#include "LayoutEncrypt.h"
#include "Resource.h"
#include "DeComp.h"
#include "Env.h"
#include "gtcss.h"
#include <transwindow.h>
#include <xskinframe.h>
#include "Message.h"

extern CAppModule _Module;


template<class FrameWindow>
class CGTLayoutBase
	: public xctrl::XCssLayoutMgrWnd<FrameWindow>
	, public xctrl::XSkinFrame<FrameWindow>
	, public xctrl::ImageTransWindow<FrameWindow>
{
public:
	BEGIN_MSG_MAP(CGTLayoutBase<FrameWindow>)
		MESSAGE_HANDLER(WM_PAINT,	OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_GT_SKIN_CHANGE, OnChangeSkin)

		CHAIN_MSG_MAP(XSkinFrame<FrameWindow>)
		CHAIN_MSG_MAP(XCssLayoutMgrWnd<FrameWindow>)
		CHAIN_MSG_MAP(ImageTransWindow<FrameWindow>)
	END_MSG_MAP()

	CGTLayoutBase() :m_bChangeSkin(FALSE)
	,m_bChangeNow(FALSE)
	,m_bAlpha(FALSE)
	{
		m_hSmallIcon = NULL;
		m_hIcon = NULL;
	}

	virtual ~CGTLayoutBase()
	{
		if(m_hIcon)
		{
			::DestroyIcon(m_hIcon);
		}
		if(m_hSmallIcon)
		{
			::DestroyIcon(m_hSmallIcon);
		}
	}

	void SetBkAlpha(BOOL bAlpha)
	{
		m_bAlpha = bAlpha;
	}

	LRESULT OnEraseBkgnd( UINT, WPARAM, LPARAM, BOOL& )
	{
		return 0;
	}
		
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_bChangeSkin)
		{
			m_bChangeSkin = FALSE;
			DoChangeSkin();
		}
		CClientRect rcClient(m_hWnd);
		CUpdateRect rcDirty(m_hWnd);
		CPaintDC dc(m_hWnd);
		dc.SetBkMode(TRANSPARENT);
		ICanvas* pCanvas = GetBkgrdCanvas();

		if ( pCanvas )
		{
			DrawLayouts(pCanvas, rcDirty);
			if(!m_bAlpha)
				pCanvas->Flush(dc.m_hDC, rcDirty, rcDirty, 0 , xgdi::DEF_TRANS_COLOR);
			else
				pCanvas->Flush(dc.m_hDC, rcDirty, rcDirty, DIF_ALPHA , xgdi::DEF_TRANS_COLOR);
		}
		return 0;
	}

	
	LRESULT OnChangeSkin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ChangeSkin();
		return 0;
	}
	void LoadEncryptLayout(LPCTSTR filename)
	{
		// ���⵱ǰĿ¼���޸� [6/22/2010 �δ���]
		SetCurrentDirectory(CEnv::strExePath);

		wstring strContent;
		//CLayoutEncrypt::DecodeFile(filename, strContent);
		
		void* buf = NULL;
		int len = 0;
		CDeComp::Inst()->ExtractToMem(CDeComp::Style, filename, &buf, &len);
		CLayoutEncrypt::DecodeMem((const unsigned char*)buf, len, strContent);
		CDeComp::Inst()->FreeBuffer(buf);

		if (m_pLayout) {
			m_pLayout->InitFromBuffer(strContent.c_str());
		}

		LoadCss();
	}
//  [10/19/2010 κ����]
	BOOL LoadCss()
	{
		CssLayoutCreate x;
		m_pLayout->Visit(&x);
		list<wstring> tmp;
		for(size_t i=0; i<x.objs.size(); ++ i)
		{
			xlayout::ILayoutObj* obj = x.objs[i].obj;
			if(obj == NULL || obj->GetProperty(_T("css")) == NULL )
				continue;
			wstring cssname = obj->GetProperty(_T("css"));
			const GTCSS* css =  Cakcss::Inst()->GetCSS(cssname);
			if(css != NULL)
			{
				for(GTCSS::const_iterator itr = css->begin(); itr != css->end(); ++itr)
				{
					obj->Set_Property(itr->name.c_str(), itr->attr.c_str());
					if(itr->name == _T("image") || itr->name == _T("bkgrd-image"))
						tmp.push_back(itr->attr);
				}
			}
		}

		list<wstring>::iterator itr = tmp.begin();
		for(itr = tmp.begin(); itr != tmp.end(); ++itr)
		{
			xskin::CResLoader::inst()->ReLoadTexture((*itr).c_str());
		}
		tmp.clear();

		return TRUE;
	}

	virtual void PreChangeSkin(){}
	virtual void SkinChanged(){}

	BOOL ChangeSkin()
	{
		if(m_bChangeNow)
		{//����ִ�л�������
			DoChangeSkin();
		}
		else
		{//���ڿɼ�ʱ���̻�����������ɼ����ӳٵ�OnPaintʱ������
			m_bChangeSkin = !(IsWindowVisible() && !IsIconic());
			if(!m_bChangeSkin)
				DoChangeSkin();
		}
		return TRUE;
	}

	BOOL DoChangeSkin()
	{
		PreChangeSkin();
		BOOL bVisible = IsWindowVisible();
		if(bVisible)
		{
			SetRedraw(FALSE);
		}
		//����css
		LoadCss();
		//�����޴���obj��ʽ
		for(size_t i=0; i<__cssWndList.size(); ++i)
		{
			__cssWndList[i]->AttachLayout(__cssWndList[i]->GetLayoutObj());
		}
		//�����д�����ʽ
		for(LAYOUT_MAP::iterator itr = m_mapObj2Wnd.begin(); itr != m_mapObj2Wnd.end(); ++itr)
		{
			itr->second->AttachLayout(itr->first);
		}

		//�����С
		SetFrameSize();


// 		// ��С������û����Ӧonsize,��ǿ����ʾ [12/30/2010 κ����]
// 		BOOL bIconic = FALSE;
// 		if(IsIconic())
// 		{
// 			bIconic = TRUE;
// 			ShowWindow(SW_HIDE);
// 			ShowWindow(SW_RESTORE);
// 		}
		//ǿ��ˢ��
		WTL::CRect rc;
		::GetWindowRect(m_hWnd, rc);
		if(XSkinFrame<FrameWindow>::_isFixSize)//��С�̶�
		{
			SetFixSize(FALSE);
			SetWindowPos(NULL, 0,0,rc.Width()-1, rc.Height()+1, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
			SetWindowPos(NULL, 0,0,rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
			//SetWindowPos(NULL, 0,0,XSkinFrame<FrameWindow>::_szMini.cx+1, XSkinFrame<FrameWindow>::_szMini.cy+1, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
			//SetWindowPos(NULL, 0,0,XSkinFrame<FrameWindow>::_szMini.cx, XSkinFrame<FrameWindow>::_szMini.cy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
			SetFixSize(TRUE);
			RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
		else
		{
			if(rc.Width() < XSkinFrame<FrameWindow>::_szMini.cx || rc.Height() < XSkinFrame<FrameWindow>::_szMini.cy)
			{
				if(rc.Width() < XSkinFrame<FrameWindow>::_szMini.cx)  rc.right = rc.left + XSkinFrame<FrameWindow>::_szMini.cx;
				if(rc.Height() < XSkinFrame<FrameWindow>::_szMini.cy) rc.bottom = rc.top + XSkinFrame<FrameWindow>::_szMini.cy;
				SetWindowPos(NULL, 0,0,XSkinFrame<FrameWindow>::_szMini.cx, XSkinFrame<FrameWindow>::_szMini.cy, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
			}
			else
			{
				SetWindowPos(NULL, 0,0,rc.Width()-1, rc.Height()+1, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
				SetWindowPos(NULL, 0,0,rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
			}
		}
		
		if(bVisible)
		{
			SetRedraw(TRUE);
			RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}

		SkinChanged();
// 		if(bIconic)
// 		{
// 			ShowWindow(SW_MINIMIZE);
// 		}
		return TRUE;
	}

	void SetFrameSize()
	{
		xlayout::ILayoutObj* obj = m_pLayout->GetLayoutObj(_T("main"));
		if(obj == NULL)
			return;
		LPCTSTR pTmp = NULL;
		pTmp = obj->GetProperty(_T("frame_fix_size"));
		int nFixSize = pTmp != NULL ? _ttoi(pTmp) : 0;
		XSkinFrame<FrameWindow>::SetFixSize(nFixSize > 0);	
		pTmp = obj->GetProperty(_T("frame_min_width"));
		int nMinWidth = pTmp != NULL ? _ttoi(pTmp) : 0;
		pTmp = obj->GetProperty(_T("frame_min_height"));
 		int nMinHeight = pTmp != NULL ? _ttoi(pTmp) : 0;
		
		if(CEnv::IsOSHigherThanXP())
		{
			nMinWidth += 8;
			nMinHeight += 8;
		}

 		XSkinFrame<FrameWindow>::SetMiniSize(nMinWidth, nMinHeight);
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAMEBIG), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(m_hIcon, TRUE);

		m_hSmallIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(m_hSmallIcon, FALSE);
		
		SetFrameSize();
//��ʱ�ӿؼ�δ���������� setwindowpos����ַ�������
// 		if(XSkinFrame<FrameWindow>::_szMini.cx > 0 && XSkinFrame<FrameWindow>::_szMini.cy > 0)
// 		{
// 			BOOL bFixSize = XSkinFrame<FrameWindow>::_isFixSize;	
// 			SetFixSize(FALSE);
// 			SetWindowPos(NULL, 0,0,XSkinFrame<FrameWindow>::_szMini.cx, XSkinFrame<FrameWindow>::_szMini.cy, SWP_NOMOVE|SWP_NOZORDER);
// 			SetFixSize(bFixSize);
// 		}
		return __super::OnCreate(uMsg, wParam, lParam, bHandled);
	}

private:
	BOOL m_bChangeSkin;
	BOOL m_bChangeNow;
	BOOL m_bAlpha;
	HICON m_hIcon;
	HICON m_hSmallIcon;
};

#pragma once

#include "skinloader.h"
#include "xgdi/xgdiplus.h"
#include <LayoutWindow.h>
#include <xuimisc.h>
#include <wtlstatic.h>
#include <wtlpicture.h>
#include <wtlbutton.h>
#include <atlddx.h>
#include "GTlayoutbase.h"
#include "GTAsideHide.h"
#include "GTMenuHandle.h"
#include "GTComboboxView.h"
#include <wtledit.h>
#include "GTRichEdit.h"
#include "Progress.h"
#include "wtlslider.h"

class CLUpdateDlg : public CGTLayoutBase<CLUpdateDlg>
{
public:
	typedef xctrl::CButton		CButton;
	typedef xctrl::CStatic		CStatic;
	typedef xctrl::CPicture		CPicture;
	typedef xctrl::ICanvas		ICanvas;
	typedef xctrl::CEdit		CEdit;
	typedef xctrl::CProgress	CProgress;

	CLUpdateDlg();
	~CLUpdateDlg();
public:
	BEGIN_MSG_MAP(CLUpdateDlg)
		MESSAGE_HANDLER(WM_CREATE, OnCreate);
		MESSAGE_HANDLER(WM_PAINT,	OnPaint)
		MESSAGE_HANDLER(WM_DESTROY,	OnDestroy)

		MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK,	OnNcLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,	OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,	OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE,	OnMouseMove)
		MESSAGE_HANDLER(WM_SYSCOMMAND,	OnSysCommand)
		MESSAGE_HANDLER(WM_EDU_CHANGE_PROGRESSPOS,	OnSliProgressPos)
		MESSAGE_HANDLER(WM_TIMER,		OnTimer)

		COMMAND_HANDLER(XLAYOUT_ID(BtnMin), BN_CLICKED, DoMin)
		COMMAND_HANDLER(XLAYOUT_ID(BtnClose), BN_CLICKED, DoClose)
		COMMAND_HANDLER(XLAYOUT_ID(BtnOK),	BN_CLICKED, DoOk)

		CHAIN_MSG_MAP(CGTLayoutBase<CLUpdateDlg>)
	END_MSG_MAP()

protected:
	LRESULT OnCreate		(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnPaint			(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnDestroy		(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnLButtonDown	(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnLButtonUp		(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnMouseMove		(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnSysCommand	(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnSliProgressPos(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnNcLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnTimer			(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT DoMin			(WORD, WORD, HWND, BOOL&);
	LRESULT DoClose			(WORD, WORD, HWND, BOOL&);
	LRESULT DoOk		(WORD, WORD, HWND, BOOL&);

private: 
	void CreateLayoutElement();


private:
	XLAYOUT_DEF(CPicture,	DlgLogo);
	XLAYOUT_DEF(CStatic,	TxtTitel);
	
	XLAYOUT_DEF(CStatic,	TxtUpdateInfo);
	XLAYOUT_DEF(CStatic,	TxtPos);

	XLAYOUT_DEF(CButton,	BtnMin);
	XLAYOUT_DEF(CButton,	BtnClose);

	XLAYOUT_DEF(CProgress,	SldProgress);

	XLAYOUT_DEF(CButton,	BtnOK);

private:
	int		m_nTotalPos;
	int		m_nNowPos;

	HBRUSH	m_hPwnBrush;

};


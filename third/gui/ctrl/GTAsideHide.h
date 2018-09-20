#pragma once


#define SLIDE_STEPS	15
#define EDGE_OFFSET	15

#define HIDEWINDOWS_SHOWHEGIHT	5
typedef enum _HIDEMODE
{
	HM_NONE,
	HM_TOP,
	HM_LEFT,
	HM_BOTTOM,
	HM_RIGHT,
}
HIDEMODE;

typedef enum _TASKBAR_POS
{
	TASKBAR_UNKNOWN,
	TASKBAR_TOP,
	TASKBAR_LEFT,
	TASKBAR_BOTTOM,
	TASKBAR_RIGHT,
}
TASKBAR_POS;

enum {
	TIME_EVENT_CHECK_MOUSE	= 1000	// 检测鼠标的位置
	,TIME_EVENT_FLASH_WND		// 贴边隐藏动画
};

template <class TWindow>
class CGTAsideHide
{
public:

	CGTAsideHide(void)
	:m_HideMode(HM_NONE)
	,m_LastMode(HM_NONE)
	,m_bHeightChanged(FALSE)
	,m_nHeightOrigin(0)
	,m_bHiding(FALSE)
	,m_bIsCheck(FALSE)
	,m_bIsSlide(FALSE)
	{
	}

	virtual ~CGTAsideHide(void)
	{
	}

	BEGIN_MSG_MAP(CGTAsideHide)
		MESSAGE_HANDLER(WM_TIMER,		OnTimer)
		MESSAGE_HANDLER(WM_MOVING,		OnMoving)
		MESSAGE_HANDLER(WM_NCHITTEST,	OnPreCheck)
		MESSAGE_HANDLER(WM_NCMOUSEMOVE, OnPreCheck)
		MESSAGE_HANDLER(WM_SETCURSOR,	OnPreCheck)
		MESSAGE_HANDLER(WM_COMMAND,		OnPreCheck)
		MESSAGE_HANDLER(WM_SYSCOMMAND,  OnPreCheck)
		MESSAGE_HANDLER(WM_NCACTIVATE,  OnPreCheck)
		MESSAGE_HANDLER(WM_SHOWWINDOW,  OnPreCheck)
	END_MSG_MAP()

private:
	HIDEMODE	m_HideMode;				// 贴边模式
	HIDEMODE	m_LastMode;				// 隐藏前的模式
	UINT		m_nHeightOrigin;		// 贴边前的高度
	BOOL		m_bHeightChanged;		// 是否改变了尺寸
	BOOL		m_bIsCheck;				// 是否已经在检测鼠标
	BOOL		m_bHiding;				// 是否正在隐藏
	BOOL		m_bIsSlide;				// 是否正在滑动

public:
	HIDEMODE GetHideMode()	{ return m_HideMode; }
	void SetHideMode(HIDEMODE hideMode)
	{
		m_HideMode = hideMode;
	}

	BOOL IsChecking()		{ return m_bIsCheck; }
	void SetCheck()
	{
		((TWindow*)this)->SetTimer(TIME_EVENT_CHECK_MOUSE, 250, NULL);
		m_bIsCheck = TRUE;
	}

	BOOL IsHiding()			{ return m_bHiding; }
	BOOL IsSliding()		{ return m_bIsSlide; }
	void SetSlide(BOOL bHiding)
	{
		m_bHiding = bHiding;
		((TWindow*)this)->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
		m_bIsSlide = TRUE;
	}

	BOOL IsHeightChanged()	{ return m_bHeightChanged; }
	UINT GetOriginHeight()	{ return m_nHeightOrigin; }
	void SetOriginHeight(UINT nOriginHeight)
	{
		m_nHeightOrigin = nOriginHeight;
		m_bHeightChanged = TRUE;
	}

	void DoSlideShow()
	{
		TWindow* pThis = (TWindow*)this;

		if (m_HideMode != HM_NONE)
		{
			m_bHiding = FALSE;
			pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
			m_bIsSlide = TRUE;
		}
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TWindow* pThis = (TWindow*)this;

		// 检测鼠标位置
		if (wParam == TIME_EVENT_CHECK_MOUSE)
		{
			// 当前鼠标位置
			POINT curPos;
			::GetCursorPos(&curPos);
			// 当前窗口位置
			CRect rectWindow;
			pThis->GetWindowRect(rectWindow);
			rectWindow.InflateRect(EDGE_OFFSET, EDGE_OFFSET);
			// 鼠标离开了检测区域
			if (!rectWindow.PtInRect(curPos))
			{
				// 取消检测
				pThis->KillTimer(TIME_EVENT_CHECK_MOUSE);
				m_bIsCheck = FALSE;
				// 滑动隐藏窗口
				m_bHiding = TRUE;
				pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
				m_bIsSlide = TRUE;
			}
		}
		// 窗口滑动动画
		else if (wParam == TIME_EVENT_FLASH_WND)
		{
			// BUG 0008015 刘睿 2010-10-19
			// 窗口最小化的时候，不执行任何会改变窗口位置的语句
			if ( !::IsIconic(pThis->m_hWnd) )
			{
				if ( !DoSlideDialog() )
				{
					pThis->KillTimer(TIME_EVENT_FLASH_WND);
					m_bIsSlide = FALSE;
					if (m_bHiding)
						::SetWindowPos(pThis->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
					else
					{
						///TODO: 以下注释可能需要实现
						/*AK_USERCONFIG& userconfig = CSessionInfo::GetInst()->GetConfigManager()->GetUserConfig();
						if (!userconfig.m_confCommon.m_bTopAlways)
							::SetWindowPos(pThis->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
					*/
					}
				}
			}
			else
			{
				pThis->KillTimer(TIME_EVENT_FLASH_WND);
			}
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPRECT lpRect = (LPRECT)lParam;
		TWindow* pThis = (TWindow*)this;
		// 判断是否设置了贴边隐藏
		///TODO:
		/*if (!CSessionInfo::GetInst()->GetConfigManager()->GetSystemConfig().m_bAutoHide)
		{
			m_HideMode = HM_NONE;
			bHandled = FALSE;
			return 0;
		}*/
		// 当前鼠标位置
		POINT curPos;
		::GetCursorPos(&curPos);
		// 当前窗口高度和宽度
		INT nHeight = lpRect->bottom - lpRect->top;
		INT nWidth = lpRect->right - lpRect->left;
		// 屏幕高度和宽度
		INT nHeightScreen = GetSystemMetrics(SM_CYSCREEN);
		INT nWidthScreen = GetSystemMetrics(SM_CXSCREEN);
		// 任务栏位置和大小
		HWND hTaskBar = ::FindWindow(_T("Shell_TrayWnd"), NULL);
		CRect rectTaskBar;
		if (hTaskBar)
			::GetWindowRect(hTaskBar, &rectTaskBar);
		// 任务栏在屏幕的位置
		TASKBAR_POS taskPos = TASKBAR_UNKNOWN;
		if (rectTaskBar.left <= 0 && rectTaskBar.top <= 0 && rectTaskBar.right >= nWidthScreen)
			taskPos = TASKBAR_TOP;
		if (rectTaskBar.left <= 0 && rectTaskBar.bottom >= nHeightScreen && rectTaskBar.right >= nWidthScreen)
			taskPos = TASKBAR_BOTTOM;
		if (rectTaskBar.left <= 0 && rectTaskBar.top <= 0 && rectTaskBar.bottom >= nHeightScreen)
			taskPos = TASKBAR_LEFT;
		if (rectTaskBar.right >= nWidthScreen && rectTaskBar.top <= 0 && rectTaskBar.bottom >= nHeightScreen)
			taskPos = TASKBAR_RIGHT;
		// 是否向上贴边
		if (curPos.y < EDGE_OFFSET)
		{
			lpRect->bottom = nHeight-HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = -HIDEWINDOWS_SHOWHEGIHT;
			m_HideMode = HM_TOP;

			// Win7下进行相关调整
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->bottom -= 4;
				lpRect->top -= 4;
			}
		}
		// 是否向左贴边
		else if (curPos.x < EDGE_OFFSET)
		{
			lpRect->right = nWidth-HIDEWINDOWS_SHOWHEGIHT;
			lpRect->left = -HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = -HIDEWINDOWS_SHOWHEGIHT;
			lpRect->bottom = nHeightScreen+HIDEWINDOWS_SHOWHEGIHT;
			// 根据任务栏调整高度
			if (taskPos == TASKBAR_BOTTOM)
				lpRect->bottom -= rectTaskBar.Height();
			if (taskPos == TASKBAR_TOP)
				lpRect->top += rectTaskBar.Height();
			// 保存原始高度
			if (!m_bHeightChanged)
			{
				m_nHeightOrigin = nHeight;
				m_bHeightChanged = TRUE;
			}
			m_HideMode = HM_LEFT;

			// Win7下进行相关调整
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->left -= 4;
				lpRect->right -= 4;
				lpRect->top -= 4;
				lpRect->bottom += 4;
			}
		}
		// 是否向右贴边
		else if (curPos.x > nWidthScreen - EDGE_OFFSET)
		{
			lpRect->right = nWidthScreen+HIDEWINDOWS_SHOWHEGIHT;
			lpRect->left = nWidthScreen - nWidth+HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = -HIDEWINDOWS_SHOWHEGIHT;
			lpRect->bottom = nHeightScreen+HIDEWINDOWS_SHOWHEGIHT;
			// 根据任务栏调整高度
			if (taskPos == TASKBAR_BOTTOM)
				lpRect->bottom -= rectTaskBar.Height();
			if (taskPos == TASKBAR_TOP)
				lpRect->top += rectTaskBar.Height();
			// 保存原始高度
			if (!m_bHeightChanged)
			{
				m_nHeightOrigin = nHeight;
				m_bHeightChanged = TRUE;
			}
			m_HideMode = HM_RIGHT;

			// Win7下进行相关调整
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->left += 4;
				lpRect->right += 4;
				lpRect->top -= 4;
				lpRect->bottom += 4;
			}
		}
		// 是否向下贴边
		else if (curPos.y > nHeightScreen - EDGE_OFFSET)
		{
			lpRect->bottom = nHeightScreen+HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = nHeightScreen - nHeight+HIDEWINDOWS_SHOWHEGIHT;
			m_HideMode = HM_BOTTOM;

			// Win7下进行相关调整
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->bottom += 4;
				lpRect->top += 4;
			}
		}
		// 不贴边
		else
		{
			// 还原窗口高度
			if (m_bHeightChanged)
			{
				lpRect->bottom = lpRect->top + m_nHeightOrigin;
				m_bHeightChanged = FALSE;
			}
			// 取消鼠标检测
			if (m_bIsCheck)
			{
				pThis->KillTimer(TIME_EVENT_CHECK_MOUSE);
				m_bIsCheck = FALSE;
			}
			m_HideMode = HM_NONE;
		}

		// 这个消息别的地方不会处理，设置为Handled
		bHandled = TRUE;
		return 1;
	}

	LRESULT OnPreCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TWindow* pThis = (TWindow*)this;

		CRect rectWindow;
		pThis->GetWindowRect(&rectWindow);

		POINT pt;
		::GetCursorPos(&pt);
		BOOL bInRect = rectWindow.PtInRect(pt);

		// 窗口隐藏和显示
		if(WM_SHOWWINDOW == uMsg)
		{
			if (!wParam)
			{
				m_LastMode = m_HideMode;
				m_HideMode = HM_NONE;
				pThis->KillTimer(TIME_EVENT_CHECK_MOUSE);
				pThis->KillTimer(TIME_EVENT_FLASH_WND);
				m_bIsCheck = FALSE;
				m_bIsSlide = FALSE;
				
			}
			else
			{
				if (m_LastMode != HM_NONE)
				{
					m_HideMode = m_LastMode;
					m_LastMode = HM_NONE;
				}
			}
			
			bHandled = FALSE;
			return 1;
		}
		// 鼠标进入窗口范围，滑动显示窗口
		if (uMsg == WM_NCHITTEST || uMsg == WM_NCMOUSEMOVE || (uMsg == WM_SETCURSOR && bInRect))
		{
			if (m_HideMode != HM_NONE && !m_bIsCheck)
			{
				pThis->SetTimer(TIME_EVENT_CHECK_MOUSE, 250, NULL);
				m_bIsCheck = TRUE;
				m_bHiding = FALSE;
				pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
				m_bIsSlide = TRUE;
			}
		}

		// 窗口还原时滑动显示
		if (uMsg == WM_COMMAND || uMsg == WM_SYSCOMMAND)
		{
			if (wParam == SC_RESTORE && m_HideMode != HM_NONE)
			{
				m_bHiding = FALSE;
				pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
				m_bIsSlide = TRUE;
			}
		}

		// 失去焦点时滑动隐藏
		if (uMsg == WM_NCACTIVATE && !wParam && m_HideMode != HM_NONE)
		{
			HWND hFocus = ::GetFocus();
			HWND hParent = ::GetParent(hFocus);

			// 最小化时不需要去滑动窗口
			if (hParent != pThis->m_hWnd && !IsIconic(pThis->m_hWnd))
			{
				m_bHiding = TRUE;
				pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
				m_bIsSlide = TRUE;
			}
		}

		bHandled = FALSE;
		return 1;
	}
private:
	BOOL DoSlideDialog()				// 滑动显示或隐藏窗口
	{
		TWindow* pThis = (TWindow*)this;
		if (m_HideMode == HM_NONE)
			return FALSE;
		// 当前窗口位置和大小
		CRect rectWindow;
		pThis->GetWindowRect(rectWindow);
		INT nHeight = rectWindow.Height();
		INT nWidth = rectWindow.Width();
		// 屏幕的高度和宽度
		INT nWidthScreen = GetSystemMetrics(SM_CXSCREEN);
		INT nHeightScreen = GetSystemMetrics(SM_CYSCREEN);
		// 默认边框尺寸
		INT nXEdge = 5;
		INT nYEdge = 5;

		// Win7下进行相关调整
		if (CEnv::IsOSHigherThanXP())
		{
			nXEdge += 4;
			nYEdge += 4;
		}

		// 任务栏位置和大小
		HWND hTaskBar = ::FindWindow(_T("Shell_TrayWnd"), NULL);
		CRect rectTaskBar;
		if (hTaskBar)
			::GetWindowRect(hTaskBar, &rectTaskBar);
		// 任务栏在屏幕的位置
		TASKBAR_POS taskPos = TASKBAR_UNKNOWN;
		if (rectTaskBar.left <= 0 && rectTaskBar.top <= 0 && rectTaskBar.right >= nWidthScreen)
			taskPos = TASKBAR_TOP;
		if (rectTaskBar.left <= 0 && rectTaskBar.bottom >= nHeightScreen && rectTaskBar.right >= nWidthScreen)
			taskPos = TASKBAR_BOTTOM;
		if (rectTaskBar.left <= 0 && rectTaskBar.top <= 0 && rectTaskBar.bottom >= nHeightScreen)
			taskPos = TASKBAR_LEFT;
		if (rectTaskBar.right >= nWidthScreen && rectTaskBar.top <= 0 && rectTaskBar.bottom >= nHeightScreen)
			taskPos = TASKBAR_RIGHT;
		BOOL bResult = TRUE;
		switch (m_HideMode)
		{
		case HM_TOP:
			{
				INT nOffset = (taskPos == TASKBAR_BOTTOM ? rectTaskBar.Height() : 0);
				INT nEdge = CEnv::IsOSHigherThanXP() ? -7 : -HIDEWINDOWS_SHOWHEGIHT;

				// 判断向上隐藏
				if (m_bHiding && rectWindow.bottom <= nYEdge)
					return FALSE;
				// 判断向下滑动显示
				if (!m_bHiding && rectWindow.top >= nEdge)
					return FALSE;
				// 设置移动步长
				INT nSteps = nHeight / SLIDE_STEPS;
				// 修改窗口位置
				if (!m_bHiding)
				{
					// 向下滑动显示
					rectWindow.OffsetRect(0, nSteps);
					if (rectWindow.top > nEdge)
					{
						rectWindow.OffsetRect(0, nEdge-rectWindow.top);
						bResult = FALSE;
					}
				}
				else
				{
					// 向上隐藏
					rectWindow.OffsetRect(0, -nSteps);
					if (rectWindow.bottom < nYEdge)
					{
						rectWindow.OffsetRect(0, nYEdge-rectWindow.bottom);
						bResult = FALSE;
					}
				}
			}
			break;
		case HM_LEFT:
			{
				INT nOffset = (taskPos == TASKBAR_RIGHT ? rectTaskBar.Width() : 0);
				INT nEdge = CEnv::IsOSHigherThanXP() ? -7 : -HIDEWINDOWS_SHOWHEGIHT;

				// 判断向左隐藏
				if (m_bHiding && rectWindow.right <= nXEdge)
					return FALSE;
				// 判断向右滑动显示
				if (!m_bHiding && rectWindow.left >= nEdge)
					return FALSE;
				// 设置移动步长
				INT nSteps = nWidth / SLIDE_STEPS;
				// 修改窗口位置
				if (!m_bHiding)
				{
					// 向右滑动显示
					rectWindow.OffsetRect(nSteps, 0);
					if (rectWindow.left > nEdge)
					{
						rectWindow.OffsetRect(nEdge-rectWindow.left, 0);
						bResult = FALSE;
					}
				}
				else
				{
					// 向左隐藏
					rectWindow.OffsetRect(-nSteps, 0);
					if (rectWindow.right < nXEdge)
					{
						rectWindow.OffsetRect(nXEdge-rectWindow.right, 0);
						bResult = FALSE;
					}
				}
			}
			break;

		case HM_BOTTOM:
			{
				INT nOffset = (taskPos == TASKBAR_TOP ? rectTaskBar.Height() : 0);
				INT nEdge = CEnv::IsOSHigherThanXP() ? 7 : HIDEWINDOWS_SHOWHEGIHT;

				// 判断向下隐藏
				if (m_bHiding && rectWindow.top >= nHeightScreen - nYEdge)
					return FALSE;
				// 判断向上滑动显示
				if (!m_bHiding && rectWindow.bottom <= nHeightScreen+nEdge)
					return FALSE;
				// 设置移动步长
				INT nSteps = nHeight / SLIDE_STEPS;
				// 修改窗口位置
				if (!m_bHiding)
				{
					// 向上滑动显示
					rectWindow.OffsetRect(0, -nSteps);
					if (rectWindow.bottom < nHeightScreen+nEdge)
					{
						rectWindow.OffsetRect(0, nHeightScreen+nEdge-rectWindow.bottom);
						bResult = FALSE;
					}
				}
				else
				{
					// 向下隐藏
					rectWindow.OffsetRect(0, nSteps);
					if (rectWindow.top > nHeightScreen - nYEdge)
					{
						rectWindow.OffsetRect(0, nHeightScreen-nYEdge-rectWindow.top);
						bResult = FALSE;
					}
				}
			}
			break;

		case HM_RIGHT:
			{
				INT nOffset = (taskPos == TASKBAR_LEFT ? rectTaskBar.Width() : 0);
				INT nEdge = CEnv::IsOSHigherThanXP() ? 7 : HIDEWINDOWS_SHOWHEGIHT;

				// 判断向右隐藏
				if (m_bHiding && rectWindow.left >= nWidthScreen - nXEdge)
					return FALSE;
				// 判断向左滑动显示
				if (!m_bHiding && rectWindow.right <= nWidthScreen+nEdge)
					return FALSE;
				// 设置移动步长
				INT nSteps = nWidth / SLIDE_STEPS;
				// 修改窗口位置
				if (!m_bHiding)
				{
					// 向左滑动显示
					rectWindow.OffsetRect(-nSteps, 0);
					if (rectWindow.right < nWidthScreen+nEdge)
					{
						rectWindow.OffsetRect(nWidthScreen+nEdge-rectWindow.right, 0);
						bResult = FALSE;
					}
				}
				else
				{
					// 向右隐藏
					rectWindow.OffsetRect(nSteps, 0);
					if (rectWindow.left > nWidthScreen - nXEdge)
					{
						rectWindow.OffsetRect(nWidthScreen-nXEdge-rectWindow.left, 0);
						bResult = FALSE;
					}
				}
			}
			break;

		default:
			return FALSE;
		}
		// 移动窗口
		pThis->MoveWindow(&rectWindow);
		return bResult;
	}
};

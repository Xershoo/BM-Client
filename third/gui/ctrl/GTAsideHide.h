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
	TIME_EVENT_CHECK_MOUSE	= 1000	// �������λ��
	,TIME_EVENT_FLASH_WND		// �������ض���
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
	HIDEMODE	m_HideMode;				// ����ģʽ
	HIDEMODE	m_LastMode;				// ����ǰ��ģʽ
	UINT		m_nHeightOrigin;		// ����ǰ�ĸ߶�
	BOOL		m_bHeightChanged;		// �Ƿ�ı��˳ߴ�
	BOOL		m_bIsCheck;				// �Ƿ��Ѿ��ڼ�����
	BOOL		m_bHiding;				// �Ƿ���������
	BOOL		m_bIsSlide;				// �Ƿ����ڻ���

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

		// ������λ��
		if (wParam == TIME_EVENT_CHECK_MOUSE)
		{
			// ��ǰ���λ��
			POINT curPos;
			::GetCursorPos(&curPos);
			// ��ǰ����λ��
			CRect rectWindow;
			pThis->GetWindowRect(rectWindow);
			rectWindow.InflateRect(EDGE_OFFSET, EDGE_OFFSET);
			// ����뿪�˼������
			if (!rectWindow.PtInRect(curPos))
			{
				// ȡ�����
				pThis->KillTimer(TIME_EVENT_CHECK_MOUSE);
				m_bIsCheck = FALSE;
				// �������ش���
				m_bHiding = TRUE;
				pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
				m_bIsSlide = TRUE;
			}
		}
		// ���ڻ�������
		else if (wParam == TIME_EVENT_FLASH_WND)
		{
			// BUG 0008015 ��� 2010-10-19
			// ������С����ʱ�򣬲�ִ���κλ�ı䴰��λ�õ����
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
						///TODO: ����ע�Ϳ�����Ҫʵ��
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
		// �ж��Ƿ���������������
		///TODO:
		/*if (!CSessionInfo::GetInst()->GetConfigManager()->GetSystemConfig().m_bAutoHide)
		{
			m_HideMode = HM_NONE;
			bHandled = FALSE;
			return 0;
		}*/
		// ��ǰ���λ��
		POINT curPos;
		::GetCursorPos(&curPos);
		// ��ǰ���ڸ߶ȺͿ��
		INT nHeight = lpRect->bottom - lpRect->top;
		INT nWidth = lpRect->right - lpRect->left;
		// ��Ļ�߶ȺͿ��
		INT nHeightScreen = GetSystemMetrics(SM_CYSCREEN);
		INT nWidthScreen = GetSystemMetrics(SM_CXSCREEN);
		// ������λ�úʹ�С
		HWND hTaskBar = ::FindWindow(_T("Shell_TrayWnd"), NULL);
		CRect rectTaskBar;
		if (hTaskBar)
			::GetWindowRect(hTaskBar, &rectTaskBar);
		// ����������Ļ��λ��
		TASKBAR_POS taskPos = TASKBAR_UNKNOWN;
		if (rectTaskBar.left <= 0 && rectTaskBar.top <= 0 && rectTaskBar.right >= nWidthScreen)
			taskPos = TASKBAR_TOP;
		if (rectTaskBar.left <= 0 && rectTaskBar.bottom >= nHeightScreen && rectTaskBar.right >= nWidthScreen)
			taskPos = TASKBAR_BOTTOM;
		if (rectTaskBar.left <= 0 && rectTaskBar.top <= 0 && rectTaskBar.bottom >= nHeightScreen)
			taskPos = TASKBAR_LEFT;
		if (rectTaskBar.right >= nWidthScreen && rectTaskBar.top <= 0 && rectTaskBar.bottom >= nHeightScreen)
			taskPos = TASKBAR_RIGHT;
		// �Ƿ���������
		if (curPos.y < EDGE_OFFSET)
		{
			lpRect->bottom = nHeight-HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = -HIDEWINDOWS_SHOWHEGIHT;
			m_HideMode = HM_TOP;

			// Win7�½�����ص���
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->bottom -= 4;
				lpRect->top -= 4;
			}
		}
		// �Ƿ���������
		else if (curPos.x < EDGE_OFFSET)
		{
			lpRect->right = nWidth-HIDEWINDOWS_SHOWHEGIHT;
			lpRect->left = -HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = -HIDEWINDOWS_SHOWHEGIHT;
			lpRect->bottom = nHeightScreen+HIDEWINDOWS_SHOWHEGIHT;
			// ���������������߶�
			if (taskPos == TASKBAR_BOTTOM)
				lpRect->bottom -= rectTaskBar.Height();
			if (taskPos == TASKBAR_TOP)
				lpRect->top += rectTaskBar.Height();
			// ����ԭʼ�߶�
			if (!m_bHeightChanged)
			{
				m_nHeightOrigin = nHeight;
				m_bHeightChanged = TRUE;
			}
			m_HideMode = HM_LEFT;

			// Win7�½�����ص���
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->left -= 4;
				lpRect->right -= 4;
				lpRect->top -= 4;
				lpRect->bottom += 4;
			}
		}
		// �Ƿ���������
		else if (curPos.x > nWidthScreen - EDGE_OFFSET)
		{
			lpRect->right = nWidthScreen+HIDEWINDOWS_SHOWHEGIHT;
			lpRect->left = nWidthScreen - nWidth+HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = -HIDEWINDOWS_SHOWHEGIHT;
			lpRect->bottom = nHeightScreen+HIDEWINDOWS_SHOWHEGIHT;
			// ���������������߶�
			if (taskPos == TASKBAR_BOTTOM)
				lpRect->bottom -= rectTaskBar.Height();
			if (taskPos == TASKBAR_TOP)
				lpRect->top += rectTaskBar.Height();
			// ����ԭʼ�߶�
			if (!m_bHeightChanged)
			{
				m_nHeightOrigin = nHeight;
				m_bHeightChanged = TRUE;
			}
			m_HideMode = HM_RIGHT;

			// Win7�½�����ص���
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->left += 4;
				lpRect->right += 4;
				lpRect->top -= 4;
				lpRect->bottom += 4;
			}
		}
		// �Ƿ���������
		else if (curPos.y > nHeightScreen - EDGE_OFFSET)
		{
			lpRect->bottom = nHeightScreen+HIDEWINDOWS_SHOWHEGIHT;
			lpRect->top = nHeightScreen - nHeight+HIDEWINDOWS_SHOWHEGIHT;
			m_HideMode = HM_BOTTOM;

			// Win7�½�����ص���
			if (CEnv::IsOSHigherThanXP())
			{
				lpRect->bottom += 4;
				lpRect->top += 4;
			}
		}
		// ������
		else
		{
			// ��ԭ���ڸ߶�
			if (m_bHeightChanged)
			{
				lpRect->bottom = lpRect->top + m_nHeightOrigin;
				m_bHeightChanged = FALSE;
			}
			// ȡ�������
			if (m_bIsCheck)
			{
				pThis->KillTimer(TIME_EVENT_CHECK_MOUSE);
				m_bIsCheck = FALSE;
			}
			m_HideMode = HM_NONE;
		}

		// �����Ϣ��ĵط����ᴦ������ΪHandled
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

		// �������غ���ʾ
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
		// �����봰�ڷ�Χ��������ʾ����
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

		// ���ڻ�ԭʱ������ʾ
		if (uMsg == WM_COMMAND || uMsg == WM_SYSCOMMAND)
		{
			if (wParam == SC_RESTORE && m_HideMode != HM_NONE)
			{
				m_bHiding = FALSE;
				pThis->SetTimer(TIME_EVENT_FLASH_WND, 10, NULL);
				m_bIsSlide = TRUE;
			}
		}

		// ʧȥ����ʱ��������
		if (uMsg == WM_NCACTIVATE && !wParam && m_HideMode != HM_NONE)
		{
			HWND hFocus = ::GetFocus();
			HWND hParent = ::GetParent(hFocus);

			// ��С��ʱ����Ҫȥ��������
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
	BOOL DoSlideDialog()				// ������ʾ�����ش���
	{
		TWindow* pThis = (TWindow*)this;
		if (m_HideMode == HM_NONE)
			return FALSE;
		// ��ǰ����λ�úʹ�С
		CRect rectWindow;
		pThis->GetWindowRect(rectWindow);
		INT nHeight = rectWindow.Height();
		INT nWidth = rectWindow.Width();
		// ��Ļ�ĸ߶ȺͿ��
		INT nWidthScreen = GetSystemMetrics(SM_CXSCREEN);
		INT nHeightScreen = GetSystemMetrics(SM_CYSCREEN);
		// Ĭ�ϱ߿�ߴ�
		INT nXEdge = 5;
		INT nYEdge = 5;

		// Win7�½�����ص���
		if (CEnv::IsOSHigherThanXP())
		{
			nXEdge += 4;
			nYEdge += 4;
		}

		// ������λ�úʹ�С
		HWND hTaskBar = ::FindWindow(_T("Shell_TrayWnd"), NULL);
		CRect rectTaskBar;
		if (hTaskBar)
			::GetWindowRect(hTaskBar, &rectTaskBar);
		// ����������Ļ��λ��
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

				// �ж���������
				if (m_bHiding && rectWindow.bottom <= nYEdge)
					return FALSE;
				// �ж����»�����ʾ
				if (!m_bHiding && rectWindow.top >= nEdge)
					return FALSE;
				// �����ƶ�����
				INT nSteps = nHeight / SLIDE_STEPS;
				// �޸Ĵ���λ��
				if (!m_bHiding)
				{
					// ���»�����ʾ
					rectWindow.OffsetRect(0, nSteps);
					if (rectWindow.top > nEdge)
					{
						rectWindow.OffsetRect(0, nEdge-rectWindow.top);
						bResult = FALSE;
					}
				}
				else
				{
					// ��������
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

				// �ж���������
				if (m_bHiding && rectWindow.right <= nXEdge)
					return FALSE;
				// �ж����һ�����ʾ
				if (!m_bHiding && rectWindow.left >= nEdge)
					return FALSE;
				// �����ƶ�����
				INT nSteps = nWidth / SLIDE_STEPS;
				// �޸Ĵ���λ��
				if (!m_bHiding)
				{
					// ���һ�����ʾ
					rectWindow.OffsetRect(nSteps, 0);
					if (rectWindow.left > nEdge)
					{
						rectWindow.OffsetRect(nEdge-rectWindow.left, 0);
						bResult = FALSE;
					}
				}
				else
				{
					// ��������
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

				// �ж���������
				if (m_bHiding && rectWindow.top >= nHeightScreen - nYEdge)
					return FALSE;
				// �ж����ϻ�����ʾ
				if (!m_bHiding && rectWindow.bottom <= nHeightScreen+nEdge)
					return FALSE;
				// �����ƶ�����
				INT nSteps = nHeight / SLIDE_STEPS;
				// �޸Ĵ���λ��
				if (!m_bHiding)
				{
					// ���ϻ�����ʾ
					rectWindow.OffsetRect(0, -nSteps);
					if (rectWindow.bottom < nHeightScreen+nEdge)
					{
						rectWindow.OffsetRect(0, nHeightScreen+nEdge-rectWindow.bottom);
						bResult = FALSE;
					}
				}
				else
				{
					// ��������
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

				// �ж���������
				if (m_bHiding && rectWindow.left >= nWidthScreen - nXEdge)
					return FALSE;
				// �ж����󻬶���ʾ
				if (!m_bHiding && rectWindow.right <= nWidthScreen+nEdge)
					return FALSE;
				// �����ƶ�����
				INT nSteps = nWidth / SLIDE_STEPS;
				// �޸Ĵ���λ��
				if (!m_bHiding)
				{
					// ���󻬶���ʾ
					rectWindow.OffsetRect(-nSteps, 0);
					if (rectWindow.right < nWidthScreen+nEdge)
					{
						rectWindow.OffsetRect(nWidthScreen+nEdge-rectWindow.right, 0);
						bResult = FALSE;
					}
				}
				else
				{
					// ��������
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
		// �ƶ�����
		pThis->MoveWindow(&rectWindow);
		return bResult;
	}
};

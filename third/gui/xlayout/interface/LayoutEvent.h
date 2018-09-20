#pragma once

#include "ILayout.h"

NAMESPACE_BEGIN(xlayout)

struct ILayout;
struct ILayoutObj;

class CLayoutEvent
{
public:
    CLayoutEvent( ILayout* pLayout=NULL, HWND hWnd=NULL );
    ~CLayoutEvent();
    inline void SetLayout( ILayout* pLayout ) { m_pLayout = pLayout; }
    inline void SetHwnd( HWND hWnd )          { m_hWnd = hWnd;       }     

public:
    void OnMouseMove( UINT nFlags, POINT point );
    void OnLButtonDown( UINT nFlags, POINT point );
    void OnLButtonUp( UINT nFlags, POINT point );

private:
    bool SetSpltCursor( ILayoutObj* pLObj );
    void DrawDragRect();
    void UndrawDragRect();

private:
    HWND         m_hWnd;
    ILayout*     m_pLayout;
    ILayoutObj*  m_pLObj;
    RECT         m_rcOld;
    POINT        m_ptOld;
    int          m_nOffset;
    bool         m_bCapture;
};

inline CLayoutEvent::CLayoutEvent( ILayout* pLayout, HWND hWnd )
: m_pLayout( pLayout )
, m_hWnd( hWnd )
, m_bCapture( false )
{

}

inline CLayoutEvent::~CLayoutEvent()
{

}

inline bool CLayoutEvent::SetSpltCursor( ILayoutObj* pLObj )
{
    if( pLObj->GetData() != 0 ) { // 相当于禁止分隔条！
        return false;
    }
    if( pLObj->GetType() == TYPE_SPLITTER_H )
    {
        ::SetCursor( ::LoadCursor( NULL, IDC_SIZEWE ) ) ;
        return true;
    }
    else if( pLObj->GetType() == TYPE_SPLITTER_V )
    {
        ::SetCursor( ::LoadCursor( NULL, IDC_SIZENS ) ) ;
        return true;
    }
    return false;
}

inline void CLayoutEvent::OnMouseMove( UINT nFlags, POINT point )
{
    if( m_bCapture )
    {	
        if( m_pLObj->GetType() == TYPE_SPLITTER_H ) {
            m_nOffset = m_pLObj->PreSeek( DIR_NULL, (short)point.x-(short)m_ptOld.x );
        } else {
            m_nOffset = m_pLObj->PreSeek( DIR_NULL, (short)point.y-(short)m_ptOld.y );
        }
        if( m_nOffset == 0 ) {
            return;
        }

        // 画拖动条
        this->DrawDragRect();
    }
    else
    {
        ILayoutObj* pLObj = m_pLayout->GetLayoutObj( point );
        if( pLObj != NULL )
        {
            this->SetSpltCursor( pLObj );
        }
    }
}

inline void CLayoutEvent::OnLButtonDown( UINT nFlags, POINT point )
{
    ILayoutObj* pLObj = m_pLayout->GetLayoutObj( point );
    if( pLObj == NULL ) {
        return;
    }
    if( !this->SetSpltCursor( pLObj ) ) {
        return;
    }

    // 捕捉
    if( m_hWnd != NULL )
    {
        m_pLObj   = pLObj;
        m_ptOld   = point;
        m_nOffset = 0;
        ::SetRectEmpty( &m_rcOld );
        ::SetCapture( m_hWnd );
        m_bCapture = true;
    }
}

inline void CLayoutEvent::OnLButtonUp( UINT nFlags, POINT point )
{
    if( m_bCapture )
    {
        // 释放鼠标
        ::ReleaseCapture();
        m_bCapture = false;

        // 抹掉分割条
        this->UndrawDragRect();

        // 拖动
        m_pLObj->Seek( xlayout::DIR_NULL, m_nOffset );

        // 重绘制
#if 0
        // 注意坐标转换
        RECT rcWnd;
        ::GetWindowRect( m_hWnd, &rcWnd );
        ::OffsetRect( &m_rcOld, rcWnd.left, rcWnd.top );
        ::ScreenToClient( m_hWnd, ( LPPOINT )&m_rcOld   );
        ::ScreenToClient( m_hWnd, ( LPPOINT )&m_rcOld+1 );
        ::InvalidateRect( m_hWnd, &m_rcOld, TRUE );
#else
        ::InvalidateRect( m_hWnd, NULL, TRUE );
#endif
    }
}

inline void CLayoutEvent::DrawDragRect()
{
    HDC hdc = ::GetWindowDC( m_hWnd );
    CDC dc;
    dc.Attach( hdc );

    RECT rcDrag;
    SIZE szDrag;
    ::CopyRect( &rcDrag, m_pLObj->GetBounds() );
    if( m_pLObj->GetType() == TYPE_SPLITTER_H )
    {
        rcDrag.right=rcDrag.left+4;
        ::OffsetRect( &rcDrag, m_nOffset, 0 );
        szDrag.cx = 2;
        szDrag.cy = 0;
    }
    else
    {
        rcDrag.bottom=rcDrag.top+4;
        ::OffsetRect( &rcDrag, 0, m_nOffset );
        szDrag.cx = 0;
        szDrag.cy = 2;
    }

    // 注意坐标转换
    RECT rcWnd;
    ::GetWindowRect( m_hWnd, &rcWnd );
    ::ClientToScreen( m_hWnd, ( LPPOINT )&rcDrag   );
    ::ClientToScreen( m_hWnd, ( LPPOINT )&rcDrag+1 );
    ::OffsetRect( &rcDrag, -rcWnd.left, -rcWnd.top );

    // draw drag
    if( ::IsRectEmpty( &m_rcOld ) ) {
        dc.DrawDragRect( &rcDrag, szDrag, NULL, szDrag );
    } else {
        dc.DrawDragRect( &rcDrag, szDrag, &m_rcOld, szDrag );
    }

    // 保存位置
    ::CopyRect( &m_rcOld, &rcDrag );

    dc.Detach();
    ::ReleaseDC( m_hWnd, hdc );
}

inline void CLayoutEvent::UndrawDragRect()
{
    HDC hdc = ::GetWindowDC( m_hWnd );
    CDC dc;
    dc.Attach( hdc );

    SIZE szDrag;
    if( m_pLObj->GetType() == TYPE_SPLITTER_H )
    {
        szDrag.cx = 2;
        szDrag.cy = 0;
    }
    else
    {
        szDrag.cx = 0;
        szDrag.cy = 2;
    }

    // draw drag
    dc.DrawDragRect( CRect(0,0,0,0), szDrag, &m_rcOld, szDrag );

    dc.Detach();
    ::ReleaseDC( m_hWnd, hdc );
}

NAMESPACE_END(xlayout)

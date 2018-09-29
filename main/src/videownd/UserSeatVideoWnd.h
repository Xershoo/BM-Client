#ifndef USER_VIDEO_SEAT_WND
#define USER_VIDEO_SEAT_WND

#include <QtWidgets/QWidget>
#include "RtmpPublisher.h"
#include "OpenGLVideoWidget.h"

/*
#include "VideoShowBase.h"

class UserSeatVideoWnd : public QWidget,
	public CRTMPStreamShow,
	public VideoShowBase
*/

class UserSeatVideoWnd : public OpenGLVideoWidget,
	public CRTMPStreamShow
{
	Q_OBJECT
public:
	UserSeatVideoWnd(QWidget * parent);
	virtual ~UserSeatVideoWnd();

public:
	virtual bool setSeatIndex(int nIndex);

public:
	inline int  getSeatIndex();

protected slots:
	void onUpdatePublishSeat(int seatIndex);	

protected:
	virtual void showRtmpVideoBuf(const RtmpVideoBuf& videoData);

protected:
	/*
	virtual void paintEvent(QPaintEvent * event);
	*/
	virtual void timerEvent(QTimerEvent * event);

protected:	
	int                 m_seat;

//	QMutex              m_mutexVideoBuf;
//	RtmpVideoBuf        m_showVideoBuf;
//	int					m_refreshTimerId;
//	int					m_paintTimerId;
};

int UserSeatVideoWnd::getSeatIndex()
{
	return m_seat;
}

#endif
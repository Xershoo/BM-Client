//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�StudentVideoWnd.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.14
//	˵����ѧ��RTMP�����Ŵ��ڷ�װ���ඨ��ͷ�ļ�
//  ʹ��˵����
//      StudentVideoWnd * videoWnd = new StudentVideoWnd(parent);
//      videoWnd->resize(400,300);
//      videoWnd->show();
//      videoWnd->setUrl(url);
//      videoWnd->start();
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef STUDENT_VIDEO_WND
#define STUDENT_VIDEO_WND

#include <QtWidgets/QWidget>
#include <QtCore\QMutex>
#include <QtWidgets\QLabel>
#include <QtWidgets\QPushButton>
#include "UserVideoWnd.h"
#include "VideoShowBase.h"
#include "clickablelabel.h"

class StudentVideoWnd : public UserVideoWnd,
    public VideoShowBase
{
	enum
	{
		BTN_SPEAK_NORAML = 0,
		BTN_SPEAK_HOVER,
		BTN_SPEAK_PRESS
	};

public:
    enum
    {
        USER_HAND_NO = 0,
        USER_HAND_ASK,
        USER_HAND_SPK
    };

    Q_OBJECT
public:
    StudentVideoWnd(QWidget * parent);

	void showRtmpVideoBuf(const RtmpVideoBuf& videoData);
		
	void setUserSpeak(bool speak);

	inline void setUserName(QString strName)
	{
		m_userName = strName;
	}

	inline void setWndIndex(int nIndex)
	{
		m_videoIndex = nIndex;
	};

	inline void setShowHand(int showHand)
	{
		m_showHand = showHand;
	};

protected:
    void paintEvent(QPaintEvent * event);
	virtual void timerEvent(QTimerEvent * event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

protected slots: 
	void onSpeakBtnClicked();
	void onSpeakLabClicked();

protected:
	void paintToolbar(QPainter& painter,QRect& rectWnd);
	void setupUI();

    void paintHandStatu(QPainter& painter, QRect& rectWnd);
signals:
	void stuSpeakBtnClicked(__int64 userId,bool speak);

protected:
    QMutex              m_mutexVideoBuf;
    RtmpVideoBuf        m_showVideoBuf;

	int					m_refreshTimerId;
	int					m_paintTimerId;

protected:
	int					m_videoIndex;
	QString				m_userName;
	int				    m_showHand;
	
//	int					m_btnSpeakState;

	QPushButton*		m_btnStuSpeak;
	ClickableLabel*		m_labStuSpeak;
};

#endif
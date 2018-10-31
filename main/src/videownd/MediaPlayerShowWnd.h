//**********************************************************************
//	Copyright ��c�� 2016,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�MediaPlayerShowWnd.h
//	�汾�ţ�1.0
//	���ߣ� 
//	ʱ�䣺 
//	˵��������Ƶ�μ����Ŵ��ڷ�װ���ඨ��ͷ�ļ�
//  ʹ��˵����
//	�޸ļ�¼��
//**********************************************************************

#pragma once

#include <QtWidgets/QWidget>
#include "VideoShowBase.h"
#include "FilePlayer.h"
#include <QImage>
class QWhiteBoardView;

class MediaPlayerShowWnd : public CMediaFilePlayer
{
    Q_OBJECT
public:
    MediaPlayerShowWnd(QWidget * parent = 0);
    virtual ~MediaPlayerShowWnd();

	virtual bool play(const string& file,bool onlyOpen = false);
	virtual unsigned int getCurPlayTime(bool video = false);
	bool seekEx(unsigned int pos);
public:
	inline void    setUserId(__int64 userId);
	inline __int64 getUserId();

	inline void    setCourseId(__int64 cid);
	inline __int64 getCourseId();

	inline void    setWbCtrl(int nCtrl);
	inline int     getWbCtrl();

	inline QWhiteBoardView* getWhiteBoardView();

	void createWhiteboardView();
protected:
	int calcFileId();
	void setWhiteboardViewBack();

	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject * obj, QEvent * event);
protected:    
    virtual void ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);

protected:
	bool	m_isVideo;

	__int64     m_userId;
	__int64     m_courseId;
	int         m_wbCtrl;

	QWhiteBoardView * m_viewWhiteboard;
};

void MediaPlayerShowWnd::setUserId(__int64 userId)
{
	m_userId = userId;
}

__int64 MediaPlayerShowWnd::getUserId()
{
	return m_userId;
}

void MediaPlayerShowWnd::setCourseId(__int64 cid)
{
	m_courseId = cid;
}

__int64 MediaPlayerShowWnd::getCourseId()
{
	return m_courseId;
}

void MediaPlayerShowWnd::setWbCtrl(int nCtrl)
{
	m_wbCtrl = nCtrl;
}

int MediaPlayerShowWnd::getWbCtrl()
{
	return m_wbCtrl;
}

QWhiteBoardView* MediaPlayerShowWnd::getWhiteBoardView()
{
	return m_viewWhiteboard;
}
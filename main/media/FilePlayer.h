//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�RtmpPlayer.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2016.01.27
//	˵������ý���ļ����ŷ�װ���ඨ��ͷ�ļ�
//  �ӿڣ�
//
//	�޸ļ�¼��
//
//**********************************************************************

#ifndef MEDIA_FILE_PLAYER_H
#define MEDIA_FILE_PLAYER_H

#include <string>
#include <QWidget>
#include "OpenGLVideoWidget.h"
#include "RtmpStream.h"
#include "../media/player/MediaPlayer/include/MediaPlayer.h"

using namespace std;

class CMediaFilePlayer : public OpenGLVideoWidget,        
	public IMediaPlayerEvent
{
    Q_OBJECT
public:
    CMediaFilePlayer(QWidget* parent);
    virtual ~CMediaFilePlayer();

	enum
	{
		STOP = 0,
		PLAY,
		PAUSE,
		SEEK
	};
public:
	virtual bool play(const string& file,bool onlyOpen = false);    
	virtual bool pause(bool isPause);
	virtual bool stop(void);
	virtual bool seek(unsigned int pos,bool force = false);
	
	virtual unsigned int getTotalPlayTime(void);
	virtual unsigned int getCurPlayTime(bool video = false);
	
protected:		//δʹ�õĽӿ�
	static bool switchPlayFile(const string& curPlayFile);
	virtual bool playSwitch(const string& file);
    void pauseShow(bool bShow);

public:
	inline unsigned int getState()
	{
		return m_state;
	};

	inline string& getFile()
	{
		return m_fileName;
	};

protected:
    virtual void ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH); 
	virtual void ControlPaint(){};
    
protected:
	virtual void timerEvent(QTimerEvent *tEvent);

signals:
	void playPosChange(unsigned int state,string& file);

protected:
	string m_fileName;

private:
	unsigned int  m_state;
	unsigned int  m_totalPlayTime;
	int m_idTimePlay;

    bool	m_bShow;
};

#endif
//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpPlayer.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2016.01.27
//	说明：多媒体文件播放封装的类定义头文件
//  接口：
//
//	修改记录：
//
//**********************************************************************

#ifndef MEDIA_FILE_PLAYER_H
#define MEDIA_FILE_PLAYER_H

#include <string>
#include <QWidget>
#include "RtmpStream.h"
#include "../media/player/MediaPlayer/include/MediaPlayer.h"

using namespace std;

class CMediaFilePlayer : public QWidget,        
	public IMediaPlayerEvent
{
    Q_OBJECT
public:
    CMediaFilePlayer(QWidget* parent);
    virtual ~CMediaFilePlayer();

	static bool switchPlayFile(const string& curPlayFile);

	enum
	{
		STOP = 0,
		PLAY,
		PAUSE,
	};
public:
	virtual bool play(const string& file,bool onlyOpen = false);
    virtual bool playSwitch(const string& file);
	virtual bool pause(bool isPause);
	virtual bool stop(void);
	virtual bool seek(unsigned int pos);

	virtual unsigned int getTotalPlayTime(void);
	virtual unsigned int getCurPlayTime(void);
	
    void Show(bool bShow);
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
    void showVideoData(const RtmpVideoBuf& videoData);
	void playPosChange(unsigned int state);

private:
	string m_fileName;
	unsigned int  m_state;
	unsigned int  m_totalPlayTime;
	int m_idTimePlay;

    bool	m_bShow;
};

#endif
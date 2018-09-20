//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpPlayer.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.24
//	说明：rtmp流播放封装的类定义头文件
//  接口：
//      1.start打开和播放rtmp流
//      2.change修改播放参数
//      3.stop停止播放
//      4.recvVideoData信号表示收到视频数据，可以进行显示
//      5.RtmpVideoBuf中videoIndex表示是第几路的视频，videoWidth表示视频宽度，videoHeight表示视频高度，videoBuffer表示视频数据，buffSize表示视频数据大小
//      6.在start和change前面先调用设置接口进行参数设定
//	修改记录：
//      1.2015.11.27 重新修改接口
//**********************************************************************

#ifndef RTMP_PLAYER_H
#define RTMP_PLAYER_H

#include <string>
#include <QtCore/QObject>
#include "RtmpStream.h"

using namespace std;

class CRTMPPlayer : public CRTMPStream	
{   
public:
    CRTMPPlayer();
    virtual ~CRTMPPlayer();

public:
    inline void setUserId(__int64 nUserId);
    inline __int64 getUserId();

    inline void setMainVideo(int nMainVideo);
    inline int getMainVideo();

    inline void setPlayAudio(bool bAudio);
    inline bool getPlayAudio();

    inline int  getAddrNum();
    inline void setStudent(bool bStudent);

    inline void setVideoIndex(int videoIndex);
public:
    virtual bool start(void);
    virtual bool change(void);
    virtual void stop(void);

protected:
    void show(int ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);
    virtual void ControlPaint(){};

protected:
    void initYUVTable();
    void tranYUVData(unsigned char** rgbBuf,unsigned int& rgbSize,unsigned char *yuv420,int nWidth,int nHeight);

private:
    
    int         m_playType;
    PlayAddress m_playAddr[4];
    int         m_addrNum;
    int         m_mainVideo;
    __int64     m_userId;
    bool        m_isStudent;
    bool        m_isAudio;
    int         m_videoIndex;

    CRtmpStreamCallback* m_rtmpCallback;
private:

    long int		    m_crvTable[256];
    long int		    m_cbuTable[256];
    long int		    m_cguTable[256];
    long int		    m_cgvTable[256];
    long int		    m_tab76309[256];
    unsigned char	    m_clp[1024];   //for clip in CCIR601
};


//////////////////////////////////////////////////////////////////////////
//参数设置接口
void CRTMPPlayer::setUserId(__int64 nUserId)
{
    m_userId = nUserId;
}

inline __int64 CRTMPPlayer::getUserId()
{
    return m_userId;
}

inline void CRTMPPlayer::setMainVideo(int nMainVideo)
{
    m_mainVideo = nMainVideo;
}

inline int CRTMPPlayer::getMainVideo()
{
    return m_mainVideo;
}

inline void CRTMPPlayer::setPlayAudio(bool bAudio)
{
    m_isAudio = bAudio;
}

inline bool CRTMPPlayer::getPlayAudio()
{
    return bool(m_playType&AUDIOTYPE);
}

inline int CRTMPPlayer::getAddrNum()
{
    return m_addrNum;
}

inline void CRTMPPlayer::setStudent(bool bStudent)
{
    m_isStudent = bStudent;
}

inline void CRTMPPlayer::setVideoIndex(int videoIndex)
{
    m_videoIndex = videoIndex;
}
#endif
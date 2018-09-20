//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�RtmpPlayer.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.24
//	˵����rtmp�����ŷ�װ���ඨ��ͷ�ļ�
//  �ӿڣ�
//      1.start�򿪺Ͳ���rtmp��
//      2.change�޸Ĳ��Ų���
//      3.stopֹͣ����
//      4.recvVideoData�źű�ʾ�յ���Ƶ���ݣ����Խ�����ʾ
//      5.RtmpVideoBuf��videoIndex��ʾ�ǵڼ�·����Ƶ��videoWidth��ʾ��Ƶ��ȣ�videoHeight��ʾ��Ƶ�߶ȣ�videoBuffer��ʾ��Ƶ���ݣ�buffSize��ʾ��Ƶ���ݴ�С
//      6.��start��changeǰ���ȵ������ýӿڽ��в����趨
//	�޸ļ�¼��
//      1.2015.11.27 �����޸Ľӿ�
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
//�������ýӿ�
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
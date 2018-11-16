//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpStream.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.24
//	说明：rtmp流发布和播放封装基类定义头文件
//  接口：
//	修改记录：
//**********************************************************************

#ifndef RTMP_STREAM_H
#define RTMP_STREAM_H

#include <string>
#include <map>
#include "../media/player/MediaPlayer/include/MediaPlayer.h"

using namespace std;
#include <QMutex>

struct RtmpVideoBuf
{
public:
    RtmpVideoBuf()
    {
        videoIndex = 0;
        videoWidth = 0;
        videoHeight = 0;
        isYUVData =  false;
		delSelf = false;
		isDraw = false;

        videoBuff = NULL;
        buffsize = 0;
    };

    RtmpVideoBuf(int nVideoIndex,unsigned char* pBuf,unsigned int nSize,int nVideoW,int nVideoH,bool yuv = true)
    {
        videoIndex = nVideoIndex;
        videoWidth = nVideoW;
        videoHeight = nVideoH;
        isYUVData = yuv;
		delSelf = false;
		isDraw = false;
		
		buffsize = nSize;
        videoBuff = pBuf;
		/*
        if( NULL == pBuf || 0 >= nSize)
        {
            buffsize = 0;
            videoBuff = NULL;
        }
        else
        {
			buffsize = nSize;
			videoBuff = new unsigned char[nSize];

            memcpy(videoBuff,pBuf,nSize);
        }*/
    };

    RtmpVideoBuf(const RtmpVideoBuf& a)
    {
        videoIndex = a.videoIndex;
        videoWidth = a.videoWidth;
        videoHeight = a.videoHeight;
        isYUVData = a.isYUVData;
		delSelf = false;
		isDraw = false;
		buffsize = a.buffsize;
		videoBuff = a.videoBuff;

		/*
        if( NULL == a.videoBuff || 0 >= a.buffsize)
        {
            buffsize = 0;
            videoBuff = NULL;
        }
        else
        {
            buffsize = a.buffsize;
            videoBuff = new unsigned char[a.buffsize];

            memcpy(videoBuff,a.videoBuff,a.buffsize);
        }*/
    };

	void copy(const RtmpVideoBuf& a)
	{
		videoIndex = a.videoIndex;
		videoWidth = a.videoWidth;
		videoHeight = a.videoHeight;
		isYUVData = a.isYUVData;
		delSelf = true;
		isDraw = false;

		if(buffsize != a.buffsize)
		{
			if(videoBuff)
			{
				delete []videoBuff;
				videoBuff = NULL;
				buffsize = 0;
			}

			if(a.buffsize > 0)
			{
				videoBuff = new unsigned char[a.buffsize];
			}
		}

		buffsize = a.buffsize;

		if( NULL != a.videoBuff && a.buffsize > 0)
		{		
			memcpy(videoBuff,a.videoBuff,a.buffsize);
		}

		return;
	};

	void zeroBuffer()
	{
		videoWidth = 0;
		videoHeight = 0;
		buffsize = 0;
		delSelf = false;
		isDraw = true;

		if (NULL != videoBuff && delSelf)
		{
			delete []videoBuff;
			videoBuff = NULL;
		}
	}

    ~RtmpVideoBuf()
    {
        if(NULL != videoBuff && delSelf)
        {
            delete []videoBuff;
            videoBuff = NULL;
        }
    };
public:
    int         videoIndex;
    int         videoWidth;
    int         videoHeight;
    bool        isYUVData;
	bool		delSelf;
	bool		isDraw;

    unsigned int    buffsize;
    unsigned char*  videoBuff;
};

class CRTMPStreamShow;
class CRTMPStream
{
	typedef map<int,CRTMPStreamShow*>  RtmpStreamShowMap;
public:
    CRTMPStream();

public:
    virtual bool start(void) = 0;
    virtual bool change(void) = 0;
    virtual void stop(void) = 0;
    
	virtual void show(const RtmpVideoBuf& videoBuf);
    virtual void show(int nIndex,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);
public:
    inline bool isStart();    
    inline void setUrl(const char* url);
    inline string getUrl();

public:
	int  addShow(CRTMPStreamShow* rtmpShow);
	void delShow(int index);
	bool hasShow();

protected:
    string  m_rtmpUrl;
    bool    m_isStart;
    int     m_baseIndex;

    QMutex       m_mutexShow;
	RtmpStreamShowMap  m_mapShow;
};

//////////////////////////////////////////////////////////////////////////
inline void CRTMPStream::setUrl(const char* url)
{
    if(NULL == url || NULL == url[0])
    {
        m_rtmpUrl="";
        return;
    }

#ifndef MEDIA_NO_UDP
    m_rtmpUrl = url;
#else
	// 去掉UDP分离 xiewb 2016.4.24	
    char *pszUdp = NULL;
	char szUrl[1024] = { 0 };	
    strcpy_s(szUrl,url);
    
	pszUdp = strstr(szUrl,"|@|");
    if(pszUdp)
    {
        *pszUdp = NULL;
    }

    m_rtmpUrl = szUrl;
#endif
}

inline string CRTMPStream::getUrl()
{
    return m_rtmpUrl;
}

inline bool CRTMPStream::isStart()
{
    return m_isStart;
}


class CRTMPStreamShow
{
public:
	CRTMPStreamShow();
	virtual ~CRTMPStreamShow();

	virtual void setRtmpStream(CRTMPStream* rtmpStream);
	virtual void showRtmpVideoBuf(const RtmpVideoBuf& videoData) = 0;

protected:
	CRTMPStream*   m_rtmpStream;
	int			   m_index;
};

class CRtmpStreamCallback : public IMediaPlayerEvent
{
public:
    CRtmpStreamCallback();
    virtual ~CRtmpStreamCallback();

    inline void setRtmpStream(CRTMPStream* rtmpStream)
    {
        m_rtmpStream = rtmpStream;
    };
    
    inline CRTMPStream* getRtmpSteam()
    {
        return m_rtmpStream;
    };

	inline void setShowVideo(bool showVideo)
	{
		m_showVideo = showVideo;
	};
public:
    virtual void ControlPaint() {};
    virtual void ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);
    
protected:
    CRTMPStream * m_rtmpStream;
	bool		  m_showVideo;
};
#endif
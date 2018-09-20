//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpPublisher.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.24
//	说明：rtmp流发布封装的类定义头文件
//  接口：
//	修改记录：
//      2015.11.27   调整接口
//**********************************************************************

#ifndef RTMP_PUBLISHER_H
#define RTMP_PUBLISHER_H

#include <string>
#include <QtCore/QObject>
#include <QtGUI/qwindowdefs_win.h>
#include "../media/publish/libPublish/include/PublishInterface.h"
#include "varname.h"
#include "RtmpStream.h"

using namespace std;

class CRTMPPublisher :public CRTMPStream
{
public:
    CRTMPPublisher();
    virtual ~CRTMPPublisher();

public:
    // 开始发布RTMP流
    bool start(void);

    // 改变发布的RTMP流的参数
    bool change(void);

    // 停止发布RTMP流
    void stop(void);

    // 收到视频数据处理函数
    void capVideoData(unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);

public:
    // 设置视频流的流畅度
    inline void setMediaLevel(MediaLevel ml);

    // 设置RTMP流的发布角色
    inline void setMediaRole(MediaRole mr);

    // 设置视频流的画面宽高比例
    inline void setShowScale(SHOWSCALE sc);

    // 设置视频流的主从机会信息
    inline void setMasterSlave(MasterSlave ms);

    // 设置美颜特效(视频流)
    inline void setAVEFilter(AVE_FILTER_TYPE aveFilter);

    // 设置美颜等级(视频流)
    inline void setAVESubLevel(int aveSubLevel);

    // 设置RTMP流的源类型:SOURCECAMERA,SOURCESCREEN,SOURCEDEVAUDIO,SOURCEDEMUSIC
    inline void setSourceType(int sourceType);

    // 设置麦克风的ID：0 ~ N
    inline void setSelectMicId(int micId);

    // 设置摄像头的ID：0 ~ N
    inline void setSelectCameraId(int cameraId);

    // 设置是否发布流(预览时，不需要发布视频流）
    inline void setPublish(bool isPublish);

    // 设置视频是否焦点框
    inline void setDrawRect(bool isDrawRect);

    // 设置用户参数
    inline void setUserData(LPVOID userData);
public:
    // 获取视频流的流畅度
    inline MediaLevel getMediaLevel();

    // 获取RTMP流的发布角色
    inline MediaRole getMediaRole();

    // 获取视频流的画面宽高比例
    inline SHOWSCALE getShowScale();

    // 获取视频流的主从机会信息
    inline MasterSlave getMasterSlave();

    // 获取美颜特效(视频流)
    inline AVE_FILTER_TYPE getAVEFilter();

    // 获取美颜等级(视频流)
    inline int getAVESubLevel();

    // 获取RTMP流的源类型:SOURCECAMERA,SOURCESCREEN,SOURCEDEVAUDIO,SOURCEDEMUSIC
    inline int getSourceType();

    // 获取麦克风的ID：0 ~ N
    inline int getSelectMicId();

    // 获取摄像头的ID：0 ~ N
    inline int getSelectCameraId();

    // 获取是否发布流(预览时，不需要发布视频流）
    inline bool getPublish();

    // 获取视频是否焦点况
    inline bool getDrawRect();

    // 获取用户参数
    inline LPVOID getUserData();

private:
    PublishParam    m_paramPublish;    
    int             m_typePublish;

    LPVOID          m_userData;
};

//////////////////////////////////////////////////////////////////////////
//参数设置接口
inline void CRTMPPublisher::setMediaLevel(MediaLevel ml)
{
    m_paramPublish.ml = ml;
}

inline void CRTMPPublisher::setMediaRole(MediaRole mr)
{
    m_paramPublish.mr = mr;
}

inline void CRTMPPublisher::setShowScale(SHOWSCALE sc)
{
    m_paramPublish.scale = sc;
}

inline void CRTMPPublisher::setMasterSlave(MasterSlave ms)
{
    m_paramPublish.ms = ms;
}

inline void CRTMPPublisher::setAVEFilter(AVE_FILTER_TYPE aveFilter)
{
    m_paramPublish.VU[0].nVEnMainID = (int)aveFilter;
}

inline void CRTMPPublisher::setAVESubLevel(int aveSubLevel)
{
    m_paramPublish.VU[0].nVEnSubID = aveSubLevel;
}

inline void CRTMPPublisher::setSourceType(int sourceType)
{
    m_typePublish = sourceType;
}

inline void CRTMPPublisher::setSelectMicId(int micId)
{
    m_paramPublish.nSelectMicID = micId;
}

inline void CRTMPPublisher::setSelectCameraId(int cameraId)
{
    m_paramPublish.VU[0].nSelectCameraID = cameraId;
}

inline void CRTMPPublisher::setPublish(bool isPublish)
{
    m_paramPublish.bIsPublish = isPublish;
}

inline void CRTMPPublisher::setDrawRect(bool isDrawRect)
{
    m_paramPublish.bDrawLocRect = isDrawRect;
}

inline void CRTMPPublisher::setUserData(LPVOID userData)
{
    m_userData = userData;
}

inline LPVOID CRTMPPublisher::getUserData()
{
    return m_userData;
}

inline MediaLevel CRTMPPublisher::getMediaLevel()
{
    return m_paramPublish.ml;
}

inline MediaRole CRTMPPublisher::getMediaRole()
{
    return m_paramPublish.mr;
}

inline SHOWSCALE CRTMPPublisher::getShowScale()
{
    return m_paramPublish.scale;
}

inline MasterSlave CRTMPPublisher::getMasterSlave()
{
    return m_paramPublish.ms;
}

inline AVE_FILTER_TYPE CRTMPPublisher::getAVEFilter()
{
    return (AVE_FILTER_TYPE)m_paramPublish.VU[0].nVEnMainID;
}

inline int CRTMPPublisher::getAVESubLevel()
{
    return m_paramPublish.VU[0].nVEnSubID;
}

inline int CRTMPPublisher::getSourceType()
{
    return m_typePublish;
}

inline int CRTMPPublisher::getSelectMicId()
{
    return m_paramPublish.nSelectMicID;
}

inline int CRTMPPublisher::getSelectCameraId()
{
    return m_paramPublish.VU[0].nSelectCameraID;
}

inline bool CRTMPPublisher::getPublish()
{
    return m_paramPublish.bIsPublish;
}

inline bool CRTMPPublisher::getDrawRect()
{
    return m_paramPublish.bDrawLocRect;
}

#endif
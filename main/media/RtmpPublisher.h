//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�RtmpPublisher.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.24
//	˵����rtmp��������װ���ඨ��ͷ�ļ�
//  �ӿڣ�
//	�޸ļ�¼��
//      2015.11.27   �����ӿ�
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
    // ��ʼ����RTMP��
    bool start(void);

    // �ı䷢����RTMP���Ĳ���
    bool change(void);

    // ֹͣ����RTMP��
    void stop(void);

    // �յ���Ƶ���ݴ�����
    void capVideoData(unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);

public:
    // ������Ƶ����������
    inline void setMediaLevel(MediaLevel ml);

    // ����RTMP���ķ�����ɫ
    inline void setMediaRole(MediaRole mr);

    // ������Ƶ���Ļ����߱���
    inline void setShowScale(SHOWSCALE sc);

    // ������Ƶ�������ӻ�����Ϣ
    inline void setMasterSlave(MasterSlave ms);

    // ����������Ч(��Ƶ��)
    inline void setAVEFilter(AVE_FILTER_TYPE aveFilter);

    // �������յȼ�(��Ƶ��)
    inline void setAVESubLevel(int aveSubLevel);

    // ����RTMP����Դ����:SOURCECAMERA,SOURCESCREEN,SOURCEDEVAUDIO,SOURCEDEMUSIC
    inline void setSourceType(int sourceType);

    // ������˷��ID��0 ~ N
    inline void setSelectMicId(int micId);

    // ��������ͷ��ID��0 ~ N
    inline void setSelectCameraId(int cameraId);

    // �����Ƿ񷢲���(Ԥ��ʱ������Ҫ������Ƶ����
    inline void setPublish(bool isPublish);

    // ������Ƶ�Ƿ񽹵��
    inline void setDrawRect(bool isDrawRect);

    // �����û�����
    inline void setUserData(LPVOID userData);
public:
    // ��ȡ��Ƶ����������
    inline MediaLevel getMediaLevel();

    // ��ȡRTMP���ķ�����ɫ
    inline MediaRole getMediaRole();

    // ��ȡ��Ƶ���Ļ����߱���
    inline SHOWSCALE getShowScale();

    // ��ȡ��Ƶ�������ӻ�����Ϣ
    inline MasterSlave getMasterSlave();

    // ��ȡ������Ч(��Ƶ��)
    inline AVE_FILTER_TYPE getAVEFilter();

    // ��ȡ���յȼ�(��Ƶ��)
    inline int getAVESubLevel();

    // ��ȡRTMP����Դ����:SOURCECAMERA,SOURCESCREEN,SOURCEDEVAUDIO,SOURCEDEMUSIC
    inline int getSourceType();

    // ��ȡ��˷��ID��0 ~ N
    inline int getSelectMicId();

    // ��ȡ����ͷ��ID��0 ~ N
    inline int getSelectCameraId();

    // ��ȡ�Ƿ񷢲���(Ԥ��ʱ������Ҫ������Ƶ����
    inline bool getPublish();

    // ��ȡ��Ƶ�Ƿ񽹵��
    inline bool getDrawRect();

    // ��ȡ�û�����
    inline LPVOID getUserData();

private:
    PublishParam    m_paramPublish;    
    int             m_typePublish;

    LPVOID          m_userData;
};

//////////////////////////////////////////////////////////////////////////
//�������ýӿ�
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
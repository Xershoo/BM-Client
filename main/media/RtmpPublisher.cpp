//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�RtmpPublisher.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.24
//	˵����rtmp��������װ����ʵ��Դ�ļ�
//  �ӿڣ�
//	�޸ļ�¼��
//**********************************************************************
#include "RtmpPublisher.h"

CRTMPPublisher::CRTMPPublisher()
{
    m_paramPublish.hShowHwnd = (HWND)this;
    m_typePublish = 0;
    m_isStart = false;
    m_userData = NULL;
}

CRTMPPublisher::~CRTMPPublisher()
{

}

//////////////////////////////////////////////////////////////////////////
//RTMP�����ƽӿ�
bool CRTMPPublisher::start(void)
{
    if(m_rtmpUrl.empty())
    {
        return false;
    }

    if(m_isStart)
    {
        return true;
    }

    if(m_typePublish == SOURCEDEVAUDIO)
    {
        m_paramPublish.nVUNum = 0;
    }
    else
    {
        m_paramPublish.nVUNum = 1;
    }
	
	if(m_paramPublish.nVUNum == 1)
	{
		if(m_paramPublish.VU[0].nSelectCameraID >= GetDevCameraCount()&&m_paramPublish.VU[0].nSelectCameraID >0)
		{
			return true;
		}
	}

    m_isStart = ::rtmpPushStreamToServerBegin(m_rtmpUrl.c_str(),m_typePublish,m_paramPublish);
    if(!m_isStart)
    {
        return false;
    }

    return true;
}

bool CRTMPPublisher::change(void)
{
    if(!m_isStart)
    {
        return false;
    }

    bool br = ::rtmpPushStreamToServerChange(m_rtmpUrl.c_str(),m_typePublish,m_paramPublish);
    if(!br)
    {
        return false;
    }

    return true;
}

void CRTMPPublisher::stop(void)
{
    if(!m_isStart)
    {
        return;
    }
    
    ::rtmpPushStreamToServerEnd(m_rtmpUrl.c_str(),true);
    m_isStart = false;
}


//////////////////////////////////////////////////////////////////////////
//���ݽӿ�
void CRTMPPublisher::capVideoData(unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{
    if(NULL == pData || 0 >= nSize || 0 >= nVideoH || 0>= nVideoW)
    {
        return;
    }

    RtmpVideoBuf vBuf(getSelectCameraId(),pData,nSize,nVideoW,nVideoH,false);
	this->show(vBuf);
}


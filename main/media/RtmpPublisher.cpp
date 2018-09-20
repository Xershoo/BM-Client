//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpPublisher.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.24
//	说明：rtmp流发布封装的类实现源文件
//  接口：
//	修改记录：
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
//RTMP流控制接口
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
//数据接口
void CRTMPPublisher::capVideoData(unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{
    if(NULL == pData || 0 >= nSize || 0 >= nVideoH || 0>= nVideoW)
    {
        return;
    }

    RtmpVideoBuf vBuf(getSelectCameraId(),pData,nSize,nVideoW,nVideoH,false);
	this->show(vBuf);
}


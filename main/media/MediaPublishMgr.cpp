//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaPublishMgr.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.27
//	说明：rtmp流发布管理类实现源码文件
//  接口：
//	修改记录：
//      
//**********************************************************************
#include "MediaPublishMgr.h"
#include "macros.h"
#include "log.h"


//////////////////////////////////////////////////////////////////////////
void localVideoshowCallBack(EventInfoType enmuType,void* dwUser,unsigned char* pBuf,unsigned int nBufSize,void* Param)
{
	if(!CMediaPublishMgr::isValidate())
	{
		return;
	}

    CMediaPublishMgr* mediaPublishMgr = static_cast<CMediaPublishMgr*>(dwUser);
    if(NULL == mediaPublishMgr)
    {
        return;
    }

	if(!mediaPublishMgr->getPlayVideo())
	{
		return;
	}

    ShowVideoParam* showVideoParam = static_cast<ShowVideoParam*>(Param);
    if(NULL == showVideoParam)
    {
        return;
    }

    mediaPublishMgr->localVideoShow(pBuf,nBufSize,showVideoParam);

	return;
}


//////////////////////////////////////////////////////////////////////////
CMediaPublishMgr* CMediaPublishMgr::m_pMediaPublishMgr = NULL;

CMediaPublishMgr::CMediaPublishMgr():m_mediaThread(this)
{
	AVP_Init();

	memset(m_addrPull,NULL,sizeof(PlayAddress) * MAX_RTMP_ADDR_NUM);
	memset(m_addrPush,NULL,sizeof(PushAddress) * MAX_RTMP_ADDR_NUM);
	memset(m_szPushRecordUrl,NULL,sizeof(char)*MAX_PATH);
	m_recordScreen = false;

	m_addrPullNum = 0;
	m_addrPushNum = 0;
    m_selCarmer = 0;
    m_selMic = 0;
    m_selSpeaker = 0;
    m_mainSeat = 0;
    m_userId = 0;
    m_initMedia = false;
	m_freeAll = false;
	m_playVideo = false;

    qRegisterMetaType<RtmpVideoBuf>("RtmpVideoBuf");

	m_mediaThread.start(QThread::NormalPriority);
}

CMediaPublishMgr::~CMediaPublishMgr()
{
	
}

CMediaPublishMgr* CMediaPublishMgr::getInstance()
{
	if(NULL == m_pMediaPublishMgr)
	{
		m_pMediaPublishMgr = new CMediaPublishMgr();		
	}

	return m_pMediaPublishMgr;
}

void CMediaPublishMgr::freeInstance()
{
	if(m_pMediaPublishMgr)
	{
		AVP_Unit();

		m_pMediaPublishMgr->m_freeAll = true;
		if(m_pMediaPublishMgr->m_mediaThread.isRunning())
		{
			m_pMediaPublishMgr->m_mediaThread.quit();
		}

		m_pMediaPublishMgr->stopRecordScreen();
		m_pMediaPublishMgr->freeDeviceList();
		m_pMediaPublishMgr->freePlayerList();
        m_pMediaPublishMgr->freePublisherList();
        m_pMediaPublishMgr->freePublishSeatList();
        m_pMediaPublishMgr->freeCallbackList();
	}

	SAFE_DELETE(m_pMediaPublishMgr);
}

bool CMediaPublishMgr::isValidate()
{
	return m_pMediaPublishMgr == NULL ? false:true;
}

void CMediaPublishMgr::setUserId(__int64 userId)
{
    m_userId = userId;
}

__int64 CMediaPublishMgr::getUserId()
{
    return m_userId;
}

void CMediaPublishMgr::setMainSeat(int nIndex)
{
    if(nIndex < 0 || nIndex > m_lstPublishSeat.size())
    {
        return;
    }

    if(m_mainSeat == nIndex)
    {
        return;
    }

    //......
    LPPUBLISHSEATINFO pOldMainSeat = getPublishSeatInfo(m_mainSeat);
    LPPUBLISHSEATINFO pNewMainSeat = getPublishSeatInfo(nIndex);
    
    if(pOldMainSeat)
    {
        pOldMainSeat->_main = false;
        if(pOldMainSeat->_ctype == CAMERA_LOCAL)
        {   
            CRTMPPublisher* rtmpPublisher = dynamic_cast<CRTMPPublisher*>(pOldMainSeat->_rtmp);
            if(NULL != rtmpPublisher)
            {
                rtmpPublisher->setMediaLevel(STANDARRDLEVEL);
                rtmpPublisher->setMasterSlave(SUBCAMERA);
            }
        }

        updatePublishSeat(pOldMainSeat,true);
    }

    if(pNewMainSeat)
    {
        pNewMainSeat->_main = true;
        if(pNewMainSeat->_ctype == CAMERA_LOCAL)
        {   
            CRTMPPublisher* rtmpPublisher = dynamic_cast<CRTMPPublisher*>(pNewMainSeat->_rtmp);
            if(NULL != rtmpPublisher)
            {
                rtmpPublisher->setMediaLevel(HIGHLEVEL);
                rtmpPublisher->setMasterSlave(MAINCAMERA);
            }
        }

        updatePublishSeat(pNewMainSeat,true);
    }

    m_mainSeat = nIndex;
}

int CMediaPublishMgr::getMainSeat()
{
    return m_mainSeat;
}

void CMediaPublishMgr::setSeatVideoDevice(int nIndex,int nDevType,int nDevId)
{   
    if(nIndex < 0 || nIndex >= m_lstPublishSeat.size())
    {
        return;
    }

    LPPUBLISHSEATINFO seatPublish = m_lstPublishSeat.at(nIndex);
    if(NULL==seatPublish)
    {
        return;
    }

    if(seatPublish->_cid == nDevId && seatPublish->_ctype == nDevType)
    {
        return;
    }

    for(int i=0;i<m_lstPublishSeat.size();i++)
    {
		if(i == nIndex)
		{
			continue;
		}

        LPPUBLISHSEATINFO useSeat = m_lstPublishSeat.at(nIndex);
        if(useSeat->_cid == nDevId && useSeat->_ctype == nDevType)
        {            
			if(useSeat->_rtmp && useSeat->_rtmp->isStart())
			{
				useSeat->_rtmp->stop();
			}

            useSeat->_cid = -1;
            useSeat->_ctype = -1;
            useSeat->_rtmp = NULL;

            break;
        }
    }

	seatPublish->_cid = nDevId;
    seatPublish->_ctype = nDevType;
	
    updatePublishSeat(seatPublish,true);
}

void CMediaPublishMgr::setSelCamera(int nCamera)
{
    if(nCamera == m_selCarmer || nCamera < 0)
    {
        return;
    }
        
    m_selCarmer = nCamera;	
	
	if(m_lstPublishSeat.empty())
	{
        emit selCameraChanged(nCamera);
		return;
	}

	if(m_lstPublishSeat.size() > 1)		//多机位时不需要改变视频显示
	{
        emit selCameraChanged(nCamera);
		return;
	}

    LPPUBLISHSEATINFO seatPublish = m_lstPublishSeat.at(m_mainSeat);
    if(NULL==seatPublish&&seatPublish->_ctype != CAMERA_LOCAL)
    {
        return;
    }

    CRTMPPublisher* rtmpPublish = getRtmpPublisherByCamera(nCamera);
    if(NULL != rtmpPublish)
    {
        rtmpPublish->setSelectCameraId(seatPublish->_cid);
        if(rtmpPublish->isStart())
        {
            rtmpPublish->change();
        }
    }

    seatPublish->_cid = nCamera;
    updatePublishSeat(seatPublish);     //选择当前摄像头

	emit selCameraChanged(nCamera);
	
    // xiewb 20160115
	/* 
    LPPUBLISHSEATINFO seatPublish = m_lstPublishSeat.at(m_mainSeat);
    if(NULL==seatPublish)
    {
        return;
    }

    if(seatPublish->_ctype != CAMERA_LOCAL)
    {
		return;
	}
	
	LPPUBLISHSEATINFO seatOther = getPublishSeatInfo(CAMERA_LOCAL,nCamera);

	int cid = seatPublish->_cid;
    setSeatVideoDevice(m_mainSeat,CAMERA_LOCAL,nCamera);
		
	// 对调摄像头
	if(seatOther)
	{
		setSeatVideoDevice(seatOther->_sid,CAMERA_LOCAL,cid);
	}
	*/
}

void CMediaPublishMgr::setSelMic(int nMic)
{
    if(nMic == m_selMic || nMic < 0 )
    {
        return;
    }

    m_selMic = nMic;

    if(m_lstPublishSeat.empty())
    {
        return;
    }

    LPPUBLISHSEATINFO seatPublish = m_lstPublishSeat.at(m_mainSeat);
    if(NULL == seatPublish)
    {
        return;
    }

    if(seatPublish->_ctype == CAMERA_NETWORK)
    {
        return;
    }

    CRTMPPublisher * rtmpPublisher = dynamic_cast<CRTMPPublisher*>(seatPublish->_rtmp);
    if(NULL==rtmpPublisher)
    {
        return;
    }

    rtmpPublisher->setSelectMicId(nMic);
    if(rtmpPublisher->isStart())
    {
        rtmpPublisher->change();
    }
}

int CMediaPublishMgr::getCameraCount()
{
    int nCount = 0;
    for(unsigned int i= 0; i<m_lstCameraDevice.size();i++)
    {
        LPMEDIADEVICEINFO pCDI = m_lstCameraDevice.at(i);
        if(NULL == pCDI)
        {
            continue;
        }

        if(pCDI->_type == CAMERA_LOCAL || pCDI->_type == CAMERA_NETWORK)
        {
            nCount++;
        }
    }

    return nCount;
}

//////////////////////////////////////////////////////////////////////////

string CMediaPublishMgr::getSeatPullUrl(int nIndex,bool bAudio /* = true */)
{
    if( nIndex < 0 || nIndex >= 3)
    {
        return string("");
    }

    LPPUBLISHSEATINFO pSPI = getPublishSeatInfo(nIndex);
    bool bMain = false;
    if(pSPI)
    {
        bMain = pSPI->_main;
    }

    string strUrl = string(m_addrPull[nIndex+1].szPushAddr);
    if(bMain&&bAudio)
    {
        strUrl += string("|@|");
        strUrl += string(m_addrPull[0].szPushAddr);
    }

    return strUrl;
}

string CMediaPublishMgr::getSeatPushUrl(int nIndex,bool bAudio /* = true */)
{
    if( nIndex < 0 || nIndex >= 3)
    {
        return string("");
    }

    LPPUBLISHSEATINFO pSPI = getPublishSeatInfo(nIndex);
    bool bMain = false;
    if(pSPI)
    {
        bMain = pSPI->_main;
    }

    string strUrl = m_addrPush[nIndex+1].szPushAddr;
    if(bMain&&bAudio)
    {
        strUrl += string("|@|");
        strUrl += string(m_addrPush[0].szPushAddr);
    }

    return strUrl;
}

//////////////////////////////////////////////////////////////////////////
void CMediaPublishMgr::mediaInitFinish()
{
    loadLocalDevice();
    initPublisherList();

    m_initMedia = true;

    for(int i=0;i<m_lstPublishSeat.size();i++)
    {
        LPPUBLISHSEATINFO seatPublish = m_lstPublishSeat.at(i);
        updatePublishSeat(seatPublish);
    }

	emit initFinished();
}

bool CMediaPublishMgr::isInitMedia()
{
    return m_initMedia;
}

//////////////////////////////////////////////////////////////////////////
void CMediaPublishMgr::loadLocalDevice()
{
    long nCount = ::GetDevCameraCount();
	nCount = (nCount <= 0) ? 1:nCount;
    for(int i=0;i<nCount;i++)
    {
        char szDevName[MAX_PATH] = { 0 };
        if(!::GetDevCameraName(i,szDevName))
        {
            continue;
        }

        LPMEDIADEVICEINFO pCDI = new MEDIADEVICEINFO;
        pCDI->_id = i;
        pCDI->_name = string(szDevName);
        pCDI->_type = CAMERA_LOCAL;

        m_lstCameraDevice.push_back(pCDI);
    }

    nCount = ::GetDevMicCount();
    for(int i=0;i<nCount;i++)
    {
        char szDevName[MAX_PATH] = { 0 };
        if(!::GetDevMicName(i,szDevName))
        {
            continue;
        }

        LPMEDIADEVICEINFO pCDI = new MEDIADEVICEINFO;
        pCDI->_id = i;
        pCDI->_name = string(szDevName);
        pCDI->_type = MICROPHONE_LOCAL;

        m_lstCameraDevice.push_back(pCDI);
    }

    nCount = ::GetDevSpeakerCount();
    for(int i=0;i<nCount;i++)
    {
        char szDevName[MAX_PATH] = { 0 };
        if(!::GetDevSpeakerName(i,szDevName))
        {
            continue;
        }

        LPMEDIADEVICEINFO pCDI = new MEDIADEVICEINFO;
        pCDI->_id = i;
        pCDI->_name = string(szDevName);
        pCDI->_type = SPEAKER_LOCAL;

        m_lstCameraDevice.push_back(pCDI);
    }
}

void CMediaPublishMgr::addNetworkCamera(int nId,WCHAR* pszName)
{
    if(nId == 0||NULL==pszName||NULL==pszName[0])
    {
        return;
    }

    if(getMediaDevice(nId,CAMERA_NETWORK))
    {
        return;
    }

    QString qstrName = QString::fromStdWString(pszName);
    LPMEDIADEVICEINFO pCDI = new MEDIADEVICEINFO;
    pCDI->_id = nId;
    pCDI->_name = string((const char*)qstrName.toLocal8Bit().data());
    pCDI->_type = CAMERA_NETWORK;

    m_lstCameraDevice.push_back(pCDI);

    return;
}

LPMEDIADEVICEINFO CMediaPublishMgr::getMediaDevice(int nId,int nType)
{
    LPMEDIADEVICEINFO pCDI = NULL;
    for(int i=0;i<m_lstCameraDevice.size();i++)
    {
        pCDI = m_lstCameraDevice.at(i);
        if(NULL == pCDI)
        {
            continue;
        }

        if(pCDI->_type == nType && pCDI->_id == nId)
        {
            break;
        }

        pCDI = NULL;
    }

    return pCDI;
}

void CMediaPublishMgr::delNetworkCamera(int nId)
{
    for(int i=0;i<m_lstCameraDevice.size();)
    {
        LPMEDIADEVICEINFO  pCDI = m_lstCameraDevice.at(i);
        if(NULL == pCDI)
        {
            i++;
            continue;
        }

        if(pCDI->_type == CAMERA_NETWORK && (pCDI->_id == nId || 0 == nId))
        {            
            if(pCDI && pCDI->_seat >= 0)
            {
                stopSeatVideo(pCDI->_seat);
            }

            SAFE_DELETE(pCDI);			
            m_lstCameraDevice.erase(m_lstCameraDevice.begin() + i);
            
            break;
        }

        i++;
    }

    LPPUBLISHSEATINFO seatPublish = getPublishSeatInfo(CAMERA_NETWORK,nId);
    if(seatPublish)
    {
        stopSeatVideo(seatPublish->_sid);
    }
}

MediaDeviceList& CMediaPublishMgr::getMediaDeviceList()
{
    return m_lstCameraDevice;
}

//////////////////////////////////////////////////////////////////////////
CRTMPPublisher* CMediaPublishMgr::getRtmpPublisher(int nIndex)
{
    if(nIndex >= m_lstPublisher.size())
    {
        return NULL;
    }

    return m_lstPublisher.at(nIndex);
}

CRTMPPublisher* CMediaPublishMgr::getRtmpPublisherByCamera(int nDevId)
{
    if(m_lstPublisher.empty())
    {
        return NULL;
    }

    CRTMPPublisher* rtmpPublish = NULL;

    for(int i=0;i<m_lstPublisher.size();i++)
    {
        rtmpPublish = m_lstPublisher.at(i);
        if(NULL != rtmpPublish && rtmpPublish->getSelectCameraId() == nDevId)
        {
            break;
        }

        rtmpPublish = NULL;
    }

    return rtmpPublish;
}
//////////////////////////////////////////////////////////////////////////
void CMediaPublishMgr::initPublisherList()
{
    ::SetShowEventCallback(localVideoshowCallBack,(void*)this);

	if(m_lstCameraDevice.empty())
    {
        return;
    }
	
    for(int i = 0; i < m_lstCameraDevice.size();i++)
    {
        LPMEDIADEVICEINFO pDevice = m_lstCameraDevice.at(i);
        if(NULL == pDevice || pDevice->_type != CAMERA_LOCAL)
        {
            continue;
        }

        CRTMPPublisher * pPublisher = new CRTMPPublisher();
        if(NULL == pPublisher)
        {
            break;
        }

        pPublisher->setSourceType(SOURCECAMERA);
        pPublisher->setMediaLevel(STANDARRDLEVEL);
        pPublisher->setMediaRole(ANCHORROLE);
        pPublisher->setMasterSlave(SUBCAMERA);
        pPublisher->setShowScale(SHOWSCALE4A3);
        pPublisher->setAVEFilter(R_FILTER_FILTERBEAUTY);
        pPublisher->setAVESubLevel(2);
        pPublisher->setSelectCameraId(i);
        
        m_lstPublisher.push_back(pPublisher);
    }
}

void CMediaPublishMgr::freePublisherList()
{
    if(m_lstPublisher.empty())
    {
        return;
    }

    for(unsigned int i= 0; i<m_lstPublisher.size();i++)
    {
        CRTMPPublisher* pRPubliser = m_lstPublisher.at(i);
        if(pRPubliser)
        {
            pRPubliser->stop();
        }

        SAFE_DELETE(pRPubliser);
    }

    m_lstPublisher.clear();
}

void CMediaPublishMgr::initPlayerList()
{
    if(m_addrPullNum < 2)
    {
        return;
    }

    for(int i = 0; i< (m_addrPullNum - 1);i++)
    {
        CRTMPPlayer * pPlayer = new CRTMPPlayer();
        if(NULL == pPlayer)
        {
            break;
        }

        string strUrl = getSeatPullUrl(i, m_mainSeat == i ? true : false);
        pPlayer->setUserId(m_userId);
        pPlayer->setUrl(strUrl.c_str());
        pPlayer->setStudent(false);
        
        if(m_mainSeat == i)
        {
            pPlayer->setPlayAudio(true);
            pPlayer->setMainVideo(0);
        }

        pPlayer->setVideoIndex(i);
        m_lstPlayer.push_back(pPlayer);
    }
}

void CMediaPublishMgr::freePlayerList()
{
    if(m_lstPlayer.empty())
    {
        return;
    }

    for(unsigned int i= 0; i<m_lstPlayer.size();i++)
    {
        CRTMPPlayer* pRPlayer = m_lstPlayer.at(i);
        if(pRPlayer)
        {
            pRPlayer->stop();
        }

        SAFE_DELETE(pRPlayer);
    }

    m_lstPlayer.clear();
}

void CMediaPublishMgr::initPublishSeatList()
{
    if(m_addrPushNum < 2)
    {
        return;
    }

    for(int i = 0; i< (m_addrPushNum - 1);i++)
    {
        LPPUBLISHSEATINFO pSeat = new PUBLISHSEATINFO;
        if(NULL == pSeat)
        {
            break;
        }
        
        pSeat->_sid = i;               
        pSeat->_scale = SHOWSCALE4A3;
        pSeat->_main = (m_mainSeat == i);
        pSeat->_cid = -1;
        pSeat->_ctype = -1;
        pSeat->_rtmp = NULL;
        
        m_lstPublishSeat.push_back(pSeat);
        pSeat->_url = getSeatPushUrl(i,m_mainSeat == i ? true : false);

        int nDevId = pSeat->_main ? m_selCarmer : (m_selCarmer == i ? m_mainSeat : i);

        pSeat->_cid = nDevId;
        pSeat->_ctype = CAMERA_LOCAL;

        updatePublishSeat(pSeat);
    }
}

void CMediaPublishMgr::freePublishSeatList()
{
    if(m_lstPublishSeat.empty())
    {
        return;
    }

    for(unsigned int i= 0; i<m_lstPublishSeat.size();i++)
    {
        LPPUBLISHSEATINFO pPSI = m_lstPublishSeat.at(i);        
        SAFE_DELETE(pPSI);
    }

    m_lstPublishSeat.clear();
}

void CMediaPublishMgr::freeDeviceList()
{
    if(m_lstCameraDevice.empty())
    {
        return;
    }

    for(unsigned int i= 0; i<m_lstCameraDevice.size();i++)
    {
        LPMEDIADEVICEINFO pCDI = m_lstCameraDevice.at(i);
        SAFE_DELETE(pCDI);
    }

    m_lstCameraDevice.clear();
}

void CMediaPublishMgr::freeCallbackList()
{
/* 
	for(unsigned int i= 0; i<m_listCallback.size();i++)
	{
		CRtmpStreamCallback* pCallback = m_listCallback.at(i);
		SAFE_DELETE(pCallback);
	}

	for(unsigned int i= 0; i<m_listCallbackDel.size();i++)
	{
		CRtmpStreamCallback* pCallback = m_listCallbackDel.at(i);
		SAFE_DELETE(pCallback);
	}
*/
    m_listCallback.clear();
	m_listCallbackDel.clear();
}
//////////////////////////////////////////////////////////////////////////

PublishSeatList& CMediaPublishMgr::getPublishSeatList()
{
	return m_lstPublishSeat;
}

LPPUBLISHSEATINFO CMediaPublishMgr::getPublishSeatInfo(int nIndex)
{
    if(nIndex < 0 )
    {
        return NULL;
    }

    if(nIndex < m_lstPublishSeat.size())
    {
        return m_lstPublishSeat.at(nIndex);
    }

    if(nIndex > m_lstPublishSeat.size() || nIndex >= MAX_PUSH_SEAT)
    {
        return NULL;
    }

    LPPUBLISHSEATINFO pSeat = new PUBLISHSEATINFO;
    if(NULL == pSeat)
    {
        return NULL;
    }

    pSeat->_sid = nIndex;               
    pSeat->_scale = SHOWSCALE4A3;
    pSeat->_main = (m_mainSeat == nIndex);
    pSeat->_cid = -1;
    pSeat->_ctype = -1;
    pSeat->_rtmp = NULL;
   
    m_lstPublishSeat.push_back(pSeat);
    pSeat->_url = getSeatPushUrl(nIndex,m_mainSeat == nIndex ? true : false);

    int nDevId = pSeat->_main ? m_selCarmer : (m_selCarmer == nIndex ? m_selCarmer : nIndex);
    
    pSeat->_cid = nDevId;
    pSeat->_ctype = CAMERA_LOCAL;
    
    updatePublishSeat(pSeat);

    return pSeat;
}

LPPUBLISHSEATINFO CMediaPublishMgr::getPublishSeatInfo(int nDevType,int nDevId)
{
	if(m_lstPublishSeat.empty())
	{
		return NULL;
	}

	for(int i=0;i<m_lstPublishSeat.size();i++)
	{
		LPPUBLISHSEATINFO seat = m_lstPublishSeat.at(i);
		if(NULL == seat)
		{
			continue;
		}

		if(seat->_cid == nDevId && seat->_ctype == nDevType)
		{
			return seat;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
CRtmpStreamCallback* CMediaPublishMgr::getCallback(CRTMPStream* rtmpStream)
{
    CRtmpStreamCallback* rtmpCallback = NULL;

	/*  xiewb 2018.11.01 synchor biling code
    if(!m_listCallback.empty())
    {
        for(unsigned int i= 0; i<m_listCallback.size();i++)
        {
            CRtmpStreamCallback* pCallback = m_listCallback.at(i);
            if(NULL == pCallback)
            {
                continue;
            }

            if(pCallback->getRtmpSteam() == rtmpStream || pCallback->getRtmpSteam() == NULL)
            {
                pCallback->setRtmpStream(rtmpStream);
                return pCallback;
            }
        }
    }
    */    
    rtmpCallback = new CRtmpStreamCallback();
    if(NULL == rtmpCallback)
    {
        return NULL;
    }

    rtmpCallback->setRtmpStream(rtmpStream);
	rtmpCallback->setShowVideo(m_playVideo);
    m_listCallback.push_back(rtmpCallback);

	return rtmpCallback;
}

void CMediaPublishMgr::delCallback(CRtmpStreamCallback* pCallback)
{
	if(NULL == pCallback)
	{
		return;
	}

	if(m_listCallback.empty())
	{
		return;
	}
	for(unsigned int i= 0; i<m_listCallback.size();i++)
	{
		CRtmpStreamCallback* spCallback = m_listCallback.at(i);
		if(NULL == spCallback)
		{
			continue;
		}

		if(spCallback == pCallback)
		{	
			m_listCallback.erase(m_listCallback.begin() + i);
			m_listCallbackDel.push_back(pCallback);
			return ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CMediaPublishMgr::setMediaUrl(LPCWSTR pwszPushUrl,LPCWSTR pwszPullUrl)
{
	if(NULL==pwszPushUrl || NULL == pwszPushUrl[0] ||NULL==pwszPullUrl || NULL == pwszPullUrl[0])
	{
		freePublishSeatList();
		freePlayerList();

		m_urlPull = "";
		m_urlPush = "";

		return;
	}
	
    QString qstrPullUrl = QString::fromStdWString(pwszPullUrl);
    QString qstrPushUrl = QString::fromStdWString(pwszPushUrl);

    string strPullUrl = string((const char*)qstrPullUrl.toLocal8Bit().data());
    string strPushUrl = string((const char*)qstrPushUrl.toLocal8Bit().data());
	
	if(strPullUrl.compare(m_urlPull.c_str())== 0 && strPushUrl.compare(m_urlPush.c_str()) == 0)
	{
		return;
	}

	freePublishSeatList();
	freePlayerList();

    m_urlPull = strPullUrl;
    m_urlPush = strPushUrl;
    
	memset(m_addrPull,NULL,sizeof(PlayAddress) * MAX_RTMP_ADDR_NUM);
	memset(m_addrPush,NULL,sizeof(PushAddress) * MAX_RTMP_ADDR_NUM);
	m_addrPushNum = 0;
	m_addrPullNum = 0;
		
	::AVP_parsePalyAddrURL(m_urlPull.c_str(),m_addrPull,m_addrPullNum);
	::parsePushAddrURL(m_urlPush.c_str(), m_addrPush,m_addrPushNum);
	
	if(m_addrPushNum > 0)
	{
		initPublishSeatList();
		setRecordScreenUrl();
	}

    if(m_addrPullNum > 0)
    {
        initPlayerList();
    }
}

void CMediaPublishMgr::setPublishParam(bool isTeacher)
{
    if(m_lstPublishSeat.empty())
    {
        return;
    }

    for(int i=0;i<m_lstPublishSeat.size();i++)
    {
        LPPUBLISHSEATINFO seat = m_lstPublishSeat.at(i);
        if(NULL == seat || NULL == seat->_rtmp)
        {
            continue;
        }

        if(seat->_ctype == CAMERA_LOCAL)
        {
            CRTMPPublisher* rtmpPublisher = dynamic_cast<CRTMPPublisher*>(seat->_rtmp);
            if(NULL == rtmpPublisher)
            {
                continue;
            }

            rtmpPublisher->setShowScale((SHOWSCALE)seat->_scale);
            rtmpPublisher->setUrl(seat->_url.c_str());
            rtmpPublisher->setPublish(true);

            if(isTeacher)
            {
                if(seat->_main)
                {
                    rtmpPublisher->setSourceType(SOURCECAMERA|SOURCEDEVAUDIO);
                    rtmpPublisher->setMasterSlave(MAINCAMERA);
                    rtmpPublisher->setMediaLevel(HIGHLEVEL);

                    rtmpPublisher->setSelectMicId(m_selMic);
                }
                else
                {
                    rtmpPublisher->setSourceType(SOURCECAMERA);
                    rtmpPublisher->setMasterSlave(SUBCAMERA);
                    rtmpPublisher->setMediaLevel(STANDARRDLEVEL);
                }

                rtmpPublisher->setMediaRole(ANCHORROLE);
            }
            else
            {
                rtmpPublisher->setSourceType(SOURCECAMERA);
                rtmpPublisher->setMediaLevel(STANDARRDLEVEL);

                if(seat->_main)
                {   
                    rtmpPublisher->setMasterSlave(MAINCAMERA);
                    rtmpPublisher->setSelectMicId(m_selMic);
                }
                else
                {
                    rtmpPublisher->setMasterSlave(SUBCAMERA);
                }

                rtmpPublisher->setMediaRole(LISTENERROLE);
            }

        }
    }
}

//////////////////////////////////////////////////////////////////////////
int CMediaPublishMgr::startSeatVideo(int nIndex)
{
    if(nIndex < 0 || nIndex >= m_lstPublishSeat.size())
    {
        return 0;
    }
	
    LPPUBLISHSEATINFO pPSI = m_lstPublishSeat.at(nIndex);	
    if(NULL == pPSI )
    {
        return 0;
    }

	if( pPSI->_state == PublishSeatInfo::Publish_Seat_Started ||
		pPSI->_state == PublishSeatInfo::Publish_Seat_Starting)
	{
		return 1;
	}

    if(NULL == pPSI->_rtmp)
    {
        return 1;
    }

	pPSI->_state = PublishSeatInfo::Publish_Seat_Starting;
	
	bool ret = pPSI->_rtmp->start();
	
	if(!m_freeAll)
	{
		pPSI->_state = ret ? PublishSeatInfo::Publish_Seat_Started : PublishSeatInfo::Publish_Seat_Unstart;
	}

    return m_freeAll ? -1 : (ret ? 1 : 0);
}

bool CMediaPublishMgr::changeSeatVideo(int nIndex)
{
    if(nIndex < 0)
    {
        for(int i=0 ;i<m_lstPublishSeat.size();i++)
        {			
            changeSeatVideo(i);
        }

        return true;
    }

    if(nIndex >= m_lstPublishSeat.size())
    {
        return false;
    }

    LPPUBLISHSEATINFO pPSI = m_lstPublishSeat.at(nIndex);	
    if(NULL == pPSI )
    {
        return false;
    }
        
    if(NULL == pPSI->_rtmp)
    {
        return false;
    }

    if(pPSI->_state == PublishSeatInfo::Publish_Seat_Started)
    {
        return pPSI->_rtmp->change();
    }
    
	emit startPublishSeatAsync(pPSI->_sid);    
    return true;
}

void CMediaPublishMgr::stopSeatVideo(int nIndex)
{
    if(nIndex < 0)
    {
        for(int i=0 ;i<m_lstPublishSeat.size();i++)
        {			
            stopSeatVideo(i);
        }

        return;
    }

    if(nIndex >= m_lstPublishSeat.size())
    {
        return;
    }

    LPPUBLISHSEATINFO pPSI = m_lstPublishSeat.at(nIndex);
    if(NULL == pPSI || pPSI->_state != PublishSeatInfo::Publish_Seat_Started)
    {
        return;
    }

    pPSI->_state = PublishSeatInfo::Publish_Seat_Unstart;
    if(NULL == pPSI->_rtmp)
    {
        return;
    }

    pPSI->_rtmp->stop();
}

void CMediaPublishMgr::localVideoShow(unsigned char* pBuf,unsigned int nBufSize,ShowVideoParam* pVideoParam)
{
    if(NULL == pBuf || 0 >= nBufSize || NULL == pVideoParam)
    {
        return;
    }

    CRTMPPublisher * rtmpPublisher = static_cast<CRTMPPublisher*>((void*)pVideoParam->hShowHwnd);
    if(NULL == rtmpPublisher)
    {
        return;
    }
	
	RtmpVideoBuf videoBuf(rtmpPublisher->getSelectCameraId(),pBuf,nBufSize,pVideoParam->nImageWidth,pVideoParam->nImageHeight,false);
	rtmpPublisher->show(videoBuf);
	
	return;
}

void CMediaPublishMgr::updatePublishSeat(LPPUBLISHSEATINFO pSeat,bool restart /* = false */)
{
    if(NULL == pSeat)
    {
        return;
    }

    bool bMain =  pSeat->_main;
    pSeat->_url = "";
    
    if(pSeat->_rtmp && pSeat->_rtmp->isStart()&&restart)
    {
        pSeat->_rtmp->stop();
        pSeat->_rtmp->setUrl("");
    }

	if(pSeat->_ctype == CAMERA_LOCAL)
    {
        pSeat->_url = getSeatPushUrl(pSeat->_sid);        
        if(NULL == pSeat->_rtmp && pSeat->_cid != -1)
        {
            pSeat->_rtmp = getRtmpPublisherByCamera(pSeat->_cid);
        }
        
        CRTMPPublisher* rtmpPublisher = dynamic_cast<CRTMPPublisher*>(pSeat->_rtmp);
        if(NULL == rtmpPublisher)
        {
            return;
        }

		rtmpPublisher->setShowScale((SHOWSCALE)pSeat->_scale);
        rtmpPublisher->setUrl(pSeat->_url.c_str());
        rtmpPublisher->setPublish(true);
        rtmpPublisher->setSelectCameraId(pSeat->_cid);
        
        if(bMain)
        {
            rtmpPublisher->setSourceType(SOURCECAMERA|SOURCEDEVAUDIO);
            rtmpPublisher->setMasterSlave(MAINCAMERA);
            rtmpPublisher->setSelectMicId(m_selMic);
            m_selCarmer = rtmpPublisher->getSelectCameraId();
        }
        else
        {
            rtmpPublisher->setSourceType(SOURCECAMERA);
            rtmpPublisher->setMasterSlave(SUBCAMERA);
        }              
    }
    else if(pSeat->_ctype == CAMERA_NETWORK)
    {
        pSeat->_url = getSeatPullUrl(pSeat->_sid);
        if(m_lstPlayer.size() > pSeat->_sid && pSeat->_cid != -1 && NULL == pSeat->_rtmp)
        {
            pSeat->_rtmp = m_lstPlayer.at(pSeat->_sid);
        }
        
        CRTMPPlayer * rtmpPlayer = dynamic_cast<CRTMPPlayer*>(pSeat->_rtmp);;
        if(NULL == rtmpPlayer)
        {
            return;
        }

        if(bMain)
        {
            rtmpPlayer->setPlayAudio(true);
            rtmpPlayer->setMainVideo(0);
        }
        else
        {
            rtmpPlayer->setPlayAudio(false);
            rtmpPlayer->setMainVideo(-1);
        }

        rtmpPlayer->setUrl(pSeat->_url.c_str());
    }
    else
    {
        return;
    }

    if(pSeat->_state == PublishSeatInfo::Publish_Seat_Started)
    {
		if(restart)
		{
			pSeat->_state = PublishSeatInfo::Publish_Seat_Unstart;
			emit startPublishSeatAsync(pSeat->_sid); 
		}
		else
		{
			pSeat->_rtmp->change();
		}
    }

	emit updatedPublishSeat(pSeat->_sid);
}

//////////////////////////////////////////////////////////////////////////
void CMediaPublishMgr::setSelSpeaker(int nSpeaker)
{
    if(SetDevSpeaker(nSpeaker))
    {
        m_selSpeaker = nSpeaker;
    };
}

bool CMediaPublishMgr::setMicMute(bool vol)
{
    return ::setMicMute(vol);
}

bool CMediaPublishMgr::setMicVolume(long vol)
{
    return ::setMicVolume(vol);
}

long CMediaPublishMgr::getMicVolume()
{
    return ::getMicVolume();
}

int CMediaPublishMgr::GetMicRTVolum()
{
    return ::GetMicRTVolum();
}

long CMediaPublishMgr::getSpeakersVolume()
{
    return ::getSpeakersVolume();
}

bool CMediaPublishMgr::setSpeakersMute(bool vol)
{
    return ::setSpeakersMute(vol);
}

bool CMediaPublishMgr::setSpeakersVolume(long vol)
{
    return ::setSpeakersVolume(vol);
}

MDevStatus CMediaPublishMgr::getMicDevStatus()
{
    return ::getMicDevStatus();
}

MDevStatus CMediaPublishMgr::getCamDevStatus()
{
    return ::getCamDevStatus();
}

MDevStatus CMediaPublishMgr::getSpeakersDevStatus()
{
    return ::getSpeakersDevStatus();
}

bool CMediaPublishMgr::PreviewSound(int nMicID,bool bIsPreview)
{
	return ::PreviewSound(nMicID,bIsPreview);
}

void CMediaPublishMgr::startAllSeatVideo()
{
	if(m_lstPublishSeat.empty())
	{
		return;
	}
	
    int retryTime = 0;

	for(int i=0 ;i<m_lstPublishSeat.size();)
	{			
		int ret = startSeatVideo(i);
		if(ret == 0)
		{
            retryTime++;
            if(retryTime > 3)
            {
                break;
            }

			QThread::msleep(50);
			continue;
		}
		else if(ret == -1)
		{
			return;
		}

        retryTime = 0;
		i++;
	}

	return ;
}

void CMediaPublishMgr::setRecordScreenUrl()
{
	if(m_addrPushNum < 1) {
		return;
	}

	string pushUrl(m_addrPush[m_addrPushNum-1].szPushAddr);
	int pos = pushUrl.find("live");
	pushUrl.replace(pos,4,"rec_live");

	strcpy_s(m_szPushRecordUrl,pushUrl.c_str());
}

bool CMediaPublishMgr::startRecordScreen()
{
	if(m_recordScreen) {
		return true;
	}

	if( NULL== m_szPushRecordUrl[0]) {
		return false;
	}

	PublishParam paramPublish;
	int typeStream = SOURCESCREEN | SOURCEDEMUSIC;
	paramPublish.ml = STANDARRDLEVEL;
	paramPublish.nSelectMicID = m_selMic;
	paramPublish.bAudioKaraoke = true;
	paramPublish.bIsFullScreen = true;

	m_recordScreen = ::rtmpPushStreamToServerBegin(m_szPushRecordUrl,typeStream,paramPublish);

	return m_recordScreen;
}

void CMediaPublishMgr::stopRecordScreen()
{
	if(!m_recordScreen) {
		return;
	}

	if( NULL== m_szPushRecordUrl[0]) {
		return;
	}

	::rtmpPushStreamToServerEnd(m_szPushRecordUrl);
	m_recordScreen = false;
}

//////////////////////////////////////////////////////////////////////////
bool CMediaPublishMgr::getPlayVideo()
{
	return m_playVideo;
}

void CMediaPublishMgr::setPlayVideo(bool playVideo)
{
	m_playVideo = playVideo;

	if(m_listCallback.empty())
	{
		return;
	}

	for(unsigned int i= 0; i<m_listCallback.size();i++)
	{
		CRtmpStreamCallback* spCallback = m_listCallback.at(i);
		if(NULL == spCallback)
		{
			continue;
		}
		spCallback->setShowVideo(playVideo);
	}
}

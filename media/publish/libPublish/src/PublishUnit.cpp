#include <stdafx.h>
#include "PublishUnit.h"

list<RtmpAddr> CPublishUnit::m_listRTMPAddr;

void CALLBACK _send_media_loop(void *pUserData)
{
	if(pUserData)
	{
		CPublishUnit * pUser = (CPublishUnit *)pUserData;
		pUser->send_proc();
	}
}

CPublishUnit::CPublishUnit(CSourDataManager* pDataManager)
{
	m_nStreamType = 0; //流类型
	m_MediaUrl[0] = '\0' ; //视频发布地址
	m_pDataManager = pDataManager;
	m_listVideoSour.clear();
	m_listAudioSour.clear();
	m_iSrcVideoWidth = 0;
	m_iSrcVideoHeight = 0;
	m_szRGBBuffer = NULL;
	m_nRGBBufSize = 0;
	m_rtmpImpl = NULL;
	m_udpImpl = NULL;
	m_bIsKaolaOK = false;
	m_bIsPublish = false;
	m_isCapturing = 0;
	m_nMicVolum = 0;
	m_pbPCMBuffer = NULL;
	m_nAACInputSamples = 0;
	m_nVideoFps = 0;
	m_nVideoSpan = 0;
	m_nLastGetTime = 0;
	m_nNextGetTime = 0;
	m_bIsTakePhotos = false;
	m_pEventCallBack = NULL;
	m_EventpUser = NULL;

	m_pShowEventCallBack = NULL;
	m_EShowventpUser = NULL;
	m_swscontext = NULL;

}

CPublishUnit::~CPublishUnit()
{
	m_send_media_thread.End();
	m_listVideoSour.clear();
	m_listAudioSour.clear();
	if(m_BPL.pBuffer)
	{
		free(m_BPL.pBuffer);
		m_BPL.pBuffer = NULL;
	}
	PublishEnd(true);
	if(m_swscontext)
	{
		m_swscontext = NULL;
	}
}

void CPublishUnit::InitRTMPAddlist()
{
	RtmpAddr ra;
	ra.copy("rtmp://class8media.address.ch1","61.147.188.51",1935);
	m_listRTMPAddr.push_back(ra);
	ra.copy("rtmp://class8media.address.ch2","61.147.188.52",1935);
	m_listRTMPAddr.push_back(ra);
	ra.copy("rtmp://class8media.address.ch0","10.2.2.234",1935);
	m_listRTMPAddr.push_back(ra);
}

bool CPublishUnit::FindRealAddrByKey(const char* szHostaddr,RtmpAddr& ra)
{
	char *szHeader = "rtmp://";
	int nHeaderSize =  strlen(szHeader);
	char szKey[256] = {'\0'};
	strcpy(szKey,szHeader);
	if(strncmp(szHeader,szHostaddr,nHeaderSize) != 0)
		return false;
	char *p = (char*)szHostaddr + nHeaderSize;
	int i = nHeaderSize;
	while(*p != '\0' && *p != '\/')
	{
		szKey[i++] = *p++;
	}
	szKey[i] = '\0';

	if(m_listRTMPAddr.size() > 0)
	{
		list<RtmpAddr>::iterator iter;
		for(iter = m_listRTMPAddr.begin();iter != m_listRTMPAddr.end();iter++)
		{
			RtmpAddr rnode = (*iter);
			if(rnode.cmpIsMapping(szKey))
			{
				ra = rnode;
				return true;
			}
		}
	}
	return false;
}

STREAMPROTYPE CPublishUnit::getStreamProtocolType(const char* szUrl)
{
	STREAMPROTYPE  st = RTMPTYPE ;
	if(strncmp(szUrl,"rtmp://",strlen("rtmp://")) == 0)
	{
		st = RTMPTYPE;
	}
	else if(strncmp(szUrl,"udp://",strlen("udp://")) == 0)
	{
		st = UDPTYPE;
	}
	else if(strncmp(szUrl,"rtsp://",strlen("rtsp://")) == 0)
	{
		st = RTSPTYPE;
	}
	else if(strncmp(szUrl,"http://",strlen("http://")) == 0)
	{
		st = HTTPTYPE;
	}
	return st;
}

bool  CPublishUnit::RTMP_ReConnect(bool bReConnect)
{
	if(bReConnect)
	{
		return RtmpConnect(m_MediaUrl,bReConnect);
	}
	return false;
}

bool CPublishUnit::RtmpConnect(const char* szRtmpUrl,bool bReConnect)
{
	if(m_rtmpImpl == NULL || !RTMP_IsConnected(m_rtmpImpl))
	{
		if(m_rtmpImpl == NULL)
		{
			m_rtmpImpl= RTMP_Alloc();  
			if(m_rtmpImpl == NULL) 
				return false;
		}
		RtmpAddr ra;
		RTMP_Init(m_rtmpImpl);
		int nRet = RTMP_SetupURL(m_rtmpImpl, (char*)szRtmpUrl); 
		RTMP_EnableWrite(m_rtmpImpl);  
		
		if(!FindRealAddrByKey(szRtmpUrl,ra))
		{
			if(!RTMP_Connect(m_rtmpImpl, NULL) || !RTMP_ConnectStream(m_rtmpImpl,0))
			{
				return false;
			}
		}
		else
		{
			if(!RTMP_ConnectEx(m_rtmpImpl, NULL,ra.szRealIP,ra.nPort) || !RTMP_ConnectStream(m_rtmpImpl,0))
			{
				return false;
			}
		}
	}
	return true;
}

int CPublishUnit::UDP_SendPacket(unsigned char* pBuf,unsigned nBufSize,unsigned int nPts)
{
	if(m_udpImpl && m_bIsPublish)
	{
		return m_udpImpl->sendData(pBuf,nBufSize,nPts);
	}
	return 0;
}

int CPublishUnit::RTMP_SendPacketEx(SendMediaPackect smp, int queue,bool bIsVideo)
{
	m_sendLock.Lock();

	if(bIsVideo)
	{
		if(m_sendVideoList.size() <= 20)
			m_sendVideoList.push_back(smp);
		else
		{
			RTMPPacket_Free(&smp.packet);
			smp.packet.m_body = NULL;
		}
	}
	else
	{
		if(m_sendAudioList.size() <= 200)
			m_sendAudioList.push_back(smp);
		else
		{
			RTMPPacket_Free(&smp.packet);
			smp.packet.m_body = NULL;
		}
	}

	if(m_sendVideoList.size() <= 0 && m_sendAudioList.size() <= 0)
	{
		m_sendCond.cond_signal();
	}
	m_sendLock.Unlock();
	return 1;
}

bool CPublishUnit::RTMP_IsConnectedEx()
{
	if(m_rtmpImpl)
	{
		return RTMP_IsConnected(m_rtmpImpl);
	}
	return true;
}

bool CPublishUnit::RtmpDisConnect()
{
	if(m_rtmpImpl)
	{
		RTMP_DeleteStream(m_rtmpImpl);
 		RTMP_Close(m_rtmpImpl);
 		RTMP_Free(m_rtmpImpl);
 		m_rtmpImpl = NULL;
 	}
	return true;
}

bool CPublishUnit::bIsExistMediaType(int nMediaType)
{
	//vedio
	int nVideoTemp = m_nStreamType & nMediaType;
	if(nVideoTemp == nMediaType)
		return true;
	return false;
}

bool CPublishUnit::PublishStart(const char* szUrl,int nStreamType,StreamParam param)
{
	//
	if(szUrl == NULL)
		return false;
	
	if(InterlockedExchange(&m_isCapturing,m_isCapturing))
	{
		return true;
	}

	m_bIsPublish = param.bIsPublish;
	m_nStreamType = nStreamType;
	strcpy(m_MediaUrl,szUrl);
	int nTempType = m_nStreamType & VIDEOTYPE;
	if(nTempType == VIDEOTYPE)
	{
		m_nVideoFps = param.nVideoFps > 0 ? param.nVideoFps : 1;
		m_nVideoSpan = 1000 / param.nVideoFps;
		m_nLastGetTime = 0;
		m_nNextGetTime = 0;
	}

	if(!SelectDevSour(nStreamType,param))
	{
		return false;
	}

	if(!SelectEncoder(nStreamType,param))
	{
		return false;
	}
	
	if(m_bIsPublish)
	{
		m_stNetType = getStreamProtocolType(szUrl);
		if(m_stNetType == RTMPTYPE)
		{
			if(!RtmpConnect(szUrl,false))
				return false;
		}
		else if(m_stNetType == UDPTYPE)
		{
			if(m_udpImpl == NULL)
			{
				m_udpImpl = new CUDPClinet(szUrl);
				m_udpImpl->InitUDPClient(UDPPUSH);
			}	
		}
	}

	if(m_send_media_thread.GetStop())
	{
		m_sendCond.conde_setwaittime(10);
		m_send_media_thread.Begin(_send_media_loop,this);
	}
	InterlockedExchange(&m_isCapturing,1);
	return true;
}


bool CPublishUnit::PublishChanage(const char* szUrl,int nStreamType,StreamParam param)
{
	// 
	if(!InterlockedExchange(&m_isCapturing,m_isCapturing))
	{
		return false;
	}

	m_nStreamType = nStreamType;
	m_bIsPublish = param.bIsPublish;
	int nTempType = m_nStreamType & VIDEOTYPE;
	if(nTempType == VIDEOTYPE)
	{
		m_nVideoFps = param.nVideoFps > 0 ? param.nVideoFps : 1;
		m_nVideoSpan = 1000 / param.nVideoFps;
	}

	if(!SelectDevSour(nStreamType,param))
	{
		return false;
	}

	if(!SelectEncoder(nStreamType,param))
	{
		return false;
	}

	if(m_bIsPublish)
	{
		m_stNetType = getStreamProtocolType(szUrl);
		if(m_stNetType == RTMPTYPE)
		{
			if(!RtmpConnect(szUrl,false))
				return false;
		}
		else if(m_stNetType == UDPTYPE)
		{
			if(m_udpImpl == NULL)
			{
				m_udpImpl = new CUDPClinet(szUrl);
			}	
			m_udpImpl->InitUDPClient(UDPTYPE);
		}
	}
	else
	{
		int nTempType = m_nStreamType & AUDIOTYPE;
		if(m_udpImpl && nTempType != AUDIOTYPE)
		{
			m_udpImpl->UnInitUDPClient();
		}	
	}

	return true;
}

bool CPublishUnit::PublishEnd(bool bIsColseDev)
{
	if(InterlockedExchange(&m_isCapturing,m_isCapturing))
	{
		m_send_media_thread.End();
		RtmpDisConnect();
		if(m_udpImpl)
		{
			m_udpImpl->UnInitUDPClient();
			delete m_udpImpl;
			m_udpImpl = NULL;
		}

		for(m_encoderVIter = m_listVE.begin();m_encoderVIter != m_listVE.end();m_encoderVIter++)
		{
			VideoEncoderNode ven = (*m_encoderVIter);
			if(ven.ph264Encoder)
			{
				ven.ph264Encoder->h264_EncodeUninit();
				delete ven.ph264Encoder;
				ven.ph264Encoder = NULL;
			}
		}
		m_listVE.clear();

		if(m_aCurExcEncoder.paacEncoder)
		{
			m_aCurExcEncoder.paacEncoder->aac_EncodeUnInit();
			delete m_aCurExcEncoder.paacEncoder;
			m_aCurExcEncoder.paacEncoder = NULL;
		}

		m_picLock.Lock();
		list<PicSourceNode>::iterator iter;
		for(iter = m_listVideoSour.begin();iter != m_listVideoSour.end();iter++)
		{
			if((*iter).pSD && (*iter).pSD->GetSourType() == SOURCECAMERA)
			{
				(*iter).pSD->SourClose(bIsColseDev);
			}
		}
		m_picLock.Unlock();

		if(m_szRGBBuffer)
		{
			delete [] m_szRGBBuffer;
			m_szRGBBuffer = NULL;
		}

		if(m_pbPCMBuffer)
		{
			delete [] m_pbPCMBuffer;
			m_pbPCMBuffer = NULL;
		}
		clearSendList();
	}
	m_isCapturing = 0;
	return true;
}

struct cmp
{
	bool operator () (const PicLayer& a,const PicLayer& b)
	{
		return a.nW > b.nW;
	}
};

bool CPublishUnit::setShowRect(int nIndex,int nStreamType,ScRect showRect)
{
	m_picLock.Lock();
	list<PicSourceNode>::iterator iter;
	for(iter = m_listVideoSour.begin();iter != m_listVideoSour.end();iter++)
	{
		if((*iter).pSD->GetSourType() == nStreamType)
		{
			if(nIndex == (*iter).pSD->getSourID())
			{
				(*iter).rc = showRect;
				break;
			}
		}
	}
	m_picLock.Unlock();
	return true;
}

bool CPublishUnit::SelectDevSour(int nStreamType,StreamParam param)
{
	bool bIsSelsct = false;
	m_nStreamType = nStreamType;

	m_picLock.Lock();
	m_audLock.Lock();
	m_iSrcVideoWidth = 0;
	m_iSrcVideoHeight = 0;
	m_listAudioSour.clear();
	m_listVideoSour.clear();

	if(m_pDataManager)
	{
		if((nStreamType & SOURCECAMERA ) == SOURCECAMERA)
		{
			int nSrcVideoWidth = 0;
			int nSrcVideoHeight = 0;
			for(int i = 0; i< param.nVUNum;i++)
			{
				if(param.VU[i].nType == SOURCECAMERA)
				{
					StreamParam tempPparam = param;
					tempPparam.VU[0] = param.VU[i];
					tempPparam.nVUNum = 1;
					ISourData*  pSD = m_pDataManager->getSourData(MEDIACAMERAVIODE,param.VU[i].nSelectCameraID);
					if(pSD)
					{
						pSD->SetSourType(nStreamType);
						// video enhcnements
						pSD->AVE_SetID(tempPparam.VU[0].nVEnMainID,tempPparam.VU[0].nVEnSubID, tempPparam.VU[0].nParam);
						pSD->SourOpen(&tempPparam);
						pSD->GetVideoWAndH(nSrcVideoWidth,nSrcVideoHeight);
						if(nSrcVideoWidth > m_iSrcVideoWidth )
						{
							m_iSrcVideoWidth = nSrcVideoWidth;
						
						}
						if(nSrcVideoHeight > m_iSrcVideoHeight)
						{
							m_iSrcVideoHeight = nSrcVideoHeight;
						}
						PicSourceNode sn;
						sn.pSD = pSD;
						sn.rc = param.VU[i].showRect;
						m_listVideoSour.push_back(sn);
						bIsSelsct = true;
					}
				}
			}
		}

		if((nStreamType & SOURCESCREEN ) == SOURCESCREEN)
		{
			int nSrcVideoWidth = 0;
			int nSrcVideoHeight = 0;
			for(int i = 0; i< param.nVUNum;i++)
			{
				if(param.VU[i].nType == SOURCESCREEN)
				{
					ISourData*  pSD = m_pDataManager->getSourData(MEDIASCREENVIODE,param.VU[i].nSelectCameraID);
					if(pSD)
					{
						StreamParam tempPparam = param;
						tempPparam.VU[0] = param.VU[i];
						tempPparam.nVUNum = 1;
						pSD->SetSourType(nStreamType);
						// video enhancments
						pSD->AVE_SetID(tempPparam.VU[0].nVEnMainID,tempPparam.VU[0].nVEnSubID, tempPparam.VU[0].nParam);
						pSD->SourOpen(&tempPparam);
						pSD->GetVideoWAndH(nSrcVideoWidth,nSrcVideoHeight);
						if(nSrcVideoWidth > m_iSrcVideoWidth )
						{
							m_iSrcVideoWidth = nSrcVideoWidth;
						
						}
						if(nSrcVideoHeight > m_iSrcVideoHeight)
						{
							m_iSrcVideoHeight = nSrcVideoHeight;
						}

						PicSourceNode sn;
						sn.pSD = pSD;
						sn.rc = param.VU[i].showRect;
						m_listVideoSour.push_back(sn);
						bIsSelsct = true;
					}
				}
			}
		}

		if((nStreamType & SOURCEDEVAUDIO ) == SOURCEDEVAUDIO)
		{
			ISourData*  pSD = m_pDataManager->getSourData(MEDIAMICAUDIO,param.nSelectMicID);
			if(pSD)
			{
				pSD->SetSourType(nStreamType);
				pSD->SourOpen(&param);
				m_listAudioSour.push_back(pSD);
				bIsSelsct = true;
			}
		}

		if((nStreamType & SOURCEDEMUSIC ) == SOURCEDEMUSIC)
		{
			ISourData*  pSD = m_pDataManager->getSourData(MEDIASPEKAUDIO,param.nSelectMicID);
			if(pSD)
			{
				((CSourAudio*)pSD)->setSaveFalg(true);
				pSD->SetSourType(nStreamType);
				pSD->SourOpen(&param);
				m_listAudioSour.push_back(pSD);
				bIsSelsct = true;
			}
		}
	}
	m_audLock.Unlock();
	m_picLock.Unlock();
	return bIsSelsct;
}

bool CPublishUnit::SelectVideoEnhanceTypes(int nStreamType,StreamParam param)
{
	bool bStatus = true;
	
	if(m_pDataManager)
	{
		for(int i = 0; i< param.nVUNum;i++)
		{
			if(param.VU[i].nType == SOURCECAMERA || param.VU[i].nType == SOURCESCREEN)
			{
				ISourData*  pSD = m_pDataManager->getSourData(MEDIASCREENVIODE,param.VU[i].nSelectCameraID);
				if(pSD)
				{
					pSD->AVE_SetID(param.VU[i].nVEnMainID,param.VU[i].nVEnSubID, param.VU[0].nParam);
				}
			}
		}
	}

	return bStatus;
}

bool CPublishUnit::SelectEncoder(int nStreamType,StreamParam param)
{
	bool bAIsExist = false;
	bool bVIsExist = false;
	VideoEncoderNode venTemp;
	AudioEncoderNode aenTemp;
	//audio
	m_bIsKaolaOK = param.bAudioKaraoke;
	int nAudioTemp = nStreamType & AUDIOTYPE;

	if(nAudioTemp == AUDIOTYPE)
	{
		m_aLock.Lock();
		AudioEncoderNode aen = m_aCurExcEncoder;
		if(aen.bAudioKaraoke == param.bAudioKaraoke
			&& aen.nAudioBitRate == param.nAudioBitRate 
			&& aen.nAudioSmpRateCap == param.nAudioSmpRateCap && aen.paacEncoder)
		{
			bAIsExist = true;		
		}
		else
		{
			if(aen.paacEncoder)
			{
				aen.paacEncoder->aac_EncodeUnInit();
				delete aen.paacEncoder;
				aen.paacEncoder = NULL;
			}
		}

	    if(!bAIsExist)
	    {
			AudioEncoderNode aen;
			aen.bAudioKaraoke = param.bAudioKaraoke;
			aen.nAudioBitRate = param.nAudioBitRate;
			aen.nAudioSmpRateCap = param.nAudioSmpRateCap;

			int iChannel = param.nAudioChannels;
			ULONG  nAACInputSamples = 0;
			aen.paacEncoder = new CAACEncoder();
			if(aen.paacEncoder && aen.paacEncoder->aac_EncodeInit(iChannel,aen.nAudioBitRate,aen.nAudioSmpRateCap,nAACInputSamples))
			{
				if(nAACInputSamples != m_nAACInputSamples)
				{
					delete m_pbPCMBuffer;
					m_pbPCMBuffer = NULL;
					m_nAACInputSamples = nAACInputSamples;
				}
				if(m_pbPCMBuffer == NULL )
					m_pbPCMBuffer = new BYTE [m_nAACInputSamples];
			}
			else
			{
				if(aen.paacEncoder)
				{
					delete aen.paacEncoder;
					aen.paacEncoder = NULL;
				}
				m_aLock.Unlock();
				return false;
			}
			aenTemp = aen;
			m_aCurExcEncoder = aenTemp;
		}
		
		m_aLock.Unlock();
	}

	//vedio
	int nVideoTemp = nStreamType & VIDEOTYPE;
	if(nVideoTemp == VIDEOTYPE)
	{
		for(m_encoderVIter = m_listVE.begin();m_encoderVIter != m_listVE.end();m_encoderVIter++)
		{
			VideoEncoderNode ven = (*m_encoderVIter);
			if(ven.nVideoBitRate == param.nVideoBitRate 
				&&ven.nVideoFps == param.nVideoFps
				&&ven.nVideoH == param.nVideoH
				&&ven.nVideoW == param.nVideoW
				&&ven.ph264Encoder)
			{
				bVIsExist = true;
				ven.ph264Encoder->h264_EncodeInit(m_iSrcVideoWidth,m_iSrcVideoHeight);
				venTemp = ven;
				break;
			}
		}

		if( !bVIsExist )
		{
			VideoEncoderNode ven;
			ven.nVideoBitRate = param.nVideoBitRate ;
			ven.nVideoFps = param.nVideoFps;
			ven.nVideoH = param.nVideoH;
			ven.nVideoW = param.nVideoW;
			ven.ph264Encoder = new CH264Encoder();
			if(ven.ph264Encoder)
			{
				ven.ph264Encoder->h264_SetParam(ven.nVideoW,ven.nVideoH,ven.nVideoBitRate,ven.nVideoFps);
				if(m_iSrcVideoWidth > 0 && m_iSrcVideoHeight > 0
					&&ven.ph264Encoder->h264_EncodeInit(m_iSrcVideoWidth,m_iSrcVideoHeight))
				{
					m_listVE.push_back(ven);
					venTemp = ven;
				}
				else
				{
					delete ven.ph264Encoder;
					ven.ph264Encoder = NULL;
					return false;
				}
			}
		}
	}
	m_vLock.Lock();
	m_vCurExcEncoder = venTemp;
	m_vLock.Unlock();
	return true;
}


bool CPublishUnit::getMediaMsgHeader(RTMPPacket& packet,bool bIsSend)
{
	if(m_rtmpImpl == NULL  || packet.m_body == NULL)
		return false;
	packet.m_nChannel = 0x04;  
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;  
	packet.m_nTimeStamp = 0;  
	packet.m_nInfoField2 = m_rtmpImpl->m_stream_id;   

	packet.m_hasAbsTimestamp = 0;  
	unsigned char* szBodyBuffer = (unsigned char*)packet.m_body;
	char * szTmp = packet.m_body;  
	packet.m_packetType = RTMP_PACKET_TYPE_INFO;  

	if(m_vCurExcEncoder.ph264Encoder)
	{
		int nVHSize = 0;
		m_vCurExcEncoder.ph264Encoder->h264_copyHeaderParam((unsigned char*)szTmp,nVHSize);
		szTmp += nVHSize;
	}
	///////////////////////////////////////////////////////////////////////////////

	if(m_aCurExcEncoder.paacEncoder)
	{
		int nAHSize = 0;
		m_aCurExcEncoder.paacEncoder->aac_copyHeaderParam((unsigned char*)szTmp,nAHSize,m_aCurExcEncoder.bAudioKaraoke);
		szTmp +=  nAHSize;
	}
	///////////////////////////////////////////////////////////////////////////////
	szTmp=put_amf_string( szTmp, "");  
	szTmp=put_byte( szTmp, AMF_OBJECT_END ); 
	packet.m_nBodySize=szTmp-(char *)szBodyBuffer; 
	return true;
}

bool CPublishUnit::getVideoHeader(RTMPPacket& packet,bool bIsSend)
{
	if((m_rtmpImpl == NULL && m_udpImpl == NULL ) || packet.m_body == NULL)
		return false;

	packet.m_nChannel = 0x04;  
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;  
	packet.m_nTimeStamp = 0;  
	if(m_rtmpImpl)
		packet.m_nInfoField2 = m_rtmpImpl->m_stream_id;  
	else if(m_udpImpl)
		packet.m_nInfoField2 = 1;  
	else 
		return false;

	packet.m_hasAbsTimestamp = 0;  
	unsigned char* szBodyBuffer = (unsigned char*)packet.m_body;
	char * szTmp = packet.m_body;  
	packet.m_packetType = RTMP_PACKET_TYPE_INFO; 

	if(m_vCurExcEncoder.ph264Encoder)
	{
		packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
		packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;  
		szBodyBuffer[0]=0x17;  
		szBodyBuffer[1]=0x00;  
		szBodyBuffer[2]=0x00;  
		szBodyBuffer[3]=0x00;  
		szBodyBuffer[4]=0x00;  
		szBodyBuffer[5]=0x01;  
		szBodyBuffer[6]=0x42;  
		szBodyBuffer[7]=0xC0;  
		szBodyBuffer[8]=0x15;  
		szBodyBuffer[9]=0x03;  
		szBodyBuffer[10]=0x01;  
		szTmp=(char *)szBodyBuffer+11;  
		int nVSSize = 0;
		m_vCurExcEncoder.ph264Encoder->h264_CopyPPSandSPS((unsigned char*)szTmp,nVSSize);
		szTmp += nVSSize;
		packet.m_nBodySize=szTmp-(char *)szBodyBuffer;
		return true;
	}
	return false;
}

bool CPublishUnit::getAudioHeader(RTMPPacket& packet,bool bIsSend)
{
	if(m_aCurExcEncoder.paacEncoder)
	{
		if((m_rtmpImpl == NULL && m_udpImpl == NULL ) || packet.m_body == NULL)
			return false;

		if(m_rtmpImpl)
			packet.m_nInfoField2 = m_rtmpImpl->m_stream_id;  
		else if(m_udpImpl)
			packet.m_nInfoField2 = 1;  
		else 
			return false;

		packet.m_nChannel = 0x04;  
		packet.m_headerType = RTMP_PACKET_SIZE_LARGE;  
		packet.m_nTimeStamp = 0;  

		packet.m_hasAbsTimestamp = 0; 
		packet.m_packetType = RTMP_PACKET_TYPE_AUDIO; 

		char hdr[4] ;
		if(m_aCurExcEncoder.nAudioSmpRateCap == 16000)
		{
			hdr[0] = 0xAF;
			hdr[1] = 0x00;
			hdr[2] = 0x14;
			hdr[3] = 0x08;
		}
		else if(m_aCurExcEncoder.nAudioSmpRateCap == 32000)
		{
			hdr[0] = 0xAF;
			hdr[1] = 0x00;
			hdr[2] = 0x12;
			hdr[3] = 0x90;
		}
		memcpy(packet.m_body,hdr, 4);
		packet.m_nBodySize	= 4;
		return true;
	}
	return false;
}

void CPublishUnit::CheckBPL(int nMaxW,int nMaxH)
{
	if(m_BPL.nW == 0 || m_BPL.nH == 0)
	{
		m_BPL.nW = nMaxW;
		m_BPL.nH = nMaxH;
		m_BPL.nBufferSize = nMaxW * nMaxH * 3;
		m_BPL.pBuffer = (unsigned char*) malloc(m_BPL.nBufferSize);
		memset(m_BPL.pBuffer,0,m_BPL.nBufferSize);
	}
	else
	{
		if(m_BPL.nBufferSize != nMaxW * nMaxH * 3)
		{
			m_BPL.nW = nMaxW;
			m_BPL.nH = nMaxH;
			m_BPL.nBufferSize = nMaxW * nMaxH * 3;
			if(m_BPL.pBuffer)
			{
				free(m_BPL.pBuffer);
				m_BPL.pBuffer = NULL;
			}
			m_BPL.pBuffer = (unsigned char*) malloc(m_BPL.nBufferSize);
		}
		memset(m_BPL.pBuffer,0,m_BPL.nBufferSize);
	}

	if(m_szRGBBuffer == NULL)
	{
		m_szRGBBuffer = new unsigned char[m_BPL.nBufferSize]; 
		memset(m_szRGBBuffer,0,m_BPL.nBufferSize); 
	}
	else
	{
		if(m_nRGBBufSize < m_BPL.nBufferSize)
		{
			if(m_szRGBBuffer)
			{
				free(m_szRGBBuffer);
				m_szRGBBuffer = NULL;
			}
			if(m_szRGBBuffer == NULL)
			{
				m_szRGBBuffer = (unsigned char*)malloc(m_BPL.nBufferSize);
				m_nRGBBufSize = m_BPL.nBufferSize;
				memset(m_szRGBBuffer,0,m_nRGBBufSize); 
			}
		}
	}
}

bool CPublishUnit::bIsGetImageTime()
{
	int nDiff = 0;
	DWORD nCurTime = GetTickCount();
	if(m_nLastGetTime == 0)
	{
		m_nLastGetTime = nCurTime;
		m_nNextGetTime = m_nVideoSpan;
	}
	else
	{
		nDiff = nCurTime- m_nLastGetTime;

		if(nDiff > m_nNextGetTime)
		{
			m_nLastGetTime = nCurTime;
			int nTemp = (m_nVideoSpan - (nDiff - m_nVideoSpan));
			m_nNextGetTime =  abs(nTemp - m_nVideoSpan) > 10 ? m_nVideoSpan : nTemp ;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CPublishUnit::getVideoData(RTMPPacket& packet)
{

	if(m_listVideoSour.size() > 0 && packet.m_body && bIsGetImageTime())
	{
		unsigned int nCameraBufferSize = 0;
		unsigned char *pDesBuf = NULL;
		unsigned int  nDesBufSize = 0;
		unsigned int  nBufSize = DATA_SIZE - 18;

		int nMaxW = 0;
		int nMaxH = 0;

		list<PicLayer> listPL;
		list<PicLayer>::iterator pliter;
		list<PicSourceNode>::iterator iter;
		PicSourceNode snDraw;

		char* szBodyBuffer = packet.m_body;
		packet.m_headerType = RTMP_PACKET_SIZE_LARGE; 
		packet.m_nTimeStamp = 0;
		packet.m_hasAbsTimestamp = 0;
		packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
		szBodyBuffer[0]=0x17;  
		szBodyBuffer[1]=0x01;  
		szBodyBuffer[2]=0x00;  
		szBodyBuffer[3]=0x00;  
		szBodyBuffer[4]=0x42; 
		if(m_listVideoSour.size() > 1)
		{
			m_picLock.Lock();
			for(iter = m_listVideoSour.begin();iter != m_listVideoSour.end();iter++)
			{
				PicLayer pl;
				PicSourceNode sn = (*iter);
				if(sn.pSD && sn.pSD->GetBuffer(&pl.pBuffer,pl.nBufferSize))
				{
					if(pl.pBuffer != NULL)
					{
						ScRect temprc;
						ScRect showRect;
						snDraw = sn;
						sn.pSD->GetVideoWAndH(pl.nW,pl.nH);
						sn.pSD->GetShowHwndRect(showRect);
						int nShowW = abs(showRect.nRight - showRect.nLeft);
						int nShowH = abs(showRect.nBottom - showRect.nTop);
						if(nShowW != 0 || nShowH != 0)
						{
							if(sn.rc.nTop == 0 && sn.rc.nRight == 0 && sn.rc.nLeft == 0 && sn.rc.nBottom == 0)
							{
								pl.rc.nTop = 0;
								pl.rc.nRight = (pl.nW *1.0/m_iSrcVideoWidth) * nShowW;
								pl.rc.nBottom = (pl.nH * 1.0/ m_iSrcVideoHeight) * nShowH;
								pl.rc.nLeft = 0;
								sn.rc = (*iter).rc = pl.rc;
							}

							float fltWRate = 1.0;
							float fltHRate = 1.0;
							if(nShowW > 0)
							{
								fltWRate = m_iSrcVideoWidth * 1.0 / nShowW ;
							}
							if(nShowH > 0)
							{
								fltHRate = m_iSrcVideoHeight * 1.0 / nShowH;
							}

							temprc.nLeft = (sn.rc.nLeft - showRect.nLeft) * fltWRate > 0 ? (sn.rc.nLeft - showRect.nLeft) * fltWRate : 0;
							temprc.nRight = (sn.rc.nRight - showRect.nLeft) * fltWRate > m_iSrcVideoWidth ? m_iSrcVideoWidth: ((sn.rc.nRight - showRect.nLeft) * fltWRate);
							temprc.nTop = (sn.rc.nTop - showRect.nTop) * fltHRate  > 0 ? (sn.rc.nTop - showRect.nTop) * fltHRate : 0;
							temprc.nBottom = (sn.rc.nBottom - showRect.nTop) * fltHRate > m_iSrcVideoHeight ? m_iSrcVideoHeight : (sn.rc.nBottom - showRect.nTop) * fltHRate;

							int nTemp = (temprc.nBottom - temprc.nTop) ;
							temprc.nTop = abs(temprc.nTop - m_iSrcVideoHeight) - nTemp;
							temprc.nBottom = temprc.nTop + nTemp;

							pl.rc = temprc;
						}
						else
						{
							pl.rc.nTop = 0;
							pl.rc.nLeft = 0;
							pl.rc.nRight = pl.nW;
							pl.rc.nBottom = pl.nH;
						}
						listPL.push_back(pl);
					}
				}
			}
			m_picLock.Unlock();
			if(listPL.size() <= 0 )
				return false;

			listPL.sort(cmp());

			int nMaxW = m_iSrcVideoWidth;
			int nMaxH = m_iSrcVideoHeight;

			//for(pliter = listPL.begin(); pliter != listPL.end();pliter++)
			//{
			//	PicLayer pl = (*pliter);
			//	nMaxW = pl.nW > nMaxW ? pl.nW:nMaxW;
			//	nMaxH = pl.nH > nMaxH ? pl.nH:nMaxH;
			//}

			CheckBPL(nMaxW,nMaxH);
		
			while(listPL.size()  > 0)
			{
				PicLayer pl = listPL.front();
				listPL.pop_front();
				ZoomBitMap(pl.pBuffer,m_BPL.pBuffer,pl.nW,pl.nH,m_BPL.nW,m_BPL.nH,pl.rc);
			}

			pDesBuf = m_BPL.pBuffer;
			nDesBufSize = m_BPL.nBufferSize;
		}
		else
		{

			m_picLock.Lock();
			for(iter = m_listVideoSour.begin();iter != m_listVideoSour.end();iter++)
			{
				PicSourceNode sn = (*iter);
				if(sn.pSD && sn.pSD->GetBuffer(&pDesBuf,nDesBufSize))
				{
					sn.pSD->GetVideoWAndH(nMaxW,nMaxH);
					CheckBPL(nMaxW,nMaxH);
					nDesBufSize = m_BPL.nBufferSize;
				//	if(m_pShowEventCallBack == NULL)
					memcpy(m_BPL.pBuffer,pDesBuf,nDesBufSize);

				//	ProcessImage(pDesBuf,nMaxW,nMaxH,m_vCurExcEncoder.nVideoW,m_vCurExcEncoder.nVideoH);

					if(m_pShowEventCallBack)
					{
						ShowVideoParam svp;
						svp.nImageWidth = nMaxW;
						svp.nImageHeight = nMaxH;
						svp.hShowHwnd = sn.pSD->GetShowHand();
						m_pShowEventCallBack(EVENT_SHOW,m_EShowventpUser,m_BPL.pBuffer,m_BPL.nBufferSize,&svp);
					}
					else
					{
						sn.pSD->ShowImage(m_BPL.pBuffer,m_BPL.nBufferSize,nMaxW,nMaxH);
					}
					break;
				}
			}
			m_picLock.Unlock();
		}

		if(pDesBuf && nDesBufSize > 0 )
		{
			if(m_bIsTakePhotos)
			{
				m_bIsTakePhotos = false;
				if(m_pEventCallBack)
				{
					TakePhptpsParam tpp;
					tpp.nImageWidth = nMaxW;
					tpp.nImageHeight = nMaxH;
					m_pEventCallBack(TAKE_PHOTOS,m_EventpUser,pDesBuf,nDesBufSize,&tpp);
				}
			}

			m_vLock.Lock();
			if(!m_vCurExcEncoder.ph264Encoder->h264_Encode((unsigned char*)szBodyBuffer,nBufSize,m_szRGBBuffer,pDesBuf,nMaxW,nMaxH))
			{
				m_vLock.Unlock();
				return false;
			}				
			m_vLock.Unlock();
			packet.m_nBodySize = nBufSize;
			return true;
		}
	}
	return false;
}

void CPublishUnit::ProcessImage(unsigned char* pInBuf,int nSrcW,int nSrcH,int nDestW,int nDestH)
{
	if(pInBuf == NULL)
		return;

	int nSrcRGBWidthStep = nSrcW * nSrcH;
	int nDestRGBWidthStep = nDestW * nDestH;

	if(nSrcW != nDestW || nSrcH != nDestH)
	{
		if(m_swscontext == NULL)
			m_swscontext = def_ff_sws_getContext(nSrcW, nSrcH, AV_PIX_FMT_BGR24, nDestW, nDestH, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
		uint8_t *pSrcBuff[3] = {pInBuf,pInBuf + nSrcRGBWidthStep,pInBuf+ nSrcRGBWidthStep * 2};
		uint8_t *pDstBuff[3] = {m_szRGBBuffer,m_szRGBBuffer + nDestRGBWidthStep ,m_szRGBBuffer+nDestRGBWidthStep*2};

		int nSrcStride[3];
		int nDstStride[3];

		for (int i=0; i<3; i++)
		{
			nSrcStride[i] = nSrcW * 3 ;
			nDstStride[i] = nDestW * 3;
		}

		if(m_swscontext)
		{
			def_ff_sws_scale(m_swscontext,pSrcBuff,nSrcStride, 0, nSrcH,pDstBuff, nDstStride);;
		}
		memcpy(pInBuf,m_szRGBBuffer,nDestRGBWidthStep*3);
		//翻转buffer
		for(int k=0;k<nDestH;k++)  
		{
			memcpy(pInBuf+(nDestW*3)*(nDestH-k-1),m_szRGBBuffer+(nDestW*3)*k,nDestW*3); 
		}
	}
	else
	{
		unsigned char szTemp[3] = {0};
		int nHLen = 0;
		int nWlen = 0;
		int nWstep = nDestW * 3;
		int nAllLen = (nDestH - 1) * nWstep ;
		unsigned char cTemp = 0;
		unsigned char* p = NULL;
		int nPos = 0;
		for(int k=0;k< nDestH; k++) 
		{
			nWlen = 0;
			for(int i = 0; i<nDestW ;i+= 2)
			{
				nPos = nHLen + nWlen;
				p = pInBuf + nPos;
				cTemp = *(p+2);
				*(p+2) = *(p);
				*(p) = cTemp;

				cTemp = *(p+5);
				*(p+5) = *(p+3);
				*(p+3) = cTemp;
				nWlen += 6;
			}

			memcpy(m_szRGBBuffer+nAllLen,pInBuf+ nHLen,nWstep); 
			nHLen += nWstep;
			nAllLen -= nWstep;
		}
		memcpy(pInBuf,m_szRGBBuffer,nDestRGBWidthStep*3);
	}
	

}


void CPublishUnit::ZoomBitMap(unsigned char *pSrcImg, unsigned char *pDstImg, int nWidth, int nHeight, int nDesW,int nDstH,ScRect rc)  
{  
   int i = 0;  
   int j = 0;  
  
   float fRateW = (rc.nRight - rc.nLeft) * 1.0/ nWidth;
   float fRateH = (rc.nBottom - rc.nTop) * 1.0/ nHeight;

   float fX, fY;  
  
   int iStepSrcImg = nWidth;  
   int iStepDstImg = nWidth * fRateW;  
  
   int iX, iY;    
   unsigned char bTL, bTR, bBL, bBR;  
   unsigned char gTL, gTR, gBL, gBR;  
   unsigned char rTL, rTR, rBL, rBR;  
 
   unsigned char b, g, r;  
  if((fabs(fRateW - 1.0) <= 0.0000001 ) && (fabs(fRateH - 1.0) < 0.0000001))
  {
	  	for(int i = 0;i<nHeight;i++)
		{
			int nB = ((rc.nTop+i)* nDesW*3)+rc.nLeft*3;
			memcpy(pDstImg + nB,pSrcImg+i*nWidth*3,nWidth*3);
		}
  }
  else
  {
	   for(i = 0; i < nHeight * fRateH; i++)  
	   {  
			for(j = 0; j < nWidth * fRateW; j++)  
			{  
				fX = ((float)j) /fRateW;  
				fY = ((float)i) /fRateH;  
	  
				iX = (int)fX;  
				iY = (int)fY;  

				float fU = fX - iX;
				float fV = fY - iY;

				int nL = iX >= nWidth ? nWidth-1 : iX ;
				int nR = iX + 1 >= nWidth ? nWidth - 1  : iX + 1;
				int nT = iY >= nHeight ? nHeight -1 : iY ;
				int nB = iY + 1 >= nHeight ? nHeight-1 : iY + 1;

				int nTLPos = nT * iStepSrcImg * 3 + nL * 3;
				int nTRPos = nT * iStepSrcImg * 3 + nR * 3;
				int nBLPos = nB * iStepSrcImg * 3 + nL * 3;
				int nBRPos = nB * iStepSrcImg * 3 + nR * 3;
				
				//b
				bTL  = pSrcImg[nTLPos + 0];  
				bTR = pSrcImg[nTRPos + 0]; 
				bBL  = pSrcImg[nBLPos + 0];  
				bBR = pSrcImg[nBRPos + 0];  
				
				//g
				gTL  = pSrcImg[nTLPos + 1];  
				gTR = pSrcImg[nTRPos + 1];  
				gBL  = pSrcImg[nBLPos + 1];  
				gBR = pSrcImg[nBRPos + 1];  
	  
				//r
				rTL  = pSrcImg[nTLPos + 2];  
				rTR = pSrcImg[nTRPos + 2];  
				rBL  = pSrcImg[nBLPos + 2];  
				rBR = pSrcImg[nBRPos + 2];  
	  
				b = bTL * ( 1 - fU) * (1 - fV) + bTR * fU * (1 - fV)  + bBL * (1 - fU) * fV  + bBR * fU * fV ;
	  
				g = gTL * ( 1 - fU) * (1 - fV) + gTR  * fU * (1 - fV) + gBL * (1 - fU) * fV  + gBR * fU * fV ;  
	  
				r = rTL * ( 1 - fU) * (1 - fV) + rTR * fU * (1 - fV)  + rBL * (1 - fU) * fV  + rBR * fU * fV;  
	 
				if((rc.nLeft + j < nDesW) && (rc.nTop+i< nDstH))
				{
					int nBPos = ((rc.nTop+i)* nDesW*3)+rc.nLeft*3;

					pDstImg[nBPos + j * 3 + 0] = b;
					pDstImg[nBPos + j * 3 + 1] = g;
					pDstImg[nBPos + j * 3 + 2] = r;
				}
				else
					break;
			}  
		}  
    }
} 

bool CPublishUnit::getAudioData(RTMPPacket& packet)
{
	if(InterlockedExchange(&m_isCapturing,m_isCapturing))
	{
		CSourAudio * pAudio = NULL;
		list<ISourData*>::iterator iter;
		packet.m_nChannel = 0x04;    
		packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
		packet.m_nTimeStamp = 0;
		packet.m_hasAbsTimestamp = 0; 
		packet.m_packetType = RTMP_PACKET_TYPE_AUDIO; 
		unsigned char* szTemp = (unsigned char*)packet.m_body; 
		szTemp[0]  = 0xAF;
		szTemp[1]  = 0x01;// Raw AAC

		m_audLock.Lock();
		for(iter = m_listAudioSour.begin();iter != m_listAudioSour.end();iter++)
		{
			pAudio = (CSourAudio *)(*iter);
			if( pAudio && pAudio->GetActiveing())
			{
				m_aLock.Lock();
				if(m_aCurExcEncoder.paacEncoder)
				{
					unsigned int nBufSize = m_nAACInputSamples;
					bool bGetBuf = false;
					if((m_nStreamType&SOURCESCREEN) == SOURCESCREEN){
						bGetBuf = pAudio->GetBufferEx(&m_pbPCMBuffer,nBufSize);
					}else {
						bGetBuf = pAudio->GetBuffer(&m_pbPCMBuffer,nBufSize);
					}

					if(bGetBuf)
					{
						m_nMicVolum = CalculateMicVolum(m_pbPCMBuffer, nBufSize);
						//AAC encode
						BYTE* pbAACBuffer = NULL;
						unsigned int  m_outPutAAcSize = 0;
						if(m_aCurExcEncoder.paacEncoder->aac_Encode(&pbAACBuffer,m_outPutAAcSize,m_pbPCMBuffer,m_nAACInputSamples))
						{
							if(m_outPutAAcSize > 0)
							{
								char szbuf[256] = {'/0'};
								sprintf(szbuf,"aac size ****************************** %d\n",m_outPutAAcSize);
							//	OutputDebugString(CString(szbuf));
								memcpy(szTemp+2,pbAACBuffer+7,m_outPutAAcSize-7);
								packet.m_nBodySize =  m_outPutAAcSize -7 + 2;
								m_aLock.Unlock();
								m_audLock.Unlock();
								return true;
							}
						}
						else
						{
							OutputDebugString(CString("aac encoder pcm to aac error!\n"));
						}
					}
				}
				else
				{
					OutputDebugString(CString("aac encoder not exist !\n"));
				}
				m_aLock.Unlock();	
			}
		}
		m_audLock.Unlock();
	}
	return false;
}

int CPublishUnit::CalculateMicVolum(unsigned char* pBuf,unsigned int nBufSize)
{
	int nVol = 0;

	for(int i = 0; i<nBufSize>>5; i++)
		nVol += abs((short)(pBuf[i*32] + (pBuf[32*i+1]<<8))); // every one sample each 32 sample pcm is 16bits

	nVol = (nVol/nBufSize)<<1; // nMicVolum*4/(nBufSize>>1)/256
	nVol = nVol>0?nVol:0;
	nVol = nVol<128?nVol:128;

	return nVol;
}


void CPublishUnit::send_proc()
{
	while(!m_send_media_thread.GetStop())
	{
		bool bIsVideo = false;
		SendMediaPackect smp;

		m_sendLock.Lock();
		int nSendListSize  = m_sendAudioList.size();
		if(nSendListSize  > 0)
		{
			smp = m_sendAudioList.front();
			m_sendAudioList.pop_front();
		}
		else
		{
			nSendListSize  = m_sendVideoList.size();
			if(nSendListSize  > 0)
			{
				bIsVideo = true;
				smp = m_sendVideoList.front();
				m_sendVideoList.pop_front();
			}
		}
		m_sendLock.Unlock();

		if(smp.packet.m_body)
		{
			int res = 0;
			if(m_rtmpImpl && RTMP_IsConnected(m_rtmpImpl))
			{
				res = RTMP_SendPacket(m_rtmpImpl, &smp.packet, smp.nQueue);
			}

			if(smp.packet.m_body != NULL)
			{
				RTMPPacket_Free(&smp.packet);
				smp.packet.m_body = NULL;
			}
		}
		m_sendCond.cond_wait();
	}
}

void CPublishUnit::clearSendList()
{
	m_sendLock.Lock();
	if(m_sendVideoList.size() > 0)
	{
		list<SendMediaPackect>::iterator itersend;
		for(itersend = m_sendVideoList.begin(); itersend != m_sendVideoList.end();itersend++)
		{
			SendMediaPackect smp = (*itersend);
			if(smp.packet.m_body)
			{
				RTMPPacket_Free(&smp.packet);
				smp.packet.m_body = NULL;
			}
		}
	}
	m_sendVideoList.clear();
	if(m_sendAudioList.size() > 0)
	{
		list<SendMediaPackect>::iterator itersend;
		for(itersend = m_sendAudioList.begin(); itersend != m_sendAudioList.end();itersend++)
		{
			SendMediaPackect smp = (*itersend);
			if(smp.packet.m_body)
			{
				RTMPPacket_Free(&smp.packet);
				smp.packet.m_body = NULL;
			}
		}
	}
	m_sendAudioList.clear();
	
	m_sendLock.Unlock();
}

bool CPublishUnit::SetEventCallback(scEventCallBack pEC,void* dwUser)
{
	m_pEventCallBack = pEC;
	m_EventpUser = dwUser;
	return true;
}

bool CPublishUnit::SetShowEventCallback(scEventCallBack pEC,void* dwUser)
{
	m_pShowEventCallBack = pEC;
	m_EShowventpUser = dwUser;
	return true;
}


void CPublishUnit::TakePhotos()
{
	m_bIsTakePhotos = true;
}
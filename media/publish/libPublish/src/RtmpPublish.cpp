
// ScreenRecoderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "rtmppublish.h"
#include "TimeUtils.h"
#include "loadffmpeg.h"

//#include "openssl/md5.h"
//#include "openssl/bio.h"
//#include "openssl/evp.h"
//#include "openssl/buffer.h"

#pragma comment(lib, "zlib.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int InitSockets()  
{  
	WORD version;  
	WSADATA wsaData;  
	version = MAKEWORD(1, 1);  
	return (WSAStartup(version, &wsaData) == 0);    
}  
void CleanupSockets()  
{  
	WSACleanup();  
}  


static void CALLBACK TimerCallBack(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{

      CRTMPPublish * pThis=(CRTMPPublish*)dwUser;//由this指针获得实例的指针
      pThis->OnVideoTimer(1);//调用要回调的成员方法

}

void  TimerReadAudioCallBack(void* pUser)
{
	CRTMPPublish * pThis=(CRTMPPublish*)pUser;//由this指针获得实例的指针
	pThis->OnAudioTimer(1);//调用要回调的成员方法
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CRTMPPublish::CRTMPPublish( HWND hShowHwnd)
{

	m_nTimer      = 0;
	m_hVideoEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	m_hAudioEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	RTMPPacket_Reset(&m_packetVideo);
	m_packetVideo.m_body = NULL;
	RTMPPacket_Reset(&m_packetAudio);
	m_packetAudio.m_body = NULL;

	m_hSendAudioThread = NULL;
	m_isAudioThreadRunning = 0;
	m_listPushFile.clear();
	m_saveFile = NULL;
	m_szPlayFileName[0] = '\0';

	m_pEventCallBack = NULL;
	m_EventpUser = NULL;	
	m_listPublish.clear();

	m_pShowEventCallBack = NULL;
	m_EShowventpUser = NULL;

	m_InitImpl = NULL;
	m_iStartTime  = 0;
	m_audioPublish = NULL;
	m_bIsPreview = false;
	m_trancode = NULL;
	m_bIsTeancodeing = false;
	if(m_packetVideo.m_body == NULL)
	{
		RTMPPacket_Alloc(&m_packetVideo,DATA_SIZE);
	}
	if(m_packetAudio.m_body == NULL)
	{
		RTMPPacket_Alloc(&m_packetAudio,DATA_SIZE);
	}

	if(m_InitImpl == NULL)
	{
		m_InitImpl= RTMP_Alloc();  
		RTMP_Init(m_InitImpl);
	}
	m_playAudio = NULL;
	m_nMicVolum = 0;
	OnInit();
}

CRTMPPublish::~CRTMPPublish()
{
	OnUnInit();
	if(m_InitImpl)
	{
		RTMP_Free(m_InitImpl);
 		m_InitImpl = NULL;
	}

	list <CPublishUnit*>::iterator piter;
	m_vLock.Lock();
	for(piter = m_listPublish.begin();piter != m_listPublish.end();piter++)
	{
		CPublishUnit* pu = (*piter);
		if(pu)
		{
			m_aLock.Lock();
			if(pu == m_audioPublish)
			{
				m_audioPublish = NULL;
			}
			m_aLock.Unlock();
			pu->PublishEnd(true);
			delete pu;
			pu = NULL;
		}
	}
	m_listPublish.clear();
	m_vLock.Unlock();

	m_aLock.Lock();
	if(m_audioPublish)
	{
		delete m_audioPublish;
		m_audioPublish = NULL;
	}
	m_aLock.Unlock();


	if(m_saveFile)
	{
		m_saveFile->PushDataToLocalFlvFileEnd();
		delete m_saveFile;
		m_saveFile = NULL;
	}

	if (NULL != m_hVideoEvent)
	{
		CloseHandle(m_hVideoEvent);
		m_hVideoEvent = NULL;
	}

	if(NULL != m_hAudioEvent)
	{
		CloseHandle(m_hAudioEvent);
		m_hAudioEvent = NULL;
	}

	list<PushFileNode>::iterator pfiter = m_listPushFile.begin();
	for(;pfiter != m_listPushFile.end();pfiter++)
	{
		PushFileNode pfn = (*pfiter);
		if(pfn.pPlaySourFile)
		{
			pfn.pPlaySourFile->close_file_and_uninit();
			delete pfn.pPlaySourFile;
			pfn.pPlaySourFile = NULL;
		}
	}
	m_listPushFile.clear();
	if(m_packetVideo.m_body != NULL)
	{
		RTMPPacket_Free(&m_packetVideo);
		m_packetVideo.m_body = NULL;
	}
	if(m_packetAudio.m_body != NULL)
	{
		RTMPPacket_Free(&m_packetAudio);
		m_packetAudio.m_body = NULL;
	}
	SDL_Quit();
	if(m_trancode)
	{
		delete m_trancode;
		m_trancode = NULL;
	}
}


bool CRTMPPublish::OnInit()
{
	CFFMPEGProxy::CreateFFMPEGProxy();
	HRESULT hr = CoInitialize(NULL);
	bool ret = InitSockets();

	m_nAudioTimer.StartTimer(10,TimerReadAudioCallBack,this);
	m_nTimer = timeSetEvent(10,1,&TimerCallBack,(DWORD)this,1);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CRTMPPublish::OnUnInit()
{	
	WaitForSingleObject(m_hAudioEvent,INFINITE);
	WaitForSingleObject(m_hVideoEvent,INFINITE);
	if(m_nTimer)
	{
		timeKillEvent(m_nTimer);
		m_nTimer = 0;
	}
	m_nAudioTimer.StopTimer();

	CleanupSockets();
	CoUninitialize();
	CFFMPEGProxy::DestoryFFMPEGProxy();
}

//获取本地摄像头数目
long   CRTMPPublish::GetDevCameraCount()
{
	return m_sdManager.GetDevCameraCount();;
}

//获取摄像头名称
bool  CRTMPPublish::GetDevCameraName(long nIndex,char szName[256])
{
	return m_sdManager.GetDevCameraName(nIndex,szName);
}

//功能:获取当前设备麦克风数目
long   CRTMPPublish::GetDevMicCount()
{
	return m_sdManager.GetDevMicCount();
}

bool CRTMPPublish::GetDevMicName(int nMicID,char szName[256])
{
	return m_sdManager.GetDevMicName(nMicID,szName);
}

long CRTMPPublish::GetDevSpeakerCount()
{
	return m_sdManager.GetDevSpeakerCount();
}

bool CRTMPPublish:: GetDevSpeakerName(int nSpeakerID,char szName[256])
{
	return m_sdManager.GetDevSpeakerName(nSpeakerID,szName);
}

bool   CRTMPPublish::SetDevSpeaker(int index)
{
	return m_sdManager.SetDevSpeaker(index);
}

//功能：摄像头插入拔出消息
bool   CRTMPPublish::DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID)
{
	return m_sdManager.DeviceChangeMsg(nDeviceType,nDeviceState,nDeviceName,DeviceID);
}


CPublishUnit* CRTMPPublish::FindPU(const char* szURL)
{
	CPublishUnit* pu = NULL;
	if(m_listPublish.size() > 0)
	{
		list <CPublishUnit*>::iterator piter;
		for(piter = m_listPublish.begin();piter != m_listPublish.end();piter++)
		{
		    pu = (*piter);
			if(strcmp(pu->getPublishAddr(),szURL) == 0)
			{
				break;
			}
			pu = NULL;
		}
	}
	return pu;
}

CPublishUnit* CRTMPPublish::FindPUAndRemove(const char* szURL)
{
	CPublishUnit* pu = NULL;
	if(m_listPublish.size() > 0)
	{
		m_vLock.Lock();
		list <CPublishUnit*>::iterator piter;
		for(piter = m_listPublish.begin();piter != m_listPublish.end();piter++)
		{
		    pu = (*piter);
			if(strcmp(pu->getPublishAddr(),szURL) == 0)
			{
				m_listPublish.erase(piter);
				break;
			}
			pu = NULL;
		}
		m_vLock.Unlock();
	}
	return pu;
}

bool CRTMPPublish::PushStart(const char* szUrl,int nStreamType,StreamParam param)
{
	OutputDebugString(CString(szUrl));
	OutputDebugStringA("\n");

	bool bIsExist = true;
	CPublishUnit* pu = FindPU(szUrl);
	if(pu == NULL)
	{
		pu = new CPublishUnit(&m_sdManager);
		bIsExist = false;
	
	}

	if(m_pShowEventCallBack)
	{
		pu->SetShowEventCallback(m_pShowEventCallBack,m_EShowventpUser);
	}

	if(!pu->PublishStart(szUrl,nStreamType,param))
	{
		delete pu;
		return false;
	}

	if(param.bIsPublish)
	{
		m_vLock.Lock();
		SendMediaHeader(pu,true);
		m_vLock.Unlock();
	}

	if(!bIsExist)
	{
		int nType = VIDEOTYPE;
		if(pu->bIsExistMediaType(nType))
		{
			m_vLock.Lock();
			m_listPublish.push_back(pu);
			m_vLock.Unlock();
		}
		nType = AUDIOTYPE;
		if(pu->bIsExistMediaType(nType))
		{
			m_aLock.Lock();
			m_audioPublish = pu;
			m_aLock.Unlock();
		}
	}


	return true;
}

bool CRTMPPublish::PushStartEx(const char* szVideoUrl,const char* szAudioUrl,int nStreamType,StreamParam param)
{
	OutputDebugString(CString(szVideoUrl));
	OutputDebugStringA("\n");
	OutputDebugString(CString(szAudioUrl));
	OutputDebugStringA("\n");

	bool bIsVideoExist = true;
	bool bIsAudioExist = true;
	CPublishUnit* pvu = FindPU(szVideoUrl);
	CPublishUnit* pau = NULL;
	int nTempType = 0;
	if(pvu == NULL)
	{
		pvu = new CPublishUnit(&m_sdManager);
		bIsVideoExist = false;
	}

	if(m_pShowEventCallBack)
	{
		pvu->SetShowEventCallback(m_pShowEventCallBack,m_EShowventpUser);
	}

	nTempType = nStreamType & (SOURCECAMERA | SOURCESCREEN);
	if(!pvu->PublishStart(szVideoUrl,nTempType,param))
	{
		delete pvu;

		return false;
	}
	
	nTempType = nStreamType & (SOURCEDEVAUDIO | SOURCEDEMUSIC);
	if(strcmp(szAudioUrl,"") != 0)
	{
		if((nTempType & AUDIOTYPE) == AUDIOTYPE )
		{
			if(pau == NULL)
			{
				pau = new CPublishUnit(&m_sdManager);
				bIsAudioExist = false;
			}

			if(!pau->PublishStart(szAudioUrl,nTempType,param))
			{
				delete pvu;
				delete pau;
								
				return false;
			}

			m_aLock.Lock();
			m_audioPublish = pau;
			m_aLock.Unlock();
		}
	}
	else
	{
		if((nTempType & AUDIOTYPE) == AUDIOTYPE )
		{
			m_aLock.Lock();
			m_audioPublish = pvu;
			m_aLock.Unlock();
		}
	}

	m_vLock.Lock();
	if(param.bIsPublish)
	{
		SendMediaHeader(pvu,true);

		if(pau && pau != pvu)
		{
			SendMediaHeader(pau,true);
		}
	}

	if(!bIsVideoExist)
	{
		m_listPublish.push_back(pvu);
	}
	m_vLock.Unlock();
		
	return true;
}


bool CRTMPPublish::PushChangeEx(const char* szVideoUrl,const char* szAudioUrl,int nStreamType,StreamParam param)
{

	CPublishUnit* pvu = FindPU(szVideoUrl);

	if(pvu)
	{
		m_vLock.Lock();
		int nTempType = nStreamType & (SOURCECAMERA | SOURCESCREEN);

		if(m_pShowEventCallBack)
		{
			pvu->SetShowEventCallback(m_pShowEventCallBack,m_EShowventpUser);
		}
		if(!pvu->PublishChanage(szVideoUrl,nTempType,param))
		{
			m_vLock.Unlock();
			return false;
		}
		else
		{
			SendMediaHeader(pvu);
		}
		m_vLock.Unlock();
	}

	bool bIsAudioChange = false;
	int nAudioType = nStreamType & (SOURCEDEVAUDIO | SOURCEDEMUSIC);
	if((nAudioType & AUDIOTYPE) == AUDIOTYPE)
	{
		if(m_audioPublish  == NULL)
		{
			m_aLock.Lock();
			int nTempType = nStreamType & (SOURCEDEVAUDIO | SOURCEDEMUSIC);
			m_audioPublish  = new CPublishUnit(&m_sdManager);
			if(!m_audioPublish->PublishStart(szAudioUrl,nTempType,param))
			{
				m_aLock.Unlock();
				return false;
			}
			SendMediaHeader(m_audioPublish,true);
			m_aLock.Unlock();
		}
		else
		{
			if(strcmp(m_audioPublish->getPublishAddr(),szAudioUrl) != 0)
			{
				return false;
			}
			else
			{
				m_aLock.Lock();
				int nType = AUDIOTYPE;
				int nTempType = nStreamType & (SOURCEDEVAUDIO | SOURCEDEMUSIC);
				if(!m_audioPublish->bIsExistMediaType(nType))
				{
					if(!m_audioPublish->PublishStart(szAudioUrl,nTempType,param))
					{
						m_aLock.Unlock();
						return false;
					}
				}
				else
				{
					
					if(!m_audioPublish->PublishChanage(szAudioUrl,nTempType,param))
					{
						m_aLock.Unlock();
						return false;
					}
					else
					{
						SendMediaHeader(m_audioPublish);
					}
				}
				m_aLock.Unlock();
			}
		}
	}
	else
	{
		m_aLock.Lock();
		if(m_audioPublish)
		{
			
			m_audioPublish->PublishEnd(false);
			m_audioPublish = NULL;
		}
		m_aLock.Unlock();
	}
	
	return true;
}

bool   CRTMPPublish::moveImageShowRect(const char* szRtmpPushUrl,int nIndex,int nStreamType,ScRect showRect)
{
	CPublishUnit* pu = FindPU(szRtmpPushUrl);
	if(pu)
	{
		return pu->setShowRect(nIndex,nStreamType,showRect);
	}
	return false;
}

bool CRTMPPublish::PushChange(const char* szUrl,int nStreamType,StreamParam param)
{
	CPublishUnit* pu = FindPU(szUrl);
	if(pu)
	{
		m_vLock.Lock();

		bool ret = pu->PublishChanage(szUrl,nStreamType,param);
		if(ret)
		{
			SendMediaHeader(pu);
			m_vLock.Unlock();

			bool bAudio = pu->bIsExistMediaType(AUDIOTYPE);
			m_aLock.Lock();
			if(bAudio) {
				m_audioPublish = pu;
			}

			m_aLock.Unlock();

			return true;
		}

		m_vLock.Unlock();
	}

	m_aLock.Lock();
	if(m_audioPublish && strcmp(m_audioPublish->getPublishAddr(),szUrl) == 0)
	{
		if(m_audioPublish->PublishChanage(szUrl,nStreamType,param))
		{
			SendMediaHeader(m_audioPublish);
			m_aLock.Unlock();
			return true;
		}
	}
	m_aLock.Unlock();
	return false;
}


bool CRTMPPublish::PushEnd(const char* szUrl,bool bIsColseDev)
{
	OutputDebugStringA(szUrl);
	OutputDebugStringA("\n");

	CPublishUnit* pu = FindPUAndRemove(szUrl);
	bool bIsOK = false;
	if(pu)
	{

		if(m_audioPublish == pu)
		{
			m_aLock.Lock();
			m_audioPublish = NULL;
			m_aLock.Unlock();
		}
		bIsOK = pu->PublishEnd(bIsColseDev);
		delete pu;
		pu = NULL;
	}

	if(m_audioPublish && strcmp(m_audioPublish->getPublishAddr(),szUrl) == 0)
	{
		m_aLock.Lock();
		m_audioPublish->PublishEnd(bIsColseDev);
        if(pu == NULL)
            delete m_audioPublish;
		m_audioPublish = NULL;
		m_aLock.Unlock();
	}
	return true;
}

void CRTMPPublish::SendMediaHeader(CPublishUnit *pu,bool bIsSend)
{
	if(pu)
	{
		pu->clearSendList();

		if(pu->getMediaMsgHeader(m_packetVideo,bIsSend))
		{
				RTMP_SendPacket_Ex(pu,&m_packetVideo,0,0);
		}
		if(pu->getVideoHeader(m_packetVideo,bIsSend))
		{
				RTMP_SendPacket_Ex(pu,&m_packetVideo,0,0);
		}
		if(pu->getAudioHeader(m_packetVideo,bIsSend))
		{
				RTMP_SendPacket_Ex(pu,&m_packetVideo,0,0);
		}
	}
}

bool   CRTMPPublish::TakePhotos(const char* szUrl)
{
	list <CPublishUnit*>::iterator piter;
	for(piter = m_listPublish.begin();piter != m_listPublish.end();piter++)
	{
		CPublishUnit *pu = (*piter);
		if(strcmp(pu->getPublishAddr(),szUrl) == 0)
		{
			if(m_pEventCallBack)
			{
				pu->SetEventCallback(m_pEventCallBack,m_EventpUser);
				pu->TakePhotos();
				return true;
			}
			break;
		}
	}
	return false;
}

void CRTMPPublish::OnVideoTimer(UINT_PTR nIDEvent)
{
	if(m_listPublish.size() > 0)	
	{
		ResetEvent(m_hVideoEvent);
		list <CPublishUnit*>::iterator piter;
		m_vLock.Lock();
		for(piter = m_listPublish.begin();piter != m_listPublish.end();piter++)
		{
			CPublishUnit *pu = (*piter);
			int nType = VIDEOTYPE;
			if(pu && pu->bIsExistMediaType(nType) && m_packetVideo.m_body)
			{
				if(pu->getVideoData(m_packetVideo))
				{
					RTMP_SendPacket_Ex(pu,&m_packetVideo,0,0);
				}
			}

			nType = AUDIOTYPE;
			if(pu && pu->bIsExistMediaType(nType) && m_packetAudio.m_body)
			{
				if(pu->getAudioData(m_packetAudio))
				{
					RTMP_SendPacket_Ex(pu,&m_packetAudio,0,0,false);
				}
			}
		}

		m_vLock.Unlock();
		SetEvent(m_hVideoEvent);
	}
}

void CRTMPPublish::OnAudioTimer(UINT_PTR nIDEvent)
{
	ResetEvent(m_hAudioEvent);
	/*
	int nType = AUDIOTYPE;
	m_aLock.Lock();
	if(m_audioPublish && m_audioPublish->bIsExistMediaType(AUDIOTYPE) && m_packetAudio.m_body)
	{
		if(m_audioPublish->getAudioData(m_packetAudio))
		{
			if(m_audioPublish->getNetType() == RTMPTYPE)
			{
				RTMP_SendPacket_Ex(m_audioPublish,&m_packetAudio,0,0,false);
			}
			else if(m_audioPublish->getNetType() == UDPTYPE)
			{
				int nRe = UDP_SendPacket_Ex(m_audioPublish,(unsigned char*)m_packetAudio.m_body,m_packetAudio.m_nBodySize);
				if(nRe == 0)
				{
					OutputDebugString(CString("udp send audio packet error \n"));
				}
			}
		}
	}
	m_aLock.Unlock();
	*/

	PreviewSoundEx();

	SetEvent(m_hAudioEvent);
}

int CRTMPPublish::CalculateMicVolum(unsigned char* pBuf,unsigned int nBufSize)
{
	int nVol = 0;

	for(int i = 0; i<nBufSize>>5; i++)
		nVol += abs((short)(pBuf[i*32] + (pBuf[32*i+1]<<8))); // every one sample each 32 sample pcm is 16bits

	nVol = (nVol/nBufSize)<<1; // nMicVolum*4/(nBufSize>>1)/256
	nVol = nVol>0?nVol:0;
	nVol = nVol<128?nVol:128;

	return nVol;
}

void CRTMPPublish::PreviewSoundEx()
{
	if(m_bIsPreview)
	{
		CSourAudio * pAudio = NULL;
		pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIAMICAUDIO,0);
		if(pAudio)
		{
			static unsigned char* pBuf = NULL;
			if(pBuf == NULL)
			{
				pBuf = new unsigned char[5120];
			}
			unsigned int nBufsize = 5120;
			unsigned int  nSample;
			unsigned int nChannel;
			if(pAudio->GetPreviewBufferEx(&pBuf,nBufsize,nSample,nChannel) && pBuf)
			{
				if(m_playAudio == NULL)
					m_playAudio = new CPlayAudio();

				m_nMicVolum = CalculateMicVolum(pBuf,nBufsize);
				m_playAudio->PlaySoundByDS(pBuf,nBufsize,1,nSample);
			}
		}
	}
	else
	{
		if(m_playAudio)
		{
			m_playAudio->StopSound();
			delete m_playAudio;
			m_playAudio = NULL;
		}
	}
}

int CRTMPPublish::UDP_SendPacket_Ex(CPublishUnit *pu,unsigned char* pBuf,unsigned nBufSize)
{
	CLock::CAutoLock lock(m_lock);
	if(m_iStartTime == 0)
	{
		m_iStartTime = ::GetTickCount();
	}
	DWORD nTimeStamp = ::GetTickCount() - m_iStartTime;
	if(pu)
	{
		return pu->UDP_SendPacket(pBuf,nBufSize,nTimeStamp);
	}
	return 0;
}

int CRTMPPublish::RTMP_SendPacket_Ex(CPublishUnit *pu, RTMPPacket *packet, int queue,int nUserType,bool bIsVideo)
{
	
	if(pu == NULL || packet == NULL || !pu->getPublishStatus() )
		return 0;

	CLock::CAutoLock lock(m_lock);


	if(packet->m_nBodySize > 0 && pu->RTMP_IsConnectedEx())
	{
		if(m_iStartTime == 0)
		{
			m_iStartTime = ::GetTickCount();
		}
		packet->m_nTimeStamp = ::GetTickCount() - m_iStartTime;

		SendMediaPackect smp;
		smp.nFrameType = nUserType;
		smp.nQueue = queue;

		memcpy(&smp.packet,packet,sizeof(RTMPPacket));
		if( bIsVideo)
		{
			int nSize = smp.packet.m_nBodySize > 131072 ? smp.packet.m_nBodySize : 131072;
			RTMPPacket_Alloc(&smp.packet,nSize);
			memcpy(smp.packet.m_body,packet->m_body,packet->m_nBodySize);
			smp.packet.m_nBodySize = packet->m_nBodySize;
		}
		else
		{
			int nSize = smp.packet.m_nBodySize > 8192 ? smp.packet.m_nBodySize : 8192;
			RTMPPacket_Alloc(&smp.packet,nSize);
			memcpy(smp.packet.m_body,packet->m_body,packet->m_nBodySize);
			smp.packet.m_nBodySize = packet->m_nBodySize;
		}

		pu->RTMP_SendPacketEx(smp,queue,bIsVideo);
	}

	if(!pu->RTMP_IsConnectedEx())
	{
		char szbuf[256] = {'/0'};
		sprintf(szbuf,"rtmp reconnect *************************\n");
		OutputDebugString(CString(szbuf));

		if(pu->RTMP_ReConnect(true))
		{
			SendMediaHeader(pu);
		}
	}
	return 1;
}

bool CRTMPPublish::PushFileToServerBegin(const char* szLocalFile,char* szServerURL,int nUseDrawToolType,HWND hShowHwnd)
{
	
	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szLocalFile))
		{
			return false;
		}
	}
	
	PushFileNode newnode;
	strcpy(newnode.szFileName,szLocalFile);
	if(hShowHwnd)
	{
		newnode.hPlayWnd = hShowHwnd;
	}
	else
		return false;
	newnode.pPlaySourFile =  new CSourFile(newnode.hPlayWnd,nUseDrawToolType);

	if(!newnode.pPlaySourFile->open_file_and_init(szLocalFile,szServerURL))
	{
		delete newnode.pPlaySourFile;
		newnode.pPlaySourFile = NULL;
		return false;
	}
	m_listPushFile.push_back(newnode);

	int nLen = strlen(szLocalFile)+1 > 512 ? 512:strlen(szLocalFile)+1;
	strncpy(m_szPlayFileName,szLocalFile,nLen);

	return true;
}

bool CRTMPPublish::PushFileToServerEnd(const char* szLocalFile)
{
	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szLocalFile))
		{
			if(pfn.pPlaySourFile)
			{
				pfn.pPlaySourFile->close_file_and_uninit();
				delete pfn.pPlaySourFile;
				pfn.pPlaySourFile = NULL;
				m_listPushFile.erase(iter);
				return true;
			}
		}
	}
	
	return false;
}


bool CRTMPPublish::PushFileSwitch(const char* szCurPlayLocalFileName)
{
	if(strcmp(m_szPlayFileName,szCurPlayLocalFileName) == 0)
		return false;

	int nLen = strlen(szCurPlayLocalFileName)+1 > 512 ? 512:strlen(szCurPlayLocalFileName)+1;
	strncpy(m_szPlayFileName,szCurPlayLocalFileName,nLen);

	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	PushFileNode pfnSwitch;
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szCurPlayLocalFileName))
		{
			pfnSwitch = (*iter);
		}
		else
		{
			pfn.pPlaySourFile->pause_in_file(true);
			pfn.pPlaySourFile->switch_paly();
		}
	}

	if(pfnSwitch.pPlaySourFile)
	{
		bool bIsOK = pfnSwitch.pPlaySourFile->switch_paly(true);
		if(bIsOK)
			return pfnSwitch.pPlaySourFile->pause_in_file(false);
	}

	return false;
}

bool CRTMPPublish::PushFileSeek(const char* szLocalFile,unsigned int nSeekTime)
{
	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szLocalFile))
		{
			if(pfn.pPlaySourFile)
			{
				return pfn.pPlaySourFile->seek_in_file(nSeekTime);
			}
		}
	}
	return false;
}

bool CRTMPPublish::PushFilePause(const char* szLocalFile,bool bIsPause)
{
	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szLocalFile))
		{
			if(pfn.pPlaySourFile)
			{
				return pfn.pPlaySourFile->pause_in_file(bIsPause);
			}
		}
	}
	return false;
}

unsigned int CRTMPPublish::GetPushFileCurPlayTime(const char* szLocalFile)
{
	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szLocalFile))
		{
			if(pfn.pPlaySourFile)
			{
				return pfn.pPlaySourFile->get_in_file_current_play_time();
			}
		}
	}
	return 0;
}

unsigned int CRTMPPublish::GetPushFileDuration(const char* szLocalFile)
{
	list<PushFileNode>::iterator iter = m_listPushFile.begin();
	for(;iter != m_listPushFile.end();iter++)
	{
		PushFileNode pfn = (*iter);
		if(pfn.cmpFileIsPushing(szLocalFile))
		{
			if(pfn.pPlaySourFile)
			{
				return pfn.pPlaySourFile->get_in_file_duration();
			}
		}
	}
	return 0;
}

inline bool IsXPHigher()
{
	static bool bIsXPHigher = false;
	static bool bIsInited = false;

	// 取得当前系统版本
	OSVERSIONINFO VersionInfo = {0};
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);

	// 判断并返回
	bIsXPHigher = VersionInfo.dwMajorVersion > 5;
	bIsInited = TRUE;
	return bIsXPHigher;
}

bool CRTMPPublish::PushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow)
{	
	if(!IsXPHigher())
		return false;

	if(m_saveFile == NULL)
		m_saveFile = new CSaveFile();

	CSourAudio * pAudio = NULL;
	pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIASPEKAUDIO,0);
	if(m_saveFile && pAudio)
		return m_saveFile->PushDataToLocalFlvFileBegin(szLocalFile,hRecordWindow,pAudio);
	return false;
}

bool CRTMPPublish::PushDataToLocalFlvFileEnd()
{
	if(m_saveFile)
		return m_saveFile->PushDataToLocalFlvFileEnd();
	return false;
}

bool CRTMPPublish::setSpeakersMute(bool vol) //静音
{
	if(!m_volMixer.SetMute(SPEAKERS,vol))
		return m_volMixer.SetMute(WAVEOUT,vol);
	return true;
}

bool CRTMPPublish::setSpeakersVolume(long vol)//音量
{
	if(!m_volMixer.SetVolume(SPEAKERS,vol))
		return m_volMixer.SetVolume(WAVEOUT,vol);
	return true;
}

long CRTMPPublish::getSpeakersVolume()
{
	long vol = 0;
	if((vol = m_volMixer.GetVolume(SPEAKERS)) == 0)
		vol = m_volMixer.GetVolume(WAVEOUT);
	return vol;
}

bool CRTMPPublish::getMicMute()
{
	CSourAudio * pAudio = NULL;
	pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIAMICAUDIO,0);
	if(pAudio)
		return pAudio->getMicMute();
	return false;
}

bool CRTMPPublish::setMicMute(bool vol)
{
	CSourAudio * pAudio = NULL;
	pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIAMICAUDIO,0);
	if(pAudio)
		return pAudio->setMicMute(vol);
	return false;
}

bool CRTMPPublish::setMicVolume(long vol)
{
	CSourAudio * pAudio = NULL;
	pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIAMICAUDIO,0);
	if(pAudio)
		return pAudio->setMicVolume(vol);
	return false;
}

long CRTMPPublish::getMicVolume()
{
	CSourAudio * pAudio = NULL;
	pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIAMICAUDIO,0);
	if(pAudio)
		return pAudio->getMicVolume();
	return false;
}


//获取麦克风设备的状态
MDevStatus CRTMPPublish::getMicDevStatus()
{
	if(GetDevMicCount() > 0)
	{

		if( m_audioPublish )
		{
			int nStreamType = m_audioPublish->getStreamType();
			int nTemp = nStreamType & SOURCEDEVAUDIO;
			if(nTemp ==  SOURCEDEVAUDIO)
			{
				if(getMicMute())
					return MUTE;
				return NORMAL;
			}
		}
		else
			return MICFORBID;
	}
	else
		return NODEV;
	return NODEV;
}

//获取麦克风状态
MDevStatus CRTMPPublish::getCamDevStatus()
{
	if(GetDevCameraCount() > 0)
	{
		if( m_listPublish.size() > 0)
		{
			list <CPublishUnit*>::iterator iter;
			for(iter = m_listPublish.begin(); iter != m_listPublish.end();iter++)
			{
				CPublishUnit* pPublish = *iter;

				if(pPublish)
				{
					int nStreamType = pPublish->getStreamType();
					int nTemp = nStreamType & SOURCECAMERA;
					if(nTemp == SOURCECAMERA)
						return NORMAL;
				}
			}
			return CAMFORBID;
		}
		else
			return CAMFORBID;
	}
	else
		return NODEV;
}

//获取扬声器状态
MDevStatus CRTMPPublish::getSpeakersDevStatus()
{
	if(m_volMixer.GetMute(SPEAKERS))
		return MUTE;
	else
		return NORMAL;
}

bool CRTMPPublish::SetEventCallback(scEventCallBack pEC,void* dwUser)
{
	m_pEventCallBack = pEC;
	m_EventpUser = dwUser;
	return true;
}

bool CRTMPPublish::SetShowEventCallback(scEventCallBack pEC,void* dwUser)
{
	m_pShowEventCallBack = pEC;
	m_EShowventpUser = dwUser;
	return true;
}

int CRTMPPublish::GetMicRTVolum() 
{
	if(m_bIsPreview)
	{
		return m_nMicVolum;
	}
	if( m_audioPublish )
		return m_audioPublish->GetMicRTVolum();
	return 0;
}


bool CRTMPPublish::PreviewSound(int nMicID,bool bIsPreview)
{
	CSourAudio * pAudio = NULL;
	pAudio = (CSourAudio *)m_sdManager.getSourData(MEDIAMICAUDIO,0);
	if(pAudio)
	{
		pAudio->setPreviewFalg(bIsPreview);
		pAudio->SourPreviewOpen(nMicID);
	}
	m_bIsPreview = bIsPreview;
	return true;
}

bool CRTMPPublish::TrancodeFile(const char* szSourFileName,const char* szDestFileName,char* szErr)
{
	if(m_bIsTeancodeing)
		return false;
	if(m_trancode == NULL)
	{
		m_trancode = new CTranscode();
	}
	if(m_trancode)
	{
		m_trancode->SetEventCallback(m_pEventCallBack, m_EventpUser);
		return m_trancode->Transcoding(szSourFileName,szDestFileName,szErr);
	}

    return false;
}

bool CRTMPPublish::StopTrancodeFile()
{
	m_bIsTeancodeing = false;
	if(m_trancode)
	{
		bool bIsResult = m_trancode->stopTrancoding();
		delete m_trancode;
		m_trancode = NULL;
		return bIsResult;
	}

    return false;
}
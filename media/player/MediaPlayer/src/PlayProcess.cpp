#include "stdafx.h"
#include "flvHeader.h"
#include "MediaPlayer.h"
#include "PlayProcess.h"

CPlayProcess * CPlayProcess:: g_PlayrArray[MAXPLAYERNUM];
CMutexLock   CPlayProcess::m_PlayrArrayLock;
int    CPlayProcess::m_nRefCount = 0;
 UINT_PTR	CPlayProcess::m_nPlayAudioTimer = 0;

 bool IsXPHigher()
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

static void CALLBACK TimerPlayCallBack(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{

	  CPlayProcess::m_PlayrArrayLock.Lock();
	  for(int i = 0;i < MAXPLAYERNUM ;i++)
	  {
		 if(CPlayProcess::g_PlayrArray[i])//由this指针获得实例的指针
			CPlayProcess::g_PlayrArray[i]->OnPlayTimer(uTimerID);//调用要回调的成员方法
	  }
	  CPlayProcess::m_PlayrArrayLock.Unlock();
}

void CALLBACK _TimerReadMediaCallBack(void* dwUser)
{

      ViedePlayNode * pThis=(ViedePlayNode*)dwUser;//由this指针获得实例的指针
	  if( pThis && pThis->pPlayProcess)
	  {
		  pThis->pPlayProcess->OnReadMediaDataTimer(pThis->pVideoPlayUnit);//调用要回调的成员方法
	  }

}

void CALLBACK _TimerReadAudioCallBack(void* dwUser)
{

      CPlayProcess * pThis=(CPlayProcess*)dwUser;//由this指针获得实例的指针
	
      pThis->OnReadAudioDataTimer(1);//调用要回调的成员方法

}


CPlayProcess::CPlayProcess()
{
	m_szPlayAudioURL[0] = '\0';

	m_bIsSeparate = true;

	m_netAudioProcess = NULL;
	m_isVideoPlaying = 0;
	m_isAudioPlaying = 0;

	m_pFlvAudioParseBuf = NULL;
	m_nFlvAudioParseBufSize = 512 * 1024;
	m_FlvAudioParsePos = 0;

	m_nFlvAudioBufSize = 1 * 1024;
	m_pFlvAudioBuf = (unsigned char*)malloc(m_nFlvAudioBufSize);

	m_samplerate = 32000;
	m_nchannel = 1;

	m_nPlayAudioTime = 0;
	m_pCallback = NULL;

	m_bIsHaveAudio = false;
	m_bIsOpenAudio = false;

	m_nLastPlayPcmSysTime = 0;
	m_nLastStopPcmSysTime = 0;
	m_begPlayAudioSysTime = 0;
	m_begPlayAudoPtsTime = 0;
	m_nCurPlayAudoPtsTime = 0;
	m_nPlayAudioSysDiffTime = 0;

	m_pcmHighbuffer.setConfig(true);
	m_playSound = NULL;
	m_playAudio = NULL;
	m_bIsPlayLocal = false;
	m_playVideoUnit.clear();			
}

CPlayProcess::~CPlayProcess()
{
	StopALLMedia();

	if(m_netAudioProcess)
	{
		CPlayVideoUnit::DestoryNetProcess(m_netAudioProcess);
		m_netAudioProcess = NULL;
	}

	if(m_pFlvAudioBuf)
	{
		free(m_pFlvAudioBuf);
		m_pFlvAudioBuf = NULL;
	}

	if(m_pFlvAudioParseBuf)
	{
		free(m_pFlvAudioParseBuf);
		m_pFlvAudioParseBuf = NULL;
		m_FlvAudioParsePos = 0;
	}
}

bool CPlayProcess::initWasAudioPlay()
{
	if(!IsXPHigher())
	{
		return false;
	}

	do 
	{
		if(NULL != m_playAudio)
		{
			break;
		}

		IWasVoiceEngine * wasEngine = CreateWasVoiceEngine();
		if(NULL==wasEngine)
		{
			break;
		}

		int defSpk = wasEngine->getDefaultSpk();
		if(defSpk < 0)
		{
			break;
		}

		m_playAudio = wasEngine->getSpkAudioImpl(defSpk);
	} while (false);

	if(NULL == m_playAudio)
	{
		return false;
	}

	if(m_playAudio->isOpen())
	{
		return true;
	}

	m_playAudio->setParam(m_nchannel,16,m_samplerate);

	return m_playAudio->open();
}

bool  CPlayProcess::PlayFile(const char* szURL,void* hPlayHand,void* hBGbitmap)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	bool bIsExit = true;
	if(pvu == NULL)
	{
		pvu = new ViedePlayNode();
		pvu->pVideoPlayUnit = new CPlayVideoUnit(static_SyncAudio,this);
		pvu->pPlayProcess = this;
		ADDPVUnit(pvu);
		m_bIsPlayLocal = true;
	}
	return pvu->pVideoPlayUnit->PlayFile(szURL,hPlayHand,hBGbitmap,fill_audio,this,m_pCallback);
}

bool CPlayProcess::getPalyStutas(const char* szURL)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->getPalyStutas();
	}
	return false;
}

bool CPlayProcess::bIsHaveVideo(const char* szURL)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->bIsHaveVideo();
	}
	return false;
}

bool CPlayProcess::PauseFile(const char* szURL,bool bIsPause)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->PauseFile(bIsPause);
	}
	return false;
}

bool CPlayProcess::SeekFile(const char* szURL,unsigned int nPalyPos)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->SeekFile(nPalyPos);
	}
	return false;
}

bool CPlayProcess::SeekFileStream(const char* szURL,unsigned int nPlayPos,bool bVideo)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->SeekFileStream(nPlayPos,bVideo);
	}
	return false;
}

bool CPlayProcess::SwitchPaly(const char* szURL,bool bIsFlag)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->SwitchPaly(fill_audio,this,bIsFlag);
	}
	return false;
}

unsigned int CPlayProcess::GetFileDuration(const char* szURL)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->GetFileDuration();
	}
	return 0;
}

unsigned int CPlayProcess::getFileCurPlayTime(const char* szURL)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->getFileCurPlayTime();
	}
	return 0;
}

unsigned int CPlayProcess::getFileStreamCurTime(const char* szURL,bool bVideo)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->getFileStreamCurTime(bVideo);
	}
	return 0;
}

void CPlayProcess::stopFile(const char* szURL)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		return pvu->pVideoPlayUnit->stopFile();
	}
}


ViedePlayNode* CPlayProcess::findPVUnit(const char* szURL)
{
	ViedePlayNode* vpn = NULL;
	if(szURL)
	{
		m_playVideoLock.Lock();
		list<ViedePlayNode*>::iterator iter;
		for(iter = m_playVideoUnit.begin();iter != m_playVideoUnit.end();iter++)
		{
			vpn = (*iter);
			if(vpn && vpn->pVideoPlayUnit && strcmp(szURL,vpn->pVideoPlayUnit->getPalyAddress()) == 0)
			{
				break;
			}
			else
			{
				vpn = NULL;
			}
		}
		m_playVideoLock.Unlock();
	}
	return vpn;
}

ViedePlayNode* CPlayProcess::RemovePVUnit(const char* szURL)
{
	ViedePlayNode* pPVU = NULL;
	if(szURL)
	{
		m_playVideoLock.Lock();
		list<ViedePlayNode*>::iterator iter;
		for(iter = m_playVideoUnit.begin();iter != m_playVideoUnit.end();iter++)
		{
			pPVU = (*iter);
			if(pPVU && pPVU->pVideoPlayUnit && strcmp(szURL,pPVU->pVideoPlayUnit ->getPalyAddress()) == 0)
			{
				m_playVideoUnit.erase(iter);
				break;
			}
			else
			{
				pPVU = NULL;
			}
		}
		m_playVideoLock.Unlock();
	}
	return pPVU;
}


void  CPlayProcess::ADDPVUnit(ViedePlayNode* pvu)
{
	if(pvu)
	{
		m_playVideoLock.Lock();
		m_playVideoUnit.push_back(pvu);
		m_playVideoLock.Unlock();
	}
}

void CPlayProcess::OnReadMediaDataTimer(void * pUser)
{
	CPlayVideoUnit* puv = (CPlayVideoUnit* )pUser;
	if(puv)
	{
		while(!puv->m_threadReadMedia.GetStop())
		{
			if(puv->m_pFlvBuf)
			{
				int nReadSize = puv->ReadMediaData();
				if(nReadSize > 0)
				{
					ParseFlvParam node;
					node.audJitterBuf = &m_audJitterBuf;
					node.pPVU = puv;
					if(CPlayVideoUnit::flvParse(&puv->m_pFlvParseBuf,puv->m_nFlvParseBufSize,puv->m_FlvParsePos,puv->m_pFlvBuf,nReadSize,node) == 1)
					{
						puv->netStopCommond();
					}

					if(node.bIsAudio && node.bIsAAC)
					{
						m_samplerate = node.samplerate;
						m_nchannel = node.nchannel;
					}

					while(!m_bIsSeparate && DecodePCMDatatoAAC(false));
				}
				else
				{
					Sleep(100);
				}
			}
			else
			{
				Sleep(10);
			}
		}
	}
}

void CPlayProcess::OnReadAudioDataTimer(int nEventID)
{
	while(!m_threadReadAudio.GetStop())
	{	
		if(m_bIsOpenAudio && m_netAudioProcess  && m_pFlvAudioBuf)
		{
			int nReadSize = m_netAudioProcess->ReadMedia(m_pFlvAudioBuf,m_nFlvAudioBufSize);
			if(nReadSize > 0)
			{
				ParseFlvParam node;
				node.audJitterBuf = &m_audJitterBuf;
				node.pPVU = NULL;
				if(CPlayVideoUnit::flvParse(&m_pFlvAudioParseBuf,m_nFlvAudioParseBufSize,m_FlvAudioParsePos,m_pFlvAudioBuf,nReadSize,node) == 1)
				{
					if(m_bIsOpenAudio&&m_netAudioProcess)
					{
						moudle_commond_t cmd = STOP;
						m_netAudioProcess->SetCommond(cmd,NULL);
					}
				}
				if(m_bIsOpenAudio)
					while(DecodePCMDatatoAAC(false));
				else
				{
					m_audJitterBuf.reset();
				}
			}
			else
			{
				Sleep(100);
			}
		}
		else
		{
			Sleep(10);
		}
	}
}

bool  CPlayProcess::DecodePCMDatatoAAC(bool bIsWaitFlag)
{
	AACAudioFrame aaf;
	AudioFrame     af;
	aaf.nSamplerate = m_samplerate;
	aaf.nChannel = m_nchannel;
	
	if(!m_audJitterBuf.GetStatus())
	{
		return false;
	}

	int nr = m_audJitterBuf.Pop((char**)&aaf.pFrame,aaf.nFrameSize,(unsigned int&)aaf.pts,bIsWaitFlag);
	if(nr!=0 || NULL == aaf.pFrame || 0 >= aaf.nFrameSize)
	{
		return false;
	}

	af.nPCMBufSize = 0;
	af.nSamplerate = aaf.nSamplerate;
	af.nChannel = aaf.nChannel;
	af.pPCMBuffer = (unsigned char*)malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE * 2);
	
	if(NULL==af.pPCMBuffer)
	{
		return false;
	}

	bool br = m_decoder.DecodeAACAudio(aaf.pFrame,aaf.nFrameSize,aaf.pts,af);
	if(!br || 0 >= af.nPCMBufSize)
	{
		free(af.pPCMBuffer);
		return false;
	}
	
	m_PlayAudioLock.Lock();

	if(m_nPlayAudioTime == 0)
	{
		m_nPlayAudioTime = aaf.pts;
		m_begPlayAudoPtsTime = aaf.pts;
	}

	if(aaf.nSamplerate == 16000)
	{
		if(aaf.nSamplerate != m_pcmLowbuffer.GetSsamplerate())
		{
			StopAudio();

			m_nPlayAudioTime = aaf.pts;
			m_begPlayAudoPtsTime = aaf.pts;
		}

		m_pcmLowbuffer.ReSetConfig(m_audJitterBuf.GetAvgRecvTime(),m_audJitterBuf.GetMaxTime());
		m_pcmLowbuffer.WritePCMData(af.pPCMBuffer,af.nPCMBufSize,af.nSamplerate,af.nChannel);
		m_nCurPlayAudoPtsTime = aaf.pts - m_begPlayAudoPtsTime - m_pcmLowbuffer.GetCacheDataTime();
		
		if(abs((int)(m_nPlayAudioSysDiffTime - m_nCurPlayAudoPtsTime)) > 300)
		{
			unsigned int playcCurTime = aaf.pts - m_pcmLowbuffer.GetCacheDataTime() > 0 ? aaf.pts - m_pcmLowbuffer.GetCacheDataTime() : 0;
			m_nPlayAudioTime = playcCurTime;
			m_begPlayAudoPtsTime = playcCurTime;
		}
	}
	else if(aaf.nSamplerate == 32000)
	{
		if(aaf.nSamplerate != m_pcmHighbuffer.GetSsamplerate())
		{
			StopAudio();
			m_nPlayAudioTime = aaf.pts;
			m_begPlayAudoPtsTime = aaf.pts;
		}

		m_pcmHighbuffer.ReSetConfig(m_audJitterBuf.GetAvgRecvTime(),m_audJitterBuf.GetMaxTime());
		m_pcmHighbuffer.WritePCMData(af.pPCMBuffer,af.nPCMBufSize,af.nSamplerate,af.nChannel);
		m_nCurPlayAudoPtsTime = aaf.pts - m_begPlayAudoPtsTime - m_pcmHighbuffer.GetCacheDataTime();
		
		if(abs((int)(m_nPlayAudioSysDiffTime - m_nCurPlayAudoPtsTime)) > 300)
		{
			unsigned int playcCurTime = aaf.pts - m_pcmHighbuffer.GetCacheDataTime() > 0 ? aaf.pts - m_pcmHighbuffer.GetCacheDataTime() : 0;
			m_nPlayAudioTime = playcCurTime;
			m_begPlayAudoPtsTime = playcCurTime;
		}
	}
		
	m_PlayAudioLock.Unlock();
	free(af.pPCMBuffer);
	return true;
}

void CPlayProcess::static_SyncAudio(int64_t & nPlayAudioTime,bool& nHaveAudio,void* dwUser)
{
	CPlayProcess * pro = (CPlayProcess*)dwUser;
	if(pro)
	{
		pro->SyncAudio(nPlayAudioTime,nHaveAudio);
	}
}

void CPlayProcess::SyncAudio(int64_t & nPlayAudioTime,bool& nHaveAudio)
{
	nPlayAudioTime = m_nPlayAudioTime;
	nHaveAudio = m_bIsHaveAudio;
}

void CPlayProcess::OnPlayTimer(int nEventID)//调用要回调的成员方法
{
	if(m_nPlayAudioTime > 0 && (m_pcmLowbuffer.GetCacheStatus() || m_pcmHighbuffer.GetCacheStatus())) //时间戳递增说明有音频数据可以播放
	{
		unsigned char* buf = NULL;
		unsigned int   bufsize = 0;
		unsigned int   xxsize = 0;
		DWORD nCurSysTime = timeGetTime();
		if(m_nLastPlayPcmSysTime == 0)
		{
			m_nLastPlayPcmSysTime = nCurSysTime;
		}

		m_nLastStopPcmSysTime = nCurSysTime;

		if(m_begPlayAudioSysTime == 0)
		{
			m_begPlayAudioSysTime = nCurSysTime;
		}
		DWORD nSysSpan = nCurSysTime - m_nLastPlayPcmSysTime;
		unsigned int nStopTime = 200;
		if(nSysSpan > nStopTime)
		{
			StopAudio();
		}
		else if(nSysSpan >= PCMFRAMETIME)
		{
			unsigned int nAudSpan = 0;
			if(nSysSpan > PCMFRAMETIME * 10)
				nSysSpan = PCMFRAMETIME * 10;

			unsigned int nReadOneSize = PCMFRAMESIZE * m_nchannel * (m_samplerate/SIMPLERATENUM);
			float fltPcmBuffSize = nReadOneSize * 1.0 / PCMFRAMETIME * nSysSpan;
			bufsize = fltPcmBuffSize;
			int nBusySize = 0;
			if(m_samplerate == 16000)
			{
				if(m_pcmHighbuffer.GetBusyPCMDataSize() > 0)
				{
					m_pcmHighbuffer.ClearBuffer();
				}
				m_pcmLowbuffer.ReadPCMData(&buf,bufsize,xxsize,nAudSpan);
				nBusySize = m_pcmLowbuffer.GetBusyPCMDataSize();
			}
			else if(m_samplerate == 32000)
			{
				if(m_pcmLowbuffer.GetBusyPCMDataSize() > 0)
				{
					m_pcmLowbuffer.ClearBuffer();
				}
				m_pcmHighbuffer.ReadPCMData(&buf,bufsize,xxsize,nAudSpan);
				nBusySize = m_pcmHighbuffer.GetBusyPCMDataSize();
			}

			if(buf && bufsize > 0)
			{
				//if(nSysSpan > PCMFRAMETIME)
				//{
				//	char szbuf[256] = {'/0'};
				//	sprintf(szbuf,"read time  %d-----%d size PCM data to cache pcm %d\n",nSysSpan ,bufsize,nBusySize);
				//	OutputDebugString(szbuf);
				//}

				unsigned int nPSize = (nReadOneSize * 1.2) / 16 * 16;
				if(bufsize > nPSize)
				{
					buf = buf + bufsize - nPSize;
					bufsize = nPSize;
					char szbuf[256] = {'/0'};
					//sprintf(szbuf,"read time  %d-----%d size PCM data to cache pcm %d\n",nSysSpan ,bufsize,m_pcmHighbuffer.GetBusyPCMDataSize());
					//OutputDebugString(szbuf);
				}
				m_nLastPlayPcmSysTime = nCurSysTime;
				m_nPlayAudioTime += nSysSpan;
				m_bIsHaveAudio = true;
				m_nPlayAudioSysDiffTime = nCurSysTime - m_begPlayAudioSysTime;


				if(IsXPHigher())
				{
					if(initWasAudioPlay())
					{
						m_playAudio->putRenderData(buf,bufsize);
					}
				}
				else
				{
					if(m_playSound == NULL)
					{
						m_playSound = new CPlayAudio();
					}

					if(m_playSound)
					{
						m_playSound->PlaySoundByDS(buf,bufsize,m_nchannel,m_samplerate);
					}
				}
			}
		}
	}
	else
	{
		DWORD nCurSysTime = timeGetTime();
		DWORD nSysSpan = nCurSysTime - m_nLastStopPcmSysTime;
		unsigned int nStopTime = 500;
		if(nSysSpan > 500)
		{
			m_PlayAudioLock.Lock();
			if(IsXPHigher())
			{
				if(m_playAudio)
				{
					m_playAudio->close();
				}
			}
			else
			{
				if(m_playSound)
				{
					m_playSound->StopSound();
				}
			}
			m_PlayAudioLock.Unlock();
		}
	}
}

void CPlayProcess::StopAudio()
{
	m_PlayAudioLock.Lock();
	m_bIsHaveAudio = false;
	m_pcmLowbuffer.ClearBuffer();
	m_pcmHighbuffer.ClearBuffer();
	m_nLastPlayPcmSysTime = 0;
	m_nPlayAudioTime = 0;
	m_begPlayAudioSysTime = 0;

	if(IsXPHigher())
	{
		if(m_playAudio)
		{
			m_playAudio->close();
			m_playAudio->release();
			m_playAudio = NULL;
		}
	}
	else
	{
		if(m_playSound)
		{
			delete m_playSound;
			m_playSound = NULL;
		}
	}

	m_PlayAudioLock.Unlock();

}

void  CPlayProcess::fill_audio(void *udata,Uint8 *stream,int len)
{   
	CPlayProcess *palyPro = (CPlayProcess *)udata;
	if(palyPro)
	{
		palyPro->SDLPlayAudio(udata,stream,len);
	}
}

void  CPlayProcess::SDLPlayAudio(void *udata,Uint8 *stream,int len)
{
	if(m_playVideoUnit.size())
	{
		ViedePlayNode* pnode = m_playVideoUnit.front();
		if(pnode && pnode->pVideoPlayUnit)
		{
			pnode->pVideoPlayUnit->SDLPlayAudio(udata,stream,len);
		}
	}
}

int  CPlayProcess::GetShowHandCount(const char* szURL)
{
	if(szURL)
	{
		ViedePlayNode* pvu = findPVUnit(szURL);
		if(pvu && pvu->pVideoPlayUnit)
		{
			return pvu->pVideoPlayUnit->GetShowHandCount();
		}
	}
	return 0;
}

void CPlayProcess::SetCallBack(const char* szFileName,IMediaPlayerEvent* pCallback)
{
    if(szFileName)
    {
        ViedePlayNode* pvu = findPVUnit(szFileName);
        if(pvu && pvu->pVideoPlayUnit)
        {
            pvu->pVideoPlayUnit->SetCallBack(pCallback);
        }
    }
	m_pCallback = pCallback;
}

bool  CPlayProcess::ChangeMedia(const char* szURL,int nPlayStreamType,HWNDHANDLE hwnd,HIBITMAP hBGbitmap,bool bIsPlay,long nUserID,bool bIsMainVideo,bool bIsStudent)
{
	int nTempPlayVideo = nPlayStreamType & VIDEOTYPE;
	int nTempPlayAudio = nPlayStreamType & AUDIOTYPE;

	if(bIsStudent)
	{
		m_samplerate = 16000;
		m_nchannel = 1;
	}
	if(nTempPlayVideo == VIDEOTYPE)
	{
		ViedePlayNode* pvu = findPVUnit(szURL);
		if(pvu && pvu->pVideoPlayUnit)
		{
			pvu->pVideoPlayUnit->ChangeMedia(hwnd,hBGbitmap,bIsMainVideo);            
		}
	}

	if(nTempPlayAudio == AUDIOTYPE)
	{
		m_bIsOpenAudio = bIsPlay;
		if(m_bIsOpenAudio)
		{
			if(!InterlockedExchange(&m_isAudioPlaying,m_isAudioPlaying))
			{
				if( nTempPlayVideo != VIDEOTYPE)
				{
					if(m_netAudioProcess == NULL)
					{
						m_netAudioProcess = CPlayVideoUnit::CreateNetProcess(szURL);

					}
					if( m_netAudioProcess )
					{
						m_audJitterBuf.Init();
						moudle_commond_t cmd = OPEN;
						m_netAudioProcess->Activate(szURL,nUserID);
						if(!m_netAudioProcess->SetCommond(cmd,NULL))
						{
							return false;	
						}
						if(m_pFlvAudioParseBuf == NULL)
						{
							m_pFlvAudioParseBuf = (unsigned char*)malloc(m_nFlvAudioParseBufSize);
						}
					}
				}				

				CPlayProcess::m_PlayrArrayLock.Lock();
	
				if(CPlayProcess::m_nPlayAudioTimer == 0 || CPlayProcess::m_nRefCount == 0)
				{
					CPlayProcess::m_nRefCount = 0;
					CPlayProcess::m_nPlayAudioTimer = timeSetEvent(5,1,&TimerPlayCallBack,(DWORD)this,1);
				}
				for(int i = 0; i < MAXPLAYERNUM ; i++)
				{
					if(g_PlayrArray[i] == NULL)
					{
						g_PlayrArray[i] = this;
						CPlayProcess::m_nRefCount++;
						break;
					}
				}
				CPlayProcess::m_PlayrArrayLock.Unlock();
				InterlockedExchange(&m_isAudioPlaying,1);
				if(m_threadReadAudio.GetStop())
					m_threadReadAudio.Begin(_TimerReadAudioCallBack,this);
			}
		}
		else
		{
			if(InterlockedExchange(&m_isAudioPlaying,m_isAudioPlaying))
			{
				if(m_netAudioProcess)
				{
					moudle_commond_t cmd = STOP;
					m_netAudioProcess->SetCommond(cmd,NULL);
					m_audJitterBuf.unInit();
				}
				
				if(CPlayProcess::m_nRefCount == 0 && CPlayProcess::m_nPlayAudioTimer)
				{
					timeKillEvent(CPlayProcess::m_nPlayAudioTimer);
					CPlayProcess::m_nPlayAudioTimer = 0;
				}

				CPlayProcess::m_PlayrArrayLock.Lock();
				for(int i = 0; i < MAXPLAYERNUM ; i++)
				{
					if(g_PlayrArray[i] == this)
					{
						CPlayProcess::m_nRefCount--;
						g_PlayrArray[i] = NULL;
					}
				}

				CPlayProcess::m_PlayrArrayLock.Unlock();

				StopAudio();
				m_FlvAudioParsePos = 0;
				m_pcmLowbuffer.ClearBuffer();
				m_pcmHighbuffer.ClearBuffer();
								
				InterlockedExchange(&m_isAudioPlaying,0);				
			}
		}
	}
	return true;
}

bool  CPlayProcess::PlayMedia(const char* szURL,int nPlayStreamType,void* hPlayHand,void* hBGbitmap,bool bIsPlay,long nUserID,bool bIsMainVideo,bool bIsStudent,IMediaPlayerEvent* pCallback)
{
	int nTempPlayAudio = nPlayStreamType & AUDIOTYPE;
	int nTempPlayVideo = nPlayStreamType & VIDEOTYPE;

	if(bIsStudent)
	{
		m_samplerate = 16000;
		m_nchannel = 1;
	}

	if(nTempPlayVideo == VIDEOTYPE)
	{
		ViedePlayNode* pvu = findPVUnit(szURL);
		bool bIsExit = true;
		if(pvu == NULL)
		{
			pvu = new ViedePlayNode();
			pvu->pVideoPlayUnit = new CPlayVideoUnit(static_SyncAudio,this);
			pvu->pPlayProcess = this;
			bIsExit = false;
		}

		if(nTempPlayAudio == AUDIOTYPE)
		{
			m_audJitterBuf.Init();
			m_bIsSeparate = false;
		}

		pvu->pVideoPlayUnit->PlayMedia(szURL,hPlayHand,hBGbitmap,_TimerReadMediaCallBack,pvu,bIsMainVideo,pCallback);
		
		if( !bIsExit )
			ADDPVUnit(pvu);

	}

	if(nTempPlayAudio == AUDIOTYPE )
	{
		if(!InterlockedExchange(&m_isAudioPlaying,m_isAudioPlaying))
		{
			m_bIsOpenAudio = bIsPlay;
			if(m_bIsOpenAudio)
			{
				if( nTempPlayVideo != VIDEOTYPE)
				{
					if(m_netAudioProcess == NULL)
					{
						m_netAudioProcess = CPlayVideoUnit::CreateNetProcess(szURL);
					
					}
					if( m_netAudioProcess )
					{
						m_audJitterBuf.Init();
						moudle_commond_t cmd = OPEN;
						m_netAudioProcess->Activate(szURL,nUserID);
						if(!m_netAudioProcess->SetCommond(cmd,NULL))
						{
							return false;	
						}
						if(m_pFlvAudioParseBuf == NULL)
						{
							m_pFlvAudioParseBuf = (unsigned char*)malloc(m_nFlvAudioParseBufSize);
						}
					}
				}

				CPlayProcess::m_PlayrArrayLock.Lock();
	
				if(CPlayProcess::m_nPlayAudioTimer == 0 || CPlayProcess::m_nRefCount == 0)
				{
					CPlayProcess::m_nRefCount = 0;
					CPlayProcess::m_nPlayAudioTimer = timeSetEvent(5,1,&TimerPlayCallBack,(DWORD)this,1);
				}
				for(int i = 0; i < MAXPLAYERNUM ; i++)
				{
					if(g_PlayrArray[i] == NULL)
					{
						g_PlayrArray[i] = this;
						CPlayProcess::m_nRefCount++;
						break;
					}
				}
				CPlayProcess::m_PlayrArrayLock.Unlock();
				InterlockedExchange(&m_isAudioPlaying,1);

				m_bIsOpenAudio = true;
				if(m_threadReadAudio.GetStop())
					m_threadReadAudio.Begin(_TimerReadAudioCallBack,this);
			}
		}
	}
        
	return true;

}

bool  CPlayProcess::StopALLMedia()
{
	m_playVideoLock.Lock();
	
	if(m_playVideoUnit.size() <= 0)
	{
		m_playVideoLock.Unlock();
		return true;
	}

	list<ViedePlayNode*>::iterator iter;
	for(iter = m_playVideoUnit.begin(); iter != m_playVideoUnit.end();iter++)
	{
		ViedePlayNode* pNode = (*iter);
		if(pNode)
		{
			if(pNode->pVideoPlayUnit)
			{
				if(m_bIsPlayLocal)
				{
					pNode->pVideoPlayUnit->stopFile();
				}
				else
				{
					pNode->pVideoPlayUnit->StopMeida(NULL);
				}

				delete pNode->pVideoPlayUnit;
				pNode->pVideoPlayUnit = NULL;
			}

			delete pNode;
			pNode = NULL;
		}
	}
	m_playVideoUnit.clear();
	m_playVideoLock.Unlock();
	
	if(InterlockedExchange(&m_isAudioPlaying,m_isAudioPlaying))
	{
		m_threadReadAudio.End(2);
		if(m_netAudioProcess)
		{
			moudle_commond_t cmd = STOP;
			m_netAudioProcess->SetCommond(cmd,NULL);
		}
		InterlockedExchange(&m_isAudioPlaying,0);
	}
		
	CPlayProcess::m_PlayrArrayLock.Lock();
	for(int i = 0; i < MAXPLAYERNUM ; i++)
	{
		if(g_PlayrArray[i] == this)
		{
			CPlayProcess::m_nRefCount--;
			g_PlayrArray[i] = NULL;
		}
	}

	if(CPlayProcess::m_nRefCount == 0 && CPlayProcess::m_nPlayAudioTimer)
	{
		timeKillEvent(CPlayProcess::m_nPlayAudioTimer);
		CPlayProcess::m_nPlayAudioTimer = 0;
	}

	CPlayProcess::m_PlayrArrayLock.Unlock();
	
	m_audJitterBuf.unInit();
	m_FlvAudioParsePos = 0;
	StopAudio();
	
	m_pcmLowbuffer.ClearBuffer();
	m_pcmHighbuffer.ClearBuffer();
	
	if(IsXPHigher())
	{
		if(m_playAudio)
		{
			m_playAudio->close();
			m_playAudio->release();
			m_playAudio = NULL;
		}
	}
	else
	{
		if(m_playSound)
		{
			delete m_playSound;
			m_playSound = NULL;
		}
	}

	return true;
}

bool  CPlayProcess::StopShowVideo(const char* szURL)
{
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		pvu->pVideoPlayUnit->StopShowVideo();
	}
        
	return true;
}

bool  CPlayProcess::StopMedia(const char* szURL,void* hPlayHand)
{
    bool bIsStop = false;
	ViedePlayNode* pvu = findPVUnit(szURL);
	if(pvu && pvu->pVideoPlayUnit)
	{
		bIsStop = pvu->pVideoPlayUnit->StopMeida(hPlayHand);
	}

	if(m_bIsSeparate)
	{
		if(InterlockedExchange(&m_isAudioPlaying,m_isAudioPlaying))
		{
			m_threadReadAudio.End(2);
			if(m_netAudioProcess)
			{
				moudle_commond_t cmd = STOP;
				m_netAudioProcess->SetCommond(cmd,NULL);
				bIsStop = true;
			}
			InterlockedExchange(&m_isAudioPlaying,0);
		}
	}

	if(bIsStop)
	{
		CPlayProcess::m_PlayrArrayLock.Lock();
		m_begPlayAudioSysTime = 0;
		m_begPlayAudoPtsTime = 0;
		m_nCurPlayAudoPtsTime = 0;
		m_nPlayAudioSysDiffTime = 0;
		for(int i = 0; i < MAXPLAYERNUM ; i++)
		{
			if(g_PlayrArray[i] == this)
			{
				CPlayProcess::m_nRefCount--;
				g_PlayrArray[i] = NULL;
			}
		}
		if(CPlayProcess::m_nRefCount == 0 && CPlayProcess::m_nPlayAudioTimer)
		{
			timeKillEvent(CPlayProcess::m_nPlayAudioTimer);
			CPlayProcess::m_nPlayAudioTimer = 0;
		}
		m_audJitterBuf.unInit();
		StopAudio();
		m_FlvAudioParsePos = 0;
		m_pcmLowbuffer.ClearBuffer();
		m_pcmHighbuffer.ClearBuffer();
		
		CPlayProcess::m_PlayrArrayLock.Unlock();
	}
	return true;
}

#include "stdafx.h"
#include "SaveFile.h"

#define VIDEO_FPS     20
#define X264_FPS      20
#define VIDEO_BITRATE 100

#define AUDIO_BITRATE    64000 // 64000
#define AUDIO_SAMPLERATE_CAP 32000
#define AUDIO_SAMPLERATE_ENC 32000

void  TimerCallBack(void* pUser)
{
      CSaveFile * pThis=(CSaveFile*)pUser;//由this指针获得实例的指针
      pThis->OnSaveVideoTimer(1);//调用要回调的成员方法
}

unsigned CALLBACK CSaveFile::SaveAudioCallBack(void* arg)
{
	return ((CSaveFile*)arg)->SaveAudioProc();
}

CSaveFile::CSaveFile()
{
	m_iSrcSaveVideoWidth = 0;
	m_iSrcSaveVideoHeight = 0;
	m_szSaveRGBBuffer = NULL;
	m_pSourScreen = NULL;
	m_vBodyBuffer= NULL;
	m_aBodyBuffer= NULL;
	m_nAACInputSamples = 0;
	m_pbPCMBuffer = NULL;
	m_pAudio = NULL;
	m_isAudioThreadRunning = 0;
	m_hSendAudioThread = NULL;
	m_pFlvFile = NULL;
	m_iStartTime = 0;
}

CSaveFile::~CSaveFile()
{
	m_iSrcSaveVideoWidth = 0;
	m_iSrcSaveVideoHeight = 0;
	if(m_pFlvFile)
	{
		PushDataToLocalFlvFileEnd();
	}

	if(m_pSourScreen)
	{
		m_pSourScreen->SourClose(false);
		delete m_pSourScreen;
		m_pSourScreen = NULL;
	}

	if(m_vSaveExcEncoder.ph264Encoder)
	{
		m_vSaveExcEncoder.ph264Encoder->h264_EncodeUninit();
		delete m_vSaveExcEncoder.ph264Encoder;
		m_vSaveExcEncoder.ph264Encoder = NULL;
	}

	if(m_szSaveRGBBuffer)
	{
		delete m_szSaveRGBBuffer;
		m_szSaveRGBBuffer = NULL;
	}

	if(m_aBodyBuffer)
	{
		delete m_aBodyBuffer;
		m_aBodyBuffer = NULL;
	}

	if(m_vBodyBuffer)
	{
		delete m_vBodyBuffer;
		m_vBodyBuffer = NULL;
	}

	if(m_pAudio)
	{
		m_pAudio = NULL;
	}
	
}


bool CSaveFile::PushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow,CSourAudio * pAudio)
{
	m_iStartTime = 0;

	if(NULL == hRecordWindow)
	{
		hRecordWindow = ::GetDesktopWindow();
	}

	if(m_pFlvFile == NULL && hRecordWindow)
	{
		StreamParam sParam;
		if(m_pSourScreen == NULL)
		{
			m_pSourScreen =  new CSourScreen(hRecordWindow,"screen");
		}
		
		m_pAudio = pAudio;

		if(m_pSourScreen == NULL || m_pAudio == NULL)
		{
			return false;
		}

		sParam.bIsFullScreen = false;
		m_pSourScreen->SetSourType(SOURCESCREEN);
		m_pSourScreen->SourOpen(&sParam);

		sParam.nAudioBitRate = 64000;
		sParam.nAudioSmpRateCap = AUDIO_SAMPLERATE_CAP;

		sParam.bAudioKaraoke = true;
		m_pAudio->setSaveFalg(true);
		m_pAudio->SourOpen(&sParam);
		
		m_iSrcSaveVideoWidth = 0;
		m_iSrcSaveVideoHeight = 0;
		m_pSourScreen->GetVideoWAndH(m_iSrcSaveVideoWidth,m_iSrcSaveVideoHeight);
		if(m_vSaveExcEncoder.ph264Encoder == NULL)
		{
			m_vSaveExcEncoder.nVideoW = 1280;
			m_vSaveExcEncoder.nVideoH = 720;
			m_vSaveExcEncoder.nVideoFps = 15;
			m_vSaveExcEncoder.nVideoBitRate = 2000;
			m_vSaveExcEncoder.ph264Encoder = new CH264Encoder();
			if(m_vSaveExcEncoder.ph264Encoder)
			{
				m_vSaveExcEncoder.ph264Encoder->h264_SetParam(m_vSaveExcEncoder.nVideoW,m_vSaveExcEncoder.nVideoH,m_vSaveExcEncoder.nVideoBitRate,m_vSaveExcEncoder.nVideoFps);
				if(!m_vSaveExcEncoder.ph264Encoder->h264_EncodeInit(m_iSrcSaveVideoWidth,m_iSrcSaveVideoHeight))
					return false;

				if(m_szSaveRGBBuffer == NULL)
				{
					m_szSaveRGBBuffer = new unsigned char[m_iSrcSaveVideoWidth * m_iSrcSaveVideoHeight * 3];
				}
				if(m_vBodyBuffer == NULL)
				{
					m_vBodyBuffer =  new unsigned char[1024*1024];
				}
			}
			else
				return false;
		}

		if(m_aSaveCurExcEncoder.paacEncoder == NULL)
		{
			m_aSaveCurExcEncoder.bAudioKaraoke = true;
			m_aSaveCurExcEncoder.nAudioBitRate = 64000;
			m_aSaveCurExcEncoder.nAudioSmpRateCap = AUDIO_SAMPLERATE_CAP;
			m_aSaveCurExcEncoder.paacEncoder = new CAACEncoder();
			if(m_aSaveCurExcEncoder.paacEncoder)
			{
				ULONG          nAACInputSamples = 0;
				m_aSaveCurExcEncoder.paacEncoder->aac_EncodeInit(2,m_aSaveCurExcEncoder.nAudioBitRate,m_aSaveCurExcEncoder.nAudioSmpRateCap,nAACInputSamples);
				m_nAACInputSamples = nAACInputSamples;
				if(m_pbPCMBuffer == NULL)
				{
					m_pbPCMBuffer = new BYTE [m_nAACInputSamples];
				}

				if(m_aBodyBuffer == NULL)
				{
					m_aBodyBuffer =  new unsigned char[1024*256];
				}
			}
		}

		m_pFlvFile  = new CFlvFile();
		if(NULL == m_pFlvFile)
		{
			return false;
		}

		if(m_pFlvFile->openFlvFile(szLocalFile))
		{
			FlvMediaData fmd;
			fmd.audiosamplerate = 32000;
			fmd.audio_data_rate = 64000;
			fmd.bNeedDurationAndFileSize = true;
			fmd.n_fps = 15;
			fmd.n_width = 1000;
			fmd.n_height = 700;
			fmd.video_data_rate = 2000000;
			m_pFlvFile->writeMediaHeaderTag(fmd);
			SaveMediaMsgHeader();
			int nSpace = 1000/15;
			m_nTimer.StartTimer(nSpace,TimerCallBack,this);
			StartSendAudioThread();
			return true;
		}
		else
		{
			delete m_pFlvFile;
			m_pFlvFile =  NULL;
		}
	}
	return false;
}

bool CSaveFile::PushDataToLocalFlvFileEnd()
{
	m_nTimer.StopTimer();
	StopSendAudioThread();	
	m_vLock.Lock();
	if(m_pSourScreen)
		m_pSourScreen->SourClose(false);
	if(m_pAudio)
	{
		m_pAudio->setSaveFalg(false);
		m_pAudio = NULL;
	}
	if(m_pFlvFile)
	{
		m_pFlvFile->closeFlvFile();
		delete m_pFlvFile;
		m_pFlvFile =  NULL;
	}
	m_vLock.Unlock();
	return true;
}

void CSaveFile::OnSaveVideoTimer(UINT_PTR nIDEvent)
{
	unsigned int nCameraBufferSize = 0;
	unsigned char *pDesBuf = NULL;
	m_vLock.Lock();
	if(m_pSourScreen && m_pSourScreen->GetBuffer(&pDesBuf,nCameraBufferSize))
	{
		unsigned int nBufSize = DATA_SIZE - 18;
		m_vBodyBuffer[0]=0x17;  
		m_vBodyBuffer[1]=0x01;  
		m_vBodyBuffer[2]=0x00;  
		m_vBodyBuffer[3]=0x00;  
		m_vBodyBuffer[4]=0x42; 
		if(!m_vSaveExcEncoder.ph264Encoder->h264_Encode(m_vBodyBuffer,nBufSize,m_szSaveRGBBuffer,pDesBuf,m_iSrcSaveVideoWidth,m_iSrcSaveVideoHeight))
		{
			m_vLock.Unlock();
			return ;
		}	
		SavePacketToFile(m_vBodyBuffer,nBufSize,true);
	}
	m_vLock.Unlock();
}


void CSaveFile::SaveMediaMsgHeader()
{

	memset(m_vBodyBuffer,0,1024*1024);
	unsigned char * szTmp = m_vBodyBuffer;  
	if(m_vSaveExcEncoder.ph264Encoder)
	{  
		m_vBodyBuffer[0]=0x17;  
		m_vBodyBuffer[1]=0x00;  
		m_vBodyBuffer[2]=0x00;  
		m_vBodyBuffer[3]=0x00;  
		m_vBodyBuffer[4]=0x00;  
		m_vBodyBuffer[5]=0x01;  
		m_vBodyBuffer[6]=0x42;  
		m_vBodyBuffer[7]=0xC0;  
		m_vBodyBuffer[8]=0x15;  
		m_vBodyBuffer[9]=0x03;  
		m_vBodyBuffer[10]=0x01;  
		szTmp=m_vBodyBuffer+11;  

		int nVSSize = 0;
		m_vSaveExcEncoder.ph264Encoder->h264_CopyPPSandSPS((unsigned char*)szTmp,nVSSize);
		szTmp += nVSSize;

		unsigned int nBodySize=szTmp-m_vBodyBuffer;
		if(m_pFlvFile)
		{
			m_pFlvFile->writeMediaDataTag(m_vBodyBuffer,nBodySize,0,true);
		}
	}

	if(m_aSaveCurExcEncoder.paacEncoder)
	{

		char hdr[] = {0xAF, 0x00, 0x12, 0x90};//后两个字节0x12, 0x90即AAC sequence header 16000sample rate
		unsigned int nBodySize	= 4;

		if(m_pFlvFile)
		{
			m_pFlvFile->writeMediaDataTag((unsigned char*)hdr,nBodySize,0,false);
		}
	}
}

unsigned CSaveFile::SaveAudioProc()
{
	while(InterlockedExchange(&m_isAudioThreadRunning,m_isAudioThreadRunning))
	{
		m_aLock.Lock();
		if(m_aSaveCurExcEncoder.paacEncoder && m_pAudio)
		{
			unsigned int nBufSize = m_nAACInputSamples;
			if(m_pAudio->GetBufferEx(&m_pbPCMBuffer,nBufSize))
			{
				//AAC encode
				BYTE* pbAACBuffer = NULL;
				unsigned int  outPutAAcSize = 0;
				if(!m_aSaveCurExcEncoder.paacEncoder->aac_Encode(&pbAACBuffer,outPutAAcSize,m_pbPCMBuffer,m_nAACInputSamples))
				{
					m_aLock.Unlock();
					break;
				}
				if(outPutAAcSize <= 0)
				{
					m_aLock.Unlock();
					continue;
				}

				unsigned char* szTemp = (unsigned char*)m_aBodyBuffer;  
				szTemp[0]			= 0xAF;
				szTemp[1]			= 0x01;// Raw AAC
				memcpy(szTemp+2,pbAACBuffer+7, outPutAAcSize-7);//AAC头七个字节数据去掉（文件头）
				unsigned int nBodySize	=  outPutAAcSize - 7 + 2;
				SavePacketToFile(m_aBodyBuffer,nBodySize,false);
			}
		}
		m_aLock.Unlock();
		Sleep(10);
	}

	return 0;
}

int CSaveFile::SavePacketToFile(unsigned char* pBuf,unsigned int nBufSize,bool bIsVideo)
{
	CLock::CAutoLock lock(m_lock);
	if(m_iStartTime == 0)
	{
		m_iStartTime = ::GetTickCount();
	}
	unsigned int nTimeStamp = ::GetTickCount() - m_iStartTime;
	
	if(m_pFlvFile)
	{
		m_pFlvFile->writeMediaDataTag(pBuf,nBufSize,nTimeStamp,bIsVideo);
	}
	return 0;
}

void CSaveFile::StartSendAudioThread()
{
	if(InterlockedExchange(&m_isAudioThreadRunning,m_isAudioThreadRunning))
		return;

	InterlockedExchange(&m_isAudioThreadRunning,1);
	m_hSendAudioThread = (HANDLE)_beginthreadex(NULL,0,SaveAudioCallBack,this,NULL,NULL);
}

void CSaveFile::StopSendAudioThread()
{
	InterlockedExchange(&m_isAudioThreadRunning,0);
	WaitForSingleObject(m_hSendAudioThread,INFINITE);
	CloseHandle(m_hSendAudioThread);
	m_hSendAudioThread = NULL;
}
#include "stdafx.h"
#include "SourAudio.h"
#include "AVE_AUDIO_RESAMPLE.h"
#include "globalconfig.h"

#pragma comment(lib, "voice_engine.lib")

void CALLBACK VoiceEngineCallback(CallBackType type, const void*arg, int size, void* cbarg)
{
	if(type == VOICE_DATA)
	{
		VData* pvdata = (VData*)arg;
		if(pvdata && pvdata->payloadSize > 0)
			((CSourAudio*)cbarg)->RecvAudio(pvdata);
	}
}

long   CSourAudio::GetDevMicCount()
{
	DeviceList vd = m_pVoiceEngine->GetInDevList();
	return vd.size;
}

bool   CSourAudio::GetDevMicName(long nIndex,char szName[256])
{
	DeviceList vd = m_pVoiceEngine->GetInDevList();
	if(nIndex >= vd.size && nIndex < 0)
		return false;

	USES_CONVERSION; 
	strcpy((LPSTR)szName,OLE2A(vd.nameArray[nIndex]));
	return true;
}

long CSourAudio::GetDevSpeakerCount()
{
	DeviceList vd = m_pVoiceEngine->GetOutDevList();
	return vd.size;
}

bool   CSourAudio::GetDevSpeakerName(long nIndex,char szName[256])
{
	DeviceList vd = m_pVoiceEngine->GetOutDevList();
	if(nIndex >= vd.size && nIndex < 0)
		return false;

	USES_CONVERSION; 
	strcpy((LPSTR)szName,OLE2A(vd.nameArray[nIndex]));
	return true;
}

bool  CSourAudio::SetDevOutPut(int index)
{
	if(m_pVoiceEngine)
		m_pVoiceEngine->SetOutDevice(index);
	return true;
}

CSourAudio::CSourAudio(string sname):ISourData(SOURCEDEVAUDIO,sname)
{
	m_pVoiceEngine = NULL;
	m_pAudioBufQue = NULL;
	m_pSaveAudioBufQue = NULL;
	m_bMicMute = false;
	m_saveFlag = false;
	m_bIsPreview = false;
	m_nAudioSampleRate = 32000;
	m_pVoiceEngine = CreateVoiceEngine(m_nAudioSampleRate);
	m_nMicVol = 50;
	m_nMicVol = 80;
	m_resampler = NULL;

	if(m_pVoiceEngine)
	{
		if(strcmp(sname.c_str(),"audio") == 0)
			m_pVoiceEngine->SetActiveVolume(0);
		else
			m_pVoiceEngine->SetActiveVolume(m_nMicVol);

		m_pVoiceEngine->SetNs(true);
		m_pVoiceEngine->SetEc(true);
		m_pVoiceEngine->SetVoiceDataEncoded(false);
		m_pVoiceEngine->SetKeySpeak(false);

		m_pAudioBufQue = new BufferQueue();
		m_pSaveAudioBufQue =  new BufferQueue();
		m_pPreviewAudioBufQue =  new BufferQueue();
		m_data  = new char[48000];
		m_resample = new char[24000];
	}
	m_bIsStart = false;
}

CSourAudio::~CSourAudio()
{
	if(m_pVoiceEngine)
    {
		m_pVoiceEngine->Stop();
    }

	SourClose(true);

	if(m_pVoiceEngine)
	{
		m_pVoiceEngine->Release();
		m_pVoiceEngine = NULL;
	}

	if(m_pAudioBufQue)
	{
		delete m_pAudioBufQue;
		m_pAudioBufQue = NULL;
	}
	if(m_pSaveAudioBufQue)
	{
		delete m_pSaveAudioBufQue;
		m_pSaveAudioBufQue = NULL;
	}

	if(m_pPreviewAudioBufQue)
	{
		delete m_pPreviewAudioBufQue;
		m_pPreviewAudioBufQue = NULL;
	}

	if(m_data)
	{
		delete m_data;
		m_data = NULL;
	}
	if(m_resample)
	{
		delete m_resample;
		m_resample = NULL;
	}

	if(m_resampler)
	{
		audio_resample_close(m_resampler);
		m_resampler = NULL;
	}
}

bool CSourAudio::GetBuffer(unsigned char** pBuf,unsigned int& nBufSize)
{
	if(m_pAudioBufQue)
	{
		if(m_pAudioBufQue->size() >= nBufSize)
		{
			memset(m_data,0,48000);
			unsigned int nSize = nBufSize;
			if(m_pAudioBufQue->pop(m_data,nSize))
			{
				memcpy(*pBuf,m_data,nSize);
				nBufSize = nSize;
				return true;
			}
		}
	}
	return false;
}

bool CSourAudio::GetBufferEx(unsigned char** pBuf,unsigned int& nBufSize)
{
	if(m_pSaveAudioBufQue && m_saveFlag)
	{
		if(m_pSaveAudioBufQue->size() >= nBufSize)
		{
			unsigned int nSize = nBufSize;
			if(m_pSaveAudioBufQue->pop((char*)(*pBuf),nSize))
			{
				nBufSize = nSize;
				return true;
			}
		}
	}
	return false;
}

void CSourAudio::setPreviewFalg(bool bIsFlag)
{
	m_bIsPreview = bIsFlag;
}

bool CSourAudio::GetPreviewBufferEx(unsigned char** pBuf,unsigned int& nBufSize,unsigned int& nAudioSmpRate,unsigned int& nChannel)
{

	if(m_pPreviewAudioBufQue && m_bIsPreview)
	{
		if(m_pPreviewAudioBufQue->size() >= nBufSize)
		{
			unsigned int nSize = nBufSize;
			if(m_pPreviewAudioBufQue->pop((char*)(*pBuf),nSize))
			{
				nBufSize = nSize;
				nAudioSmpRate = 32000;//m_sParam.nAudioSmpRateCap;
				nChannel = 1;//m_sParam.nAudioChannels;
				return true;
			}
		}
	}
	return false;
}

void CSourAudio::setSaveFalg(bool bIsFlag)
{
	m_saveFlag = bIsFlag;
}

bool CSourAudio::SetSourType(int nSourType)
{

	int nTemp = 0;
	if(strcmp(GetSourName().c_str(),"audio") == 0)
	{
		 nTemp = nSourType & SOURCEDEVAUDIO;
		 if( nTemp == SOURCEDEVAUDIO)
		 {
			m_bIsEffective = true;
		 }
		 else
		 {
			m_bIsEffective = false;
		 }
	}
	else
	{
		 nTemp = nSourType & SOURCEDEMUSIC;
		 if( nTemp == SOURCEDEMUSIC)
			m_bIsEffective = true;
		 else
			m_bIsEffective = false;
		 
	}
	return true;
}

bool CSourAudio::SourOpen(void* param) 
{
	if(m_pVoiceEngine && (m_bIsEffective || m_saveFlag || m_bIsPreview))
	{
		if(param)
		{
			 m_sParam = (*(StreamParam*)param);
			 if(m_sParam.nSelectMicID >= 0)
				 m_pVoiceEngine->SetInDevice(m_sParam.nSelectMicID);
		}

		if(m_sParam.nAudioSmpRateCap != m_nAudioSampleRate)
		{
			if(m_resampler)
			{
				audio_resample_close(m_resampler);
				m_resampler = NULL;
			}

			if(m_resampler == NULL)
			{
				m_resampler = av_audio_resample_init(1,1,m_sParam.nAudioSmpRateCap,m_nAudioSampleRate,AV_SAMPLE_FMT_S16,AV_SAMPLE_FMT_S16,16,10,0,1.0);
			}
		}

		if(!m_bIsStart)
		{
			m_bIsStart = m_pVoiceEngine->Start(50000,VoiceEngineCallback,(void*)this);
			//m_bIsStart = true;
		}

		if(m_sParam.bAudioKaraoke)
			m_pVoiceEngine->SetKaraoke(m_sParam.bAudioKaraoke);

		if(	m_pAudioBufQue)
			m_pAudioBufQue->clear();

	}
	return true;
}

bool CSourAudio::SourPreviewOpen(int nSelectID) 
{
	if(m_pVoiceEngine && m_bIsPreview && nSelectID >= 0)
	{
		m_pVoiceEngine->SetInDevice(nSelectID);
		if(!m_bIsStart)
		{
			m_bIsStart = m_pVoiceEngine->Start(50000,VoiceEngineCallback,(void*)this);
			//m_bIsStart = true;
		}
	}
	return true;
}

bool CSourAudio::SourClose(bool bIsColseDev)
{
	if(	m_pAudioBufQue)
		m_pAudioBufQue->clear();

	if(m_pSaveAudioBufQue && m_saveFlag)
	{
		m_pSaveAudioBufQue->clear();
	}

	m_bIsEffective = false;
	m_saveFlag = false;
	return true;
}

void CSourAudio::RecvAudio(VData* pvdata)
{

	if(!m_pAudioBufQue || pvdata == NULL || pvdata->payloadSize <= 0)
		return;

	int nsize = m_pAudioBufQue->size();
	int nSaveSize = m_pSaveAudioBufQue->size();
	int nPreviewSize = m_pPreviewAudioBufQue->size();

	if(nSaveSize <  8192 * 4 && m_saveFlag )
		m_pSaveAudioBufQue->push(pvdata->payload,pvdata->payloadSize);

	
	if(nPreviewSize <  8192 * 2 && m_bIsPreview)
		m_pPreviewAudioBufQue->push(pvdata->payload,pvdata->payloadSize);

	if(m_nAudioSampleRate == m_sParam.nAudioSmpRateCap)
	{
			
 		if(m_pAudioBufQue->size() < 8192*3)
		{
			m_pAudioBufQue->push(pvdata->payload,pvdata->payloadSize);
		}
	}
	else
	{
		if( m_sParam.nAudioSmpRateCap == 16000)
		{
			if(m_pAudioBufQue->size() < 8192)//...............
			{
				// default sample rate is 32000, modify into 16000
				AVE_Audio_Dnsample2((char*)(pvdata->payload), m_resample, 0, pvdata->payloadSize, m_nAudioSampleRate, m_sParam.nAudioSmpRateCap);
				int nRate = m_nAudioSampleRate / m_sParam.nAudioSmpRateCap;
				int nResampleSize = pvdata->payloadSize/nRate;
				m_pAudioBufQue->push(m_resample,nResampleSize);
			}
		}
		else if(m_sParam.nAudioSmpRateCap == 44100)
		{
			if(m_pAudioBufQue->size() < 8192 * 4 && m_resampler)
			{
				int nSample = m_nAudioSampleRate/100 ;
				int olen  = audio_resample(m_resampler,(short*)m_resample,(short*)(pvdata->payload),pvdata->payloadSize/2);
				int nSize  = olen*2 ;
				m_pAudioBufQue->push(m_resample,nSize);
			}
		}
		else if(m_sParam.nAudioSmpRateCap == 8000)
		{
			if(m_pAudioBufQue->size() < 8192 && m_resampler)
			{
				int nSample = m_nAudioSampleRate/100 ;
				int olen  = audio_resample(m_resampler,(short*)m_resample,(short*)(pvdata->payload),pvdata->payloadSize/2);
				m_pAudioBufQue->push(m_resample,olen*sizeof(short));
			}
		}
	}
}

bool CSourAudio::setMicMute(bool vol)
{
	if(m_pVoiceEngine && m_bIsEffective)
	{
		m_bMicMute = vol;
		m_pVoiceEngine->SetInMute(vol);
		return true;
	}
	return false;
}

bool CSourAudio::AVE_SetID(int nMainValue, int nSubValue, int nParams[2])
{
	
	return true;
}

bool CSourAudio::setMicVolume(long vol)
{
	if(m_pVoiceEngine && m_bIsEffective)
	{
		m_pVoiceEngine->SetInVolume(vol);
		m_nMicVol = vol;
		return true;
	}
	return false;
}

long CSourAudio::getMicVolume()
{
	return m_nMicVol;
}
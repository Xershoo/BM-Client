#include "stdafx.h"
#include "AudioRender.h"

CAudioRender::CAudioRender(IMMDevice* pDevice):CAudioImplBase(pDevice,AUDIO_IMPL_RENDER)
{

}

CAudioRender::~CAudioRender()
{

}

void CAudioRender::getResampleParam(int& in_sample_bit,int& in_channels ,int& in_sample_rate ,int& out_sample_bit,int& out_channels,int& out_sample_rate)
{
	in_sample_bit = m_setSampleBit;
	in_channels = m_setChannel;
	in_sample_rate = m_setSampleRate;
	out_sample_bit = m_audioFormat->wBitsPerSample;
	out_channels = m_audioFormat->nChannels;
	out_sample_rate = m_audioFormat->nSamplesPerSec;	
}

UINT CAudioRender::doAuidoImpl()
{
	if(NULL == m_audioClient)
	{
		return 0;
	}

	UINT32  nBufFrames = 0;	
	HRESULT	hr = S_OK;	
	BYTE *	pData = NULL;
	UINT32	nFramesOfPadding = 0;
	UINT	nNeedDataLen = 0;

	IAudioRenderClient* pAudioRenderClient = NULL;	

	HANDLE	eventWait[2] = {m_eventStop,m_audioImplEvent};	

	if(!initAudioResample())
	{
		goto _end;
	}

	hr = m_audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,m_audioFormat, NULL);
	if (hr == AUDCLNT_E_UNSUPPORTED_FORMAT)
	{
		goto _end;
	}

	hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST|AUDCLNT_STREAMFLAGS_EVENTCALLBACK, m_hnsDuration, m_hnsDuration, m_audioFormat, 0);
	if(FAILED(hr))
	{
		if(hr!= AUDCLNT_E_ALREADY_INITIALIZED)
		{
			goto _end;
		}
	}
	else
	{
		hr = m_audioClient->SetEventHandle(m_audioImplEvent);
		if(FAILED(hr))
		{
			goto _end;
		}
	}
	
	hr = m_audioClient->GetBufferSize(&nBufFrames);
	if(FAILED(hr))
	{
		goto _end;
	}

	hr = m_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pAudioRenderClient);
	if(FAILED(hr) || NULL == pAudioRenderClient) 
	{
		goto _end;
	}

	m_audioImpl = pAudioRenderClient;
	hr = m_audioClient->Start();
	if(FAILED(hr))
	{
		goto _end;
	}

	SetEvent(m_eventStart);

	if(m_procAudioState)
	{
		(*m_procAudioState)(AUDIO_STATE_START);
	}
		
	while(true)
	{
		//DWORD wait = WaitForMultipleObjects(sizeof(eventWait)/sizeof(eventWait[0]), eventWait, FALSE, (DWORD)(m_hnsDuration / REFTIMES_PER_MILLISEC));
		DWORD wait = WaitForSingleObject(m_eventStop, (DWORD)(m_hnsDuration / REFTIMES_PER_MILLISEC));
		if(WAIT_OBJECT_0 == wait) 
		{
			break;
		}

		hr = m_audioClient->GetCurrentPadding(&nFramesOfPadding);
		if(FAILED(hr)) 
		{
			break;
		}

		if(nBufFrames == nFramesOfPadding) 
		{
			continue;
		}

		nNeedDataLen = nBufFrames - nFramesOfPadding;
		hr = pAudioRenderClient->GetBuffer(nNeedDataLen, &pData);
		if(FAILED(hr))
		{
			break;
		}

		int read_size = getAudioDataFromList(pData,nNeedDataLen*m_audioFormat->nBlockAlign);
		if(read_size <= 0)
		{
			read_size = nNeedDataLen*m_audioFormat->nBlockAlign;
			memset(pData,0,read_size);	

			m_curVolume = 0;

		}
		else
		{
			calcPcmVolume(pData,read_size);
		}
		
		pAudioRenderClient->ReleaseBuffer(read_size / m_audioFormat->nBlockAlign, 0);
	}

	Sleep((DWORD)(m_hnsDuration/REFTIMES_PER_MILLISEC));
	
	m_audioClient->Stop();

	if(m_procAudioState)
	{
		(*m_procAudioState)(AUDIO_STATE_STOP);
	}

_end:
	unitAudioResample();	
	SAFE_RELEASE(m_audioImpl);

	return 0;
}
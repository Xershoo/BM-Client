#include "stdafx.h"
#include "AudioRecord.h"
#include "AudioEngine.h"
#include "AudioRender.h"
#include "PlaybackRecord.h"
#include "MediaBuffer.h"

CAudioRecord::CAudioRecord(IMMDevice* pDevice):CAudioImplBase(pDevice,CAudioImplBase::AUDIO_IMPL_CAP_MIC)
	,m_audioRender(NULL)
	,m_recordPlayback(NULL)
{

}

CAudioRecord::~CAudioRecord()
{
	SAFE_RELEASE(m_audioRender);
	SAFE_RELEASE(m_recordPlayback);
}

bool CAudioRecord::openAudioRender()
{	
	if(NULL == m_audioRender)
	{
		CAudioEngine* pAudioEngine = CAudioEngine::createInstance();
		if(NULL == pAudioEngine)
		{
			return false;
		}

		int spkIndex = pAudioEngine->getDefaultSpk();
		if(spkIndex < 0)
		{
			return false;
		}

		m_audioRender = (CAudioRender*)pAudioEngine->getSpkAudioImpl(spkIndex);
		if(NULL == m_audioRender)
		{
			return false;
		}
	}
		
	m_audioRender->setParam(m_audioFormat->nChannels,m_audioFormat->wBitsPerSample,m_audioFormat->nSamplesPerSec);
	
	if(m_audioRender->isOpen())
	{
		return true;	
	}
	
	return m_audioRender->open();
}

void CAudioRecord::setPreview(bool preview,int spkIndex)
{
	m_setPreview = preview;
	if(NULL == m_audioRender)
	{
		return;
	}

	if(!preview)
	{
		m_audioRender->close();
	}

	m_audioRender->freeAudioPacketList();
}

bool CAudioRecord::open()
{
	bool ret = false;
	return CAudioImplBase::open();
}

void CAudioRecord::close()
{
	CAudioImplBase::close();

	if(m_aecRecord)
	{		
		if(m_audioRender)
		{
			m_audioRender->close();
		}

		SAFE_RELEASE(m_audioRender);
	}
}

int CAudioRecord::getVolume()
{
	if(NULL == m_audioDevice)
	{
		return -1;
	}

	IAudioEndpointVolume*  audioVolume = NULL;	
	HRESULT hr = S_OK;
	float fVolume = 0.0f;
	int	  nVolume = -1;

	do 
	{
		m_audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void **)(&audioVolume)); 
		if(FAILED(hr) || NULL == audioVolume)
		{
			break;
		}

		hr = audioVolume->GetMasterVolumeLevelScalar(&fVolume);
		if(FAILED(hr))
		{
			break;
		}

		nVolume = (int)(fVolume*100);
	} while (false);

	SAFE_RELEASE(audioVolume);

	return nVolume;
}

void  CAudioRecord::setVolume(int volume)
{
	IAudioEndpointVolume*  audioVolume = NULL;	
	HRESULT hr = S_OK;
	float fVolume = 0.0f;
	
	do 
	{
		m_audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void **)(&audioVolume)); 
		if(FAILED(hr) || NULL == audioVolume)
		{
			break;
		}

		fVolume = (float)volume / 100;
		hr = audioVolume->SetMasterVolumeLevelScalar(fVolume,NULL);
		if(FAILED(hr))
		{
			break;
		}

	} while (false);

	SAFE_RELEASE(audioVolume);
	return ;
}

void CAudioRecord::renderPreviewData(BYTE* pbData, int nSize)
{
	if(!m_setPreview)
	{
		return;
	}

	if(NULL == pbData || nSize <= 0)
	{
		return;
	}
	
	bool ret = openAudioRender();
	if(!ret)
	{
		return;
	}

	m_audioRender->putRenderData(pbData,nSize);
	return;
}

UINT CAudioRecord::doAuidoImpl()
{
	if(NULL == m_audioClient)
	{
		return 0;
	}

	if(m_aecRecord)
	{
		doAecAudioRecord();
	}
	else
	{
		doAudioRecord();
	}

	if(m_procAudioState)
	{
		(*m_procAudioState)(AUDIO_STATE_STOP);
	}

	unitAudioResample();
	SAFE_RELEASE(m_audioImpl);

	return 0;
}

void CAudioRecord::doAudioRecord()
{
	IAudioCaptureClient* pAudioCaptureClient = NULL;
	HANDLE	eventWait[2] = {m_eventStop,m_audioImplEvent};

	UINT32  nBufFrames = 0;
	HRESULT hr = 0;
	BYTE *	pData = NULL;
	UINT32	nNextPacketSize = 0;
	UINT	nNumFramesToRead = 0;
	DWORD	dwFlags = 0;

	if(!initAudioResample())
	{
		return;
	}
		
	hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,AUDCLNT_STREAMFLAGS_EVENTCALLBACK, m_hnsDuration, m_hnsDuration, m_audioFormat, 0);
	if(FAILED(hr))
	{
		if(hr!= AUDCLNT_E_ALREADY_INITIALIZED)
		{
			return;
		}
	}
	else
	{
		hr = m_audioClient->SetEventHandle(m_audioImplEvent);
		if(FAILED(hr))
		{
			return;
		}
	}


	hr = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
	if(FAILED(hr) || NULL == pAudioCaptureClient) 
	{
		return;
	}

	m_audioImpl = pAudioCaptureClient;

	hr = m_audioClient->Start();
	if(FAILED(hr))
	{
		return;
	}

	SetEvent(m_eventStart);

	if(m_procAudioState)
	{
		(*m_procAudioState)(AUDIO_STATE_START);
	}

	while(true)
	{
		DWORD wait = WaitForMultipleObjects(sizeof(eventWait)/sizeof(eventWait[0]), eventWait, FALSE, (DWORD)(m_hnsDuration / REFTIMES_PER_MILLISEC));
		if(WAIT_OBJECT_0 == wait) 
		{
			break;
		}

		bool done = true;
		do 
		{
			hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
			if(FAILED(hr))
			{	
				done = false;
				break;
			}

			if (nNextPacketSize == 0) 
			{
				break;
			}

			hr = pAudioCaptureClient->GetBuffer(&pData,&nNumFramesToRead,&dwFlags,NULL,NULL);
			if(FAILED(hr))
			{	
				done = false;
				break;
			}

			if (0 != nNumFramesToRead )
			{
				ULONGLONG timeStamp = GetTickCount64();

				putAudioDataToList(pData,nNumFramesToRead*m_audioFormat->nBlockAlign);
				renderPreviewData(pData,nNumFramesToRead*m_audioFormat->nBlockAlign);
				calcPcmVolume(pData,nNumFramesToRead*m_audioFormat->nBlockAlign);

				if(m_recordPlayback)
				{
					m_recordPlayback->putKarokRecordData(pData,nNumFramesToRead*m_audioFormat->nBlockAlign,timeStamp);
				}
			}

			pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
		} while (::WaitForSingleObject(m_eventStop,1) == WAIT_TIMEOUT);

		if(!done)
		{
			break;
		}
	}

	m_audioClient->Stop();
	
	return;
}

void CAudioRecord::doAecAudioRecord()
{
	if(!initAecAudioRecord())
	{
		return;
	}

	if(!initAudioResample())
	{
		return;
	}

	IMediaObject * objAecAudio = static_cast<IMediaObject*>(m_audioImpl);
	if(NULL == objAecAudio)
	{
		return;
	}

	SetEvent(m_eventStart);

	if(m_procAudioState)
	{
		(*m_procAudioState)(AUDIO_STATE_START);
	}

	HRESULT hr = objAecAudio->AllocateStreamingResources();
	if(FAILED(hr))
	{
		return;		
	}

	PROPVARIANT				pvFrameSize;
	IPropertyStore*			storeProperty = NULL;
	CAecMediaBuffer 		*mediaBuffer = NULL;
	DMO_OUTPUT_DATA_BUFFER	dmoDataBuf = {0};

	int		sizeFrame = 0;
	BYTE *	readBuf = NULL;
	UINT	sizeBuf = 0;

	// Get actually frame size being used in the DMO. (optional, do as you need)
	hr = objAecAudio->QueryInterface(IID_IPropertyStore, (void**)&storeProperty);
	if(SUCCEEDED(hr))
	{
		PropVariantInit(&pvFrameSize);
		storeProperty->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize);
		sizeFrame = pvFrameSize.lVal;
		PropVariantClear(&pvFrameSize);
	}

	while(::WaitForSingleObject(m_eventStop,DMO_THREAD_LOOP_TIME) == WAIT_TIMEOUT)
	{	
		do 
		{
			hr = CAecMediaBuffer::Create(DMO_AEC_SAMPERATE*DMO_AEC_ALIGN,(IMediaBuffer**)&mediaBuffer);
			if(FAILED(hr) || NULL == mediaBuffer)
			{
				break;
			}

			DWORD dwStatus = 0;
			dmoDataBuf.pBuffer = mediaBuffer;
			dmoDataBuf.dwStatus = 0;
			hr = objAecAudio->ProcessOutput(0, 1, &dmoDataBuf, &dwStatus);
			if( FAILED(hr) || S_FALSE == hr)
			{
				SAFE_RELEASE(mediaBuffer);
				break;
			}

			DWORD	sampleSize  = 0;
			BYTE *	sampleBuf = NULL;
			hr = mediaBuffer->GetBufferAndLength(&sampleBuf,&sampleSize);
			if(FAILED(hr) || sampleSize <= 0)
			{
				SAFE_RELEASE(mediaBuffer);
				break;
			}

			ULONGLONG timeStamp = GetTickCount64();

			putAudioDataToList(sampleBuf,sampleSize);
			renderPreviewData(sampleBuf,sampleSize);
			calcPcmVolume(sampleBuf,sampleSize);

			if(m_recordPlayback)
			{
				m_recordPlayback->putKarokRecordData(sampleBuf,sampleSize,timeStamp);
			}

			SAFE_RELEASE(mediaBuffer);
		} while (dmoDataBuf.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);
	}

	objAecAudio->FreeStreamingResources();

	return;
}

bool CAudioRecord::initAecAudioRecord()
{
	HRESULT hr = S_FALSE;
	HANDLE currProcess = ::GetCurrentProcess ();
	if(SetPriorityClass (currProcess, HIGH_PRIORITY_CLASS) == 0)	
	{
		return false;
	}

	IMediaObject *	objAecAudio = NULL;  	
	IPropertyStore* storeProperty = NULL;
	PROPVARIANT		varProp = { 0 };
	DMO_MEDIA_TYPE	typeMedia = { 0 };
	WAVEFORMATEX	fmtWave = {WAVE_FORMAT_PCM, DMO_AEC_CHANNEL, DMO_AEC_SAMPERATE, DMO_AEC_SAMPECAP, DMO_AEC_ALIGN, DMO_AEC_SAMPEBIT, 0};
	UINT spkIndex = CAudioEngine::createInstance()->getDefaultSpk();
	INT  micIndex = CAudioEngine::createInstance()->getMicIndex(m_deviceId);

	if(micIndex < 0)
	{
		return false;
	}

	bool  done = false;

	do 
	{
		hr = ::CoCreateInstance(CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (void**)&objAecAudio);
		if( FAILED(hr) || NULL == objAecAudio)
		{
			break;
		}

		hr = objAecAudio->QueryInterface(IID_IPropertyStore, (void**)&storeProperty);
		if( FAILED(hr) || NULL == storeProperty)
		{
			break;
		}

		//set option
		PropVariantInit(&varProp);
		varProp.vt = VT_I4;
		varProp.lVal = (LONG)(0);
		hr = storeProperty->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, varProp);
		if(FAILED(hr))
		{
			break;
		}

		PropVariantClear(&varProp);
		PropVariantInit(&varProp);
		varProp.vt = VT_I4;
		varProp.lVal = (LONG)true;
		hr = storeProperty->SetValue(MFPKEY_WMAAECMA_FEATR_NS, varProp);
		if(FAILED(hr))
		{
			break;
		}

		PropVariantClear(&varProp);
		PropVariantInit(&varProp);
		varProp.vt = VT_BOOL;
		varProp.lVal = VARIANT_FALSE;
		hr = storeProperty->SetValue(MFPKEY_WMAAECMA_FEATR_AGC, varProp);
		if(FAILED(hr))
		{
			break;
		}

		PropVariantClear(&varProp);
		PropVariantInit(&varProp);
		varProp.vt = VT_BOOL;
		varProp.lVal = VARIANT_FALSE;
		hr = storeProperty->SetValue(MFPKEY_WMAAECMA_FEATR_CENTER_CLIP, varProp);
		if(FAILED(hr))
		{
			break;
		}

		PropVariantClear(&varProp);
		PropVariantInit(&varProp);
		varProp.vt = VT_I4;
		varProp.lVal = (unsigned long)(spkIndex<<16) + (unsigned long)(0x0000ffff & micIndex);
		hr = storeProperty->SetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, varProp);
		if(FAILED(hr))
		{
			break;
		}
		
		//set media type
		hr = ::MoInitMediaType(&typeMedia, sizeof(WAVEFORMATEX));
		if(FAILED(hr))
		{
			break;
		}

		typeMedia.majortype = MEDIATYPE_Audio;
		typeMedia.subtype = MEDIASUBTYPE_PCM;
		typeMedia.lSampleSize = 0;
		typeMedia.bFixedSizeSamples = TRUE;
		typeMedia.bTemporalCompression = FALSE;
		typeMedia.formattype = FORMAT_WaveFormatEx;

		memcpy(typeMedia.pbFormat, &fmtWave, sizeof(WAVEFORMATEX));
		hr = objAecAudio->SetOutputType(0, &typeMedia, 0);
		if(FAILED(hr))
		{
			break;
		}

		m_audioFormat->wFormatTag = WAVE_FORMAT_PCM;
		m_audioFormat->nChannels = DMO_AEC_CHANNEL;
		m_audioFormat->wBitsPerSample = DMO_AEC_SAMPEBIT;
		m_audioFormat->nSamplesPerSec = DMO_AEC_SAMPERATE;
		m_audioFormat->nBlockAlign = DMO_AEC_ALIGN;
		m_audioFormat->nAvgBytesPerSec = DMO_AEC_SAMPECAP;

		done = true;
	} while (false);

	::MoFreeMediaType(&typeMedia);
	PropVariantClear(&varProp);
	SAFE_RELEASE(storeProperty);

	if(done)
	{
		m_audioImpl = objAecAudio;
	}
	else
	{
		SAFE_RELEASE(objAecAudio);
	}

	return done; 
}

void CAudioRecord::setKarokRecord(CPlaybackRecord* recordPlayback)
{
	if(m_recordPlayback)
	{
		m_recordPlayback->Release();
	}

	m_recordPlayback = recordPlayback;
	if(NULL != m_recordPlayback)
	{
		m_recordPlayback->AddRef();
	}
}

void CAudioRecord::setAec(bool useAec)
{
	this->m_aecRecord = useAec;
	
	if(!isOpen())
	{
		return;
	}

	// reopen
	close();
	open();
}
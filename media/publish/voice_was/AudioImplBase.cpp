#include "stdafx.h"
#include "AudioImplBase.h"
#include "common_macro.h"

CAudioImplBase::CAudioImplBase(IMMDevice* pDevice,audio_impl_type type) :m_audioDevice(pDevice)
	,m_audioClient(NULL)
	,m_threadHandle(NULL)
	,m_eventStart(NULL)
	,m_eventStop(NULL)
	,m_audioImpl(NULL)
	,m_audioImplEvent(NULL)
	,m_audioFormat(NULL)
	,m_refCount(1)
	,m_open(false)
	,m_setChannel(DEF_AUDIO_CHANNEL)
	,m_setSampleBit(DEF_AUDIO_SAMPLE_BIT)
	,m_setSampleRate(DEF_AUDIO_SAMPLE_RATE)
	,m_procDeviceState(NULL)
	,m_procAudioState(NULL)
	,m_swrContext(NULL)
	,m_outAudioBuf(NULL)
	,m_outAudioSize(0)
	,m_curVolume(0)
	,m_initialize(false)
	,m_aecRecord(false)
	,m_karokRecord(false)
	,m_setPreview(false)
	,m_typeAudio(type)
	,m_hnsDuration(0)
{
	memset(&m_deviceId,0,sizeof(m_deviceId));

	::CoInitialize(NULL);
	::InitializeCriticalSectionAndSpinCount(&m_crstPacketList,4000);

	initAudioImpl();
}

CAudioImplBase::~CAudioImplBase()
{
	freeAudioImpl();

	::CoUninitialize();
	::DeleteCriticalSection(&m_crstPacketList);
}

void CAudioImplBase::initAudioImpl()
{
	if(NULL==m_audioDevice)
	{
		return;
	}

	LPWSTR				pwszId = NULL;
	HRESULT				hr = S_OK;	
		
	do 
	{
		hr = m_audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_audioClient);
		if(FAILED(hr) || NULL == m_audioClient) 
		{
			break;
		}

		hr = m_audioClient->GetMixFormat(&m_audioFormat);
		if(FAILED(hr))
		{
			break;
		}

		if(!AdjustFormatTo16Bits(m_audioFormat))
		{
			break;
		}

		hr = m_audioDevice->GetId(&pwszId);
		if(NULL != pwszId)
		{
			wcscpy_s(m_deviceId,pwszId);
			CoTaskMemFree(pwszId);
		}

		m_audioImplEvent = ::CreateEvent(NULL,false,false,NULL);
		if(NULL == m_audioImplEvent)
		{
			break;
		}

		hr = m_audioClient->GetDevicePeriod(&m_hnsDuration,NULL);
		if(FAILED(hr))
		{
			break;
		}
		
		m_hnsDuration *= 10;		
		m_initialize = true;
	} while (false);
	
	return;
}

void CAudioImplBase::freeAudioImpl()
{
	if(!m_initialize)
	{
		return;
	}

	m_initialize = false;

	close();
	
	SAFE_RELEASE(m_audioClient);
	SAFE_RELEASE(m_audioDevice);
	SAFE_CLOSE_HANDLE(m_audioImplEvent);

	if(NULL != m_audioFormat)
	{
		::CoTaskMemFree(m_audioFormat);
		m_audioFormat = NULL;		
	}
	
	memset(&m_deviceId,0,sizeof(m_deviceId));
}

bool CAudioImplBase::AdjustFormatTo16Bits(WAVEFORMATEX * fmtWave)
{
	if(fmtWave->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		fmtWave->wFormatTag = WAVE_FORMAT_PCM;
		fmtWave->wBitsPerSample = 16;
		fmtWave->nBlockAlign = fmtWave->nChannels * fmtWave->wBitsPerSample / 8;
		fmtWave->nAvgBytesPerSec = fmtWave->nBlockAlign * fmtWave->nSamplesPerSec;
	}
	else if(fmtWave->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(fmtWave);
		if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
		{
			pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			pEx->Samples.wValidBitsPerSample = 16;
			fmtWave->wBitsPerSample = 16;
			fmtWave->nBlockAlign = fmtWave->nChannels * fmtWave->wBitsPerSample / 8;
			fmtWave->nAvgBytesPerSec = fmtWave->nBlockAlign * fmtWave->nSamplesPerSec;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void CAudioImplBase::setMute(bool mute)
{
	if(NULL == m_audioDevice)
	{
		return;
	}

	IAudioSessionManager2* mgrAudioSession = NULL;
	ISimpleAudioVolume*   splAudioVolume = NULL;
	HRESULT hr = S_OK;

	do 
	{
		m_audioDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void **)(&mgrAudioSession)); 
		if(FAILED(hr) || NULL == mgrAudioSession)
		{
			break;
		}


		hr = mgrAudioSession->GetSimpleAudioVolume(NULL,FALSE,&splAudioVolume);
		if(FAILED(hr) || NULL == splAudioVolume)
		{
			break;
		}

		hr = splAudioVolume->SetMute(mute ? TRUE:FALSE,NULL);
	} while (false);

	SAFE_RELEASE(splAudioVolume);
	SAFE_RELEASE(mgrAudioSession);

	return;
}

bool CAudioImplBase::getMute()
{
	if(NULL == m_audioDevice)
	{
		return false;
	}

	IAudioSessionManager2* mgrAudioSession = NULL;
	ISimpleAudioVolume*   splAudioVolume = NULL;
	HRESULT hr = S_OK;
	BOOL	bMute = FALSE;

	do 
	{
		m_audioDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void **)(&mgrAudioSession)); 
		if(FAILED(hr) || NULL == mgrAudioSession)
		{
			break;
		}


		hr = mgrAudioSession->GetSimpleAudioVolume(NULL,FALSE,&splAudioVolume);
		if(FAILED(hr) || NULL == splAudioVolume)
		{
			break;
		}

		hr = splAudioVolume->GetMute(&bMute);
	} while (false);

	SAFE_RELEASE(splAudioVolume);
	SAFE_RELEASE(mgrAudioSession);

	return bMute?true:false;
}
			
int CAudioImplBase::getVolume()
{
	if(NULL == m_audioDevice)
	{
		return -1;
	}

	IAudioSessionManager2* mgrAudioSession = NULL;
	ISimpleAudioVolume*   splAudioVolume = NULL;
	HRESULT hr = S_OK;
	float fVolume = 0.0f;
	int	  nVolume = -1;

	do 
	{
		m_audioDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void **)(&mgrAudioSession)); 
		if(FAILED(hr) || NULL == mgrAudioSession)
		{
			break;
		}


		hr = mgrAudioSession->GetSimpleAudioVolume(NULL,FALSE,&splAudioVolume);
		if(FAILED(hr) || NULL == splAudioVolume)
		{
			break;
		}

		hr = splAudioVolume->GetMasterVolume(&fVolume);
		if(FAILED(hr))
		{
			break;
		}

		nVolume = (int)(fVolume*100);
	} while (false);

	SAFE_RELEASE(splAudioVolume);
	SAFE_RELEASE(mgrAudioSession);

	return nVolume;
}

void  CAudioImplBase::setVolume(int volume)
{
	if(NULL == m_audioDevice)
	{
		return ;
	}

	IAudioSessionManager2* mgrAudioSession = NULL;
	ISimpleAudioVolume*   splAudioVolume = NULL;
	HRESULT hr = S_OK;
	float fVolume = 0.0f;

	do 
	{
		m_audioDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void **)(&mgrAudioSession)); 
		if(FAILED(hr) || NULL == mgrAudioSession)
		{
			break;
		}
		
		hr = mgrAudioSession->GetSimpleAudioVolume(NULL,FALSE,&splAudioVolume);
		if(FAILED(hr) || NULL == splAudioVolume)
		{
			break;
		}

		fVolume = (float)volume / 100;
		hr = splAudioVolume->SetMasterVolume(fVolume,NULL);
		if(FAILED(hr))
		{
			break;
		}

	} while (false);

	SAFE_RELEASE(splAudioVolume);
	SAFE_RELEASE(mgrAudioSession);

	return ;
}
			
void CAudioImplBase::setAudioStateCallback(PDeviceStateProc devStateProc,PAudioStateProc audioStateProc)
{
	m_procDeviceState = devStateProc;
	m_procAudioState = audioStateProc;
}

void CAudioImplBase::setParam(int channel,int sampleBit,int sampleRate)
{
	if (channel == m_setChannel && 
		sampleBit == m_setSampleBit && 
		sampleRate == m_setSampleRate)
	{
		return;
	}

	m_setChannel = channel;
	m_setSampleBit = sampleBit;
	m_setSampleRate = sampleRate;

	if(!isOpen())
	{
		return;
	}

	close();
	open();
}
	
void CAudioImplBase::getParam(int& channel,int& sampleBit,int& sampleRate)
{
	channel = m_setChannel;
	sampleBit = m_setSampleBit;
	sampleRate = m_setSampleRate;
}

void CAudioImplBase::getDeviceParam(int& channel,int& sampleBit,int& sampleRate)
{
	channel = m_audioFormat->nChannels;
	sampleBit = m_audioFormat->wBitsPerSample;
	sampleRate =  m_audioFormat->nSamplesPerSec;
}

bool CAudioImplBase::isNeedResample()
{
	if(NULL == m_audioFormat)
	{
		return false;
	}

	if (m_setChannel != m_audioFormat->nChannels ||
		m_setSampleBit != m_audioFormat->wBitsPerSample ||
		m_setSampleRate != m_audioFormat->nSamplesPerSec)
	{
		return true;
	}

	return false;
}

bool CAudioImplBase::isOpen()
{
	return m_open;
}

bool CAudioImplBase::open()
{
	if(m_open)
	{
		return true;
	}

	//free audio data.......
	freeAudioPacketList();
	SAFE_FREE(m_outAudioBuf);
	m_outAudioSize = 0;

	do 
	{
		m_eventStart = ::CreateEvent(NULL,true,false,NULL);
		if(NULL == m_eventStart)
		{
			break;
		}

		m_eventStop = ::CreateEvent(NULL,true,false,NULL);
		if(NULL == m_eventStop)
		{
			break;
		}

		m_threadHandle = (HANDLE)_beginthreadex(NULL,0,&audioThreadProc,(void*)this,0,NULL);
		if(NULL == m_threadHandle)
		{			
			break;
		}

		HANDLE waitsHandle[2] = {m_eventStart, m_threadHandle};
		DWORD wait = ::WaitForMultipleObjects(sizeof(waitsHandle)/sizeof(waitsHandle[0]), waitsHandle, FALSE, INFINITE);
		if(wait != WAIT_OBJECT_0)
		{			
			break;
		}

		m_open = true;
	} while (false);

	if(!m_open)
	{
		SAFE_CLOSE_HANDLE(m_threadHandle);
		SAFE_CLOSE_HANDLE(m_eventStart);
		SAFE_CLOSE_HANDLE(m_eventStop);
	}

	return m_open;	
}

void CAudioImplBase::close()
{
	if(!m_open)
	{
		return;
	}

	m_open = false;

	if(m_eventStop)
	{
		::SetEvent(m_eventStop);
	}

	if(m_threadHandle)
	{
		::WaitForSingleObject(m_threadHandle,INFINITE);
	}

	SAFE_CLOSE_HANDLE(m_threadHandle);
	SAFE_CLOSE_HANDLE(m_eventStart);
	SAFE_CLOSE_HANDLE(m_eventStop);
}

int CAudioImplBase::getCurrentVolume()
{
	return m_curVolume;
}

UINT _stdcall CAudioImplBase::audioThreadProc(void* lParam)
{
	CAudioImplBase* audioImpl = (CAudioImplBase*)lParam;
	if (NULL == audioImpl)
	{
		return 0;
	}

	UINT	ret = 0;
	DWORD	mmcssTaskIndex = 0;
	HANDLE	mmcssHandle = NULL;
	WCHAR   wszTaskName[32] = { 0 };
	
	::CoInitialize(NULL);

	switch(audioImpl->m_typeAudio)
	{
	case AUDIO_IMPL_RENDER:
		{
			wcscpy_s(wszTaskName,L"Audio");
		}
		break;
	case AUDIO_IMPL_CAP_MIC:
		{
			wcscpy_s(wszTaskName,L"Capture");
		}
		break;
	case AUDIO_IMPL_CAP_SPK:
		{
			wcscpy_s(wszTaskName,L"Playback");
		}
		break;
	default:
		{
			wcscpy_s(wszTaskName,L"Audio");
		}
		break;
	}

	::timeBeginPeriod(1);
	mmcssHandle = ::AvSetMmThreadCharacteristicsW(wszTaskName, &mmcssTaskIndex);

	ret = audioImpl->doAuidoImpl();
	
	::AvRevertMmThreadCharacteristics(mmcssHandle);
	::timeEndPeriod(1);
	::CoUninitialize();

	return ret;
}

void CAudioImplBase::putRenderData(BYTE* pbData,int dataSize)
{
	if(m_typeAudio != AUDIO_IMPL_RENDER)
	{
		return;
	}

	if(NULL==pbData||0>=dataSize)
	{
		return;
	}

	BYTE *	audioBuf = pbData;
	int		audioSize = dataSize;
	int		push_size = 0;

	LPAUDIOPACKET audioPacket = NULL;

	CAutoLock autoLock(&m_crstPacketList);

	if(!m_listAudioPacket.empty())
	{
		audioPacket = dynamic_cast<LPAUDIOPACKET>(*m_listAudioPacket.rbegin());
		if(NULL != audioPacket)
		{
			push_size = audioPacket->pushData(audioBuf,audioSize);
			if(push_size == audioSize)
			{	
				return;
			}

			audioSize -= push_size;
			audioPacket = NULL;
		}
	}

	while(audioSize > 0 )
	{
		audioPacket = new AUDIOPACKET;
		if(NULL == audioPacket)
		{
			break;
		}

		int push = audioPacket->pushData(audioBuf+push_size,audioSize);
		if(push <= 0)
		{
			SAFE_DELETE(audioPacket);
			break;
		}

		m_listAudioPacket.push_back(audioPacket);
		push_size += push;
		audioSize -= push;
	}

	return;
}

int	CAudioImplBase::getRecordData(BYTE* pbData,int dataSize)
{
	memset(pbData,0,dataSize);

	if(m_typeAudio == AUDIO_IMPL_RENDER)
	{
		return 0;
	}

	if(NULL==pbData||0>=dataSize)
	{
		return 0;
	}
	
	int	  allSize = 0;	
	int   destSize = dataSize;
	BYTE* bufDest = pbData;

	CAutoLock autoLock(&m_crstPacketList);

	// get new buffer
	while(!m_listAudioPacket.empty())
	{
		LPAUDIOPACKET audioPacket = dynamic_cast<LPAUDIOPACKET>(*m_listAudioPacket.begin());
		if(NULL==audioPacket)
		{
			break;
		}

		int packSize = audioPacket->sizeAudio;
		int getSize = audioPacket->popData(bufDest,destSize);
		if(getSize ==0 )
		{
			break;
		}

		allSize += getSize;
		destSize -= getSize;
		bufDest += getSize;

		if(destSize <= 0)
		{
			if(getSize == packSize)
			{
				m_listAudioPacket.pop_front();
				SAFE_DELETE(audioPacket);
			}

			break;
		}

		m_listAudioPacket.pop_front();
		SAFE_DELETE(audioPacket);
	}
	return allSize;
}

void CAudioImplBase::setAec(bool useAec)
{
	m_aecRecord = useAec;
}

void CAudioImplBase::setKarok(bool useKarok,int micIndex)
{
	m_karokRecord = useKarok;
}

void CAudioImplBase::setPreview(bool preview,int spkIndex)
{
	m_setPreview = preview;
}

void CAudioImplBase::putAudioDataToList(BYTE* pbData,int nSize)
{	
	if(NULL == pbData || nSize <=0)
	{
		return;
	}

	BYTE*	pbAudioData = NULL;
	int		nDataSize = 0;
	bool	bResample = false;
	bool	ret = false;

	bResample = isNeedResample();
	if(bResample)
	{
		ret = resampleAudioData(pbData,nSize,&pbAudioData,nDataSize);
		if(!ret)
		{
			return;
		}

		if( NULL == pbAudioData || nDataSize <= 0)
		{
			return;
		}
	}
	else
	{
		pbAudioData = pbData;
		nDataSize = nSize;
	}

	LPAUDIOPACKET audioPacket = NULL;
	CAutoLock autoLock(&m_crstPacketList);

	do
	{
		if(!m_listAudioPacket.empty())
		{
			audioPacket = dynamic_cast<LPAUDIOPACKET>(*m_listAudioPacket.rbegin());
			if(NULL != audioPacket)
			{	
				ret = audioPacket->pushDataAll(pbAudioData,nDataSize);
				if(ret)
				{	
					break;
				}
			}
		}

		audioPacket = new AUDIOPACKET;
		if(NULL == audioPacket)
		{
			break;
		}

		if(!audioPacket->pushDataAll(pbAudioData,nDataSize))
		{
			break;
		}

		m_listAudioPacket.push_back(audioPacket);
		if(m_listAudioPacket.size() > 5)
		{
			LPAUDIOPACKET popPacket = dynamic_cast<LPAUDIOPACKET>(*m_listAudioPacket.begin());
			SAFE_DELETE(popPacket);
			m_listAudioPacket.pop_front();
		}

		ret = true;
	}while(false);

	if(!ret)
	{
		SAFE_DELETE(audioPacket);
	}

	if(bResample)
	{
		SAFE_FREE(pbAudioData);
		nDataSize = 0;
	}

	return;
}

UINT CAudioImplBase::getAudioDataFromList(BYTE* pbData,int dataSize)
{
	if(NULL == pbData || dataSize <=0)
	{
		return 0;
	}

	BYTE* bufDest = pbData;
	int   getSize = 0;

	// copy remainder buffer
	if(NULL != m_outAudioBuf && m_outAudioSize > 0)
	{
		getSize = dataSize > m_outAudioSize ? m_outAudioSize : dataSize;
		memcpy(bufDest,m_outAudioBuf,getSize);

		if(getSize == dataSize)
		{
			m_outAudioSize -= getSize;
			memmove(m_outAudioBuf,m_outAudioBuf+getSize,m_outAudioSize);

			return dataSize;
		}

		bufDest += getSize;
		SAFE_FREE(m_outAudioBuf);
		m_outAudioSize = 0;
	}

	BYTE * audioBuf = NULL;
	int	   audioSize = 0;

	CAutoLock autoLock(&m_crstPacketList);

	// get new buffer
	while(!m_listAudioPacket.empty())
	{
		LPAUDIOPACKET audioPacket = dynamic_cast<LPAUDIOPACKET>(*m_listAudioPacket.begin());
		m_listAudioPacket.pop_front();

		if(NULL == audioPacket)
		{			
			continue;
		}

		bool resample = isNeedResample();
		if(resample)
		{
			bool  ret = resampleAudioData(audioPacket->dataAudio,audioPacket->sizeAudio,&audioBuf,audioSize); //resample audio data
			if(!ret)
			{
				SAFE_DELETE(audioPacket);
				break;
			}

			if(NULL == audioBuf || audioSize <= 0)
			{
				SAFE_DELETE(audioPacket);
				break;
			}
		}
		else
		{
			audioBuf = audioPacket->dataAudio;
			audioSize = audioPacket->sizeAudio;
		}

		// copy audio data
		int copySize = (dataSize - getSize) > audioSize ? audioSize : (dataSize - getSize);
		memcpy(bufDest,audioBuf,copySize);

		getSize += copySize;
		bufDest += copySize;

		if(copySize < audioSize)
		{
			m_outAudioSize = audioSize - copySize;
			if(m_outAudioSize > 0)					// if it has remainder buffer ,copy and save 
			{
				m_outAudioBuf = (BYTE*)malloc(m_outAudioSize);
				memcpy(m_outAudioBuf,audioBuf + copySize,m_outAudioSize);
			}
		}

		if(resample)
		{
			SAFE_FREE(audioBuf);
			audioSize = 0;
		}

		SAFE_DELETE(audioPacket);

		if(getSize == dataSize)		// buffer is fill
		{
			break;
		}
	}

	return getSize;
}

void CAudioImplBase::calcPcmVolume(BYTE* pbData,int nSize) // only for 16bit 16000 1
{
	short * buf = (short*)pbData;
	int     len = nSize / 2;

	double  sum = 0.0;

	for(int i=0;i<len;i++)
	{
		sum += (buf[i] * buf[i]);
	}

	double cur = sum / nSize;
	double ndb = 10*log10(cur);
	m_curVolume = (int)ndb;
}

void CAudioImplBase::freeAudioPacketList()
{
	CAutoLock autoLock(&m_crstPacketList);

	while(!m_listAudioPacket.empty())
	{
		LPAUDIOPACKET audioPacket =  dynamic_cast<LPAUDIOPACKET> (*m_listAudioPacket.begin());
		SAFE_DELETE(audioPacket);
		m_listAudioPacket.pop_front();
	}
}

void CAudioImplBase::getResampleParam(int& in_sample_bit,int& in_channels ,int& in_sample_rate ,int& out_sample_bit,int& out_channels,int& out_sample_rate)
{
	in_sample_bit = m_audioFormat->wBitsPerSample;
	in_channels = m_audioFormat->nChannels;
	in_sample_rate = m_audioFormat->nSamplesPerSec;
	out_sample_bit = m_setSampleBit;
	out_channels = m_setChannel;
	out_sample_rate = m_setSampleRate;
}

AVSampleFormat CAudioImplBase::fromSampleBit(int sampleBit)
{
	AVSampleFormat sampleFmt = AV_SAMPLE_FMT_NONE;
	
	switch(sampleBit)
	{
	case 8:
		sampleFmt = AV_SAMPLE_FMT_U8;
		break;
	case 16:
		sampleFmt = AV_SAMPLE_FMT_S16;
		break;
	case 32:
		sampleFmt = AV_SAMPLE_FMT_S32;	
		break;
	}

	return sampleFmt;
}

bool CAudioImplBase::initAudioResample()
{
	if(!isNeedResample())
	{
		return true;
	}

	if(NULL == m_audioFormat)
	{
		return false;
	}

	if(NULL != m_swrContext)
	{
		unitAudioResample();
	}

	m_swrContext = swr_alloc();
	if(NULL==m_swrContext)
	{
		return false;
	}

	int in_sample_bit = 0;
	int out_sample_bit = 0;
	int in_sample_rate = 0; 
	int out_sample_rate = 0;
	int dst_nb_channels = 0; 
	int src_nb_channels = 0;

	getResampleParam(in_sample_bit,src_nb_channels,in_sample_rate ,out_sample_bit,dst_nb_channels,out_sample_rate);	
	if(!initSwrContext(m_swrContext,in_sample_bit,src_nb_channels,in_sample_rate,out_sample_bit,dst_nb_channels,out_sample_rate))
	{
		return false;
	};

	return true;	
}

bool CAudioImplBase::initSwrContext(SwrContext* pSwrContext ,int in_sample_bit,int in_channels ,int in_sample_rate ,int out_sample_bit,int out_channels,int out_sample_rate)
{
	if(NULL == pSwrContext)
	{
		return false;
	}

	if(swr_is_initialized(pSwrContext))
	{
		return true;
	}

	int64_t src_ch_layout = AV_CH_LAYOUT_STEREO;	//初始化这样根据不同文件做调整  
	int64_t dst_ch_layout = AV_CH_LAYOUT_MONO;		//这里设定ok
	int in_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_fmt = AV_SAMPLE_FMT_S16;

	src_ch_layout = av_get_default_channel_layout(in_channels);
	dst_ch_layout = av_get_default_channel_layout(out_channels); 

	in_sample_fmt = fromSampleBit(in_sample_bit);
	out_sample_fmt = fromSampleBit(out_sample_bit);

	if(in_sample_fmt == AV_SAMPLE_FMT_NONE || out_sample_fmt == AV_SAMPLE_FMT_NONE)
	{
		return false;
	}
	
	if (src_ch_layout <= 0 || dst_ch_layout <= 0)
	{
		return false;  
	}  

	/* set options */  
	av_opt_set_int(pSwrContext, "in_channel_layout",    src_ch_layout, 0);  
	av_opt_set_int(pSwrContext, "in_sample_rate",       in_sample_rate, 0);  
	av_opt_set_sample_fmt(pSwrContext, "in_sample_fmt", (AVSampleFormat)in_sample_fmt, 0);  

	av_opt_set_int(pSwrContext, "out_channel_layout",    dst_ch_layout, 0);  
	av_opt_set_int(pSwrContext, "out_sample_rate",       out_sample_rate, 0);  
	av_opt_set_sample_fmt(pSwrContext, "out_sample_fmt", (AVSampleFormat)out_sample_fmt, 0);  

	swr_init(pSwrContext); 
	return true;
}

void CAudioImplBase::unitAudioResample()
{
	if(NULL==m_swrContext)
	{
		return;
	}

	if(swr_is_initialized(m_swrContext))
	{
		swr_close(m_swrContext);
	}

	swr_free(&m_swrContext);
	m_swrContext = NULL;
}

bool CAudioImplBase::resampleAudioData(BYTE* pInBuf,int nInSize,BYTE** ppOutBuf,int& nOutSize)
{
	if(NULL==pInBuf || 0 >= nInSize || NULL == ppOutBuf)
	{
		return false;
	}

	if(NULL == m_swrContext)
	{
		return false;
	}
	
	if(!swr_is_initialized(m_swrContext))
	{
		return false;
	}

	AVSampleFormat in_sample_fmt = AV_SAMPLE_FMT_S16;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;

	int in_sample_bit = 0;
	int out_sample_bit = 0;
	int in_sample_rate = 0; 
	int out_sample_rate = 0;
		
	int64_t src_nb_samples = 0;
	int64_t dst_nb_samples = 0;
	int64_t max_dst_nb_samples = 0; 

	int dst_nb_channels = 0; 
	int src_nb_channels = 0;

	int dst_linesize = 0;  	

	uint8_t ** dst_data = NULL;  
	int resampled_data_size = 0; 

	int data_size = 0;
	int func_ret = 0;
	bool do_resample = false;

	getResampleParam(in_sample_bit,src_nb_channels,in_sample_rate ,out_sample_bit,dst_nb_channels,out_sample_rate);

	in_sample_fmt = fromSampleBit(in_sample_bit);
	out_sample_fmt = fromSampleBit(out_sample_bit);

	if(in_sample_fmt == AV_SAMPLE_FMT_NONE || out_sample_fmt == AV_SAMPLE_FMT_NONE)
	{
		return false;
	}

	src_nb_samples = nInSize / (src_nb_channels * (in_sample_bit/8));

	dst_nb_samples = av_rescale_rnd(src_nb_samples, out_sample_rate, in_sample_rate, AV_ROUND_UP);  
	max_dst_nb_samples = dst_nb_samples;
	if (max_dst_nb_samples <= 0)  
	{	  
		return false;  
	}

	func_ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,(int)dst_nb_samples, out_sample_fmt, 0);  
	if (func_ret < 0)  
	{
		return false;  
	} 

	dst_nb_samples = av_rescale_rnd(swr_get_delay(m_swrContext, in_sample_rate) +  src_nb_samples, out_sample_rate, in_sample_rate,AV_ROUND_UP);  
	if (dst_nb_samples <= 0)  
	{
		goto _end; 
	}

	if (dst_nb_samples > max_dst_nb_samples)  
	{  
		av_free(dst_data[0]); 		
		if (av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,(int)dst_nb_samples, out_sample_fmt, 1) < 0)  
		{
			goto _end; 
		}

		max_dst_nb_samples = dst_nb_samples;  
	}  

	data_size = av_samples_get_buffer_size(NULL, src_nb_channels,(int)src_nb_samples, in_sample_fmt, 1);  
	if (data_size <= 0)  
	{	
		goto _end; 
	}

	resampled_data_size = data_size;  

	func_ret = swr_convert(m_swrContext, dst_data, (int)dst_nb_samples,(const uint8_t **)&pInBuf, (int)src_nb_samples);  
	if (func_ret <= 0)  
	{	
		goto _end;  
	}  

	resampled_data_size = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels, func_ret, (AVSampleFormat)out_sample_fmt, 1);  
	if (resampled_data_size <= 0)  
	{  
		goto _end;  
	}

	*ppOutBuf = (BYTE*)malloc(resampled_data_size);
	if(NULL == *ppOutBuf)
	{
		goto _end;
	}

	//将值返回去  
	memcpy(*ppOutBuf,dst_data[0],resampled_data_size);

	nOutSize = resampled_data_size;
	do_resample = true;

_end:	
	if (dst_data)  
	{  
		if(dst_data[0])
		{
			av_freep(&dst_data[0]);  
		}

		av_freep(&dst_data);
		dst_data = NULL; 
	}  

	return do_resample;
}

//////////////////////////////////////////////////////////////////////////
/// IMMNotificationClient
///

ULONG STDMETHODCALLTYPE CAudioImplBase::AddRef()  
{  
	return InterlockedIncrement(&m_refCount);  
}  

ULONG STDMETHODCALLTYPE CAudioImplBase::Release()  
{  
	ULONG ref = InterlockedDecrement(&m_refCount);  
	if (0 == ref)  
	{  
		delete this;  
	}

	return ref;  
}

HRESULT STDMETHODCALLTYPE CAudioImplBase::QueryInterface(REFIID riid, VOID **ppInterface)  
{  
	if (IID_IUnknown == riid)  
	{  
		AddRef();  
		*ppInterface = (IUnknown*)this;  
	}  
	else if (__uuidof(IMMNotificationClient) == riid)  
	{  
		AddRef();  
		*ppInterface = (IMMNotificationClient*)this;  
	}  
	else  
	{  
		*ppInterface = NULL;  
		return E_NOINTERFACE;  
	}

	return S_OK;  
}

HRESULT STDMETHODCALLTYPE CAudioImplBase::OnDefaultDeviceChanged(EDataFlow flow, ERole role,LPCWSTR pwstrDeviceId)   
{   
	return S_OK;   
}

HRESULT STDMETHODCALLTYPE CAudioImplBase::OnDeviceAdded(LPCWSTR pwstrDeviceId)   
{   
	return S_OK;   
}

HRESULT STDMETHODCALLTYPE CAudioImplBase::OnDeviceRemoved(LPCWSTR pwstrDeviceId)   
{
	return S_OK;   
}

HRESULT STDMETHODCALLTYPE CAudioImplBase::OnDeviceStateChanged(LPCWSTR pwstrDeviceId,DWORD dwNewState)   
{
	if(wcscmp(pwstrDeviceId,m_deviceId) != 0)
	{
		return S_OK;
	}
 
 	int available = false;
 	if(dwNewState == DEVICE_STATE_ACTIVE)
 	{
 		available = DEVICE_STATE_ENABLE;
 	}
 	else
 	{
 		available = DEVICE_STATE_DISABLE;
 	}

 	if(m_procDeviceState)
 	{
 		(*m_procDeviceState)(available);
 	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CAudioImplBase::OnPropertyValueChanged(LPCWSTR pwstrDeviceId,const PROPERTYKEY key)   
{   
	return S_OK;   
}
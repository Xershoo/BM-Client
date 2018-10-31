#include "stdafx.h"
#include "PlaybackRecord.h"
#include "AudioRecord.h"
#include "AudioEngine.h"
#include <math.h>

#define MAX_TIME_DIFFER		(20)

CPlaybackRecord::CPlaybackRecord(IMMDevice* pDevice):CAudioImplBase(pDevice,AUDIO_IMPL_CAP_SPK)
	,m_karokMic(-1)
	,m_audioRecord(NULL)
	,m_swrContextRecord(NULL)
	,m_mixAudioThread(NULL)
	,m_eventStopMix(NULL)
{
	::InitializeCriticalSectionAndSpinCount(&m_crRecPacketList,4000);
	::InitializeCriticalSectionAndSpinCount(&m_crPbkPacketList,4000);
}

CPlaybackRecord::~CPlaybackRecord()
{
	setKarok(false,-1);
	freePlaybackPacketList();

	::DeleteCriticalSection(&m_crRecPacketList);
	::DeleteCriticalSection(&m_crPbkPacketList);
}

bool CPlaybackRecord::open()
{	
	m_eventStopMix = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	if(NULL==m_eventStopMix)
	{
		return false;
	}

	m_mixAudioThread = (HANDLE)_beginthreadex(NULL,0,&mixAuidoThreadProc,(void*)this,0,NULL);
	if(NULL==m_mixAudioThread)
	{
		return false;
	}

	return CAudioImplBase::open();
}

void CPlaybackRecord::close()
{
	setKarok(false,0);
	if(m_eventStopMix)
	{
		::SetEvent(m_eventStopMix);
	}

	if(m_mixAudioThread)
	{
		WaitForSingleObject(m_mixAudioThread,INFINITE);
		SAFE_CLOSE_HANDLE(m_mixAudioThread);
	}

	SAFE_CLOSE_HANDLE(m_eventStopMix);

	CAudioImplBase::close();
}

void CPlaybackRecord::setKarok(bool useKarok,int micIndex)
{
	if(m_karokRecord == useKarok && micIndex == m_karokMic)
	{
		return;
	}

	freeRecordPacketList();
	unitRecordResample();

	if(m_audioRecord)
	{
		m_audioRecord->setKarokRecord(NULL);
		SAFE_RELEASE(m_audioRecord);
	}

	m_karokRecord = useKarok;
	m_karokMic = micIndex;

	if(!useKarok)
	{
		return;
	}

	CAudioEngine * audioEngine = CAudioEngine::createInstance();
	if(NULL == audioEngine)
	{
		return;
	}

	m_audioRecord = (CAudioRecord*)audioEngine->getMicAudioImpl(micIndex);
	if(NULL == m_audioRecord)
	{
		return;
	}

	m_audioRecord->setKarokRecord(this);
	initRecordResample();

	return;
}

void CPlaybackRecord::freeRecordPacketList()
{
	CAutoLock autoLock(&m_crRecPacketList);

	while(!m_listRecordPacket.empty())
	{
		LPAUDIOPACKET audioPacket =  dynamic_cast<LPAUDIOPACKET> (*m_listRecordPacket.begin());
		SAFE_DELETE(audioPacket);
		m_listRecordPacket.pop_front();
	}
}

void CPlaybackRecord::freePlaybackPacketList()
{
	CAutoLock autoLock(&m_crPbkPacketList);

	while(!m_listPlaybackPacket.empty())
	{
		LPAUDIOPACKET audioPacket =  dynamic_cast<LPAUDIOPACKET> (*m_listPlaybackPacket.begin());
		SAFE_DELETE(audioPacket);
		m_listPlaybackPacket.pop_front();
	}
}


bool CPlaybackRecord::isRecordNeedResample()
{
	if(NULL == m_audioRecord)
	{
		return false;
	}

	int in_sample_bit = 0;
	int in_sample_rate = 0; 
	int in_channels = 0;

	m_audioRecord->getDeviceParam(in_channels,in_sample_bit,in_sample_rate);
	if (m_setChannel != in_channels || 
		m_setSampleBit != in_sample_bit ||
		m_setSampleRate != in_sample_rate)
	{
		return true;
	}

	return false;
}

void CPlaybackRecord::initRecordResample()
{
	unitRecordResample();

	if(!isRecordNeedResample())
	{
		return;
	}

	if(NULL == m_audioRecord)
	{
		return;
	}

	m_swrContextRecord = swr_alloc();
	if(NULL==m_swrContextRecord)
	{
		return;
	}

	int in_sample_bit = 0;
	int in_sample_rate = 0; 
	int in_channels = 0;

	m_audioRecord->getDeviceParam(in_channels,in_sample_bit,in_sample_rate);

	if(!initSwrContext(m_swrContextRecord,in_sample_bit,in_channels,in_sample_rate,m_setSampleBit,m_setChannel,m_setSampleRate))
	{
		return;
	}
}

void CPlaybackRecord::unitRecordResample()
{
	if(NULL==m_swrContextRecord)
	{
		return;
	}

	if(swr_is_initialized(m_swrContextRecord))
	{
		swr_close(m_swrContextRecord);
	}

	swr_free(&m_swrContextRecord);
	m_swrContextRecord = NULL;
}


bool CPlaybackRecord::resampleRecordAudioData(BYTE* pInBuf,int nInSize,BYTE** ppOutBuf,int& nOutSize)
{
	if(NULL==pInBuf || 0 >= nInSize || NULL == ppOutBuf)
	{
		return false;
	}

	if(NULL==m_audioRecord)
	{
		return false;
	}

	if(NULL == m_swrContextRecord)
	{
		return false;
	}

	if(!swr_is_initialized(m_swrContextRecord))
	{
		return false;
	}
	
	AVSampleFormat in_sample_fmt = AV_SAMPLE_FMT_S16;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;

	int in_sample_bit = 0;
	int in_sample_rate = 0; 
	int src_nb_channels = 0;

	int out_sample_rate = m_setSampleRate;
	int out_sample_bit = m_setSampleBit;
	int dst_nb_channels = m_setChannel; 

	int64_t src_nb_samples = 0;
	int64_t dst_nb_samples = 0;
	int64_t max_dst_nb_samples = 0; 
	int dst_linesize = 0;  	

	uint8_t ** dst_data = NULL;  
	int resampled_data_size = 0; 

	int data_size = 0;
	int func_ret = 0;
	bool do_resample = false;

	m_audioRecord->getDeviceParam(src_nb_channels,in_sample_bit,in_sample_rate);
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

	dst_nb_samples = av_rescale_rnd(swr_get_delay(m_swrContextRecord, in_sample_rate) +  src_nb_samples, out_sample_rate, in_sample_rate,AV_ROUND_UP);  
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

	func_ret = swr_convert(m_swrContextRecord, dst_data, (int)dst_nb_samples,(const uint8_t **)&pInBuf, (int)src_nb_samples);  
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

void CPlaybackRecord::setParam(int channel,int sampleBit,int sampleRate)
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

UINT CPlaybackRecord::doAuidoImpl()
{
	if(NULL == m_audioClient)
	{
		return 0;
	}

	IAudioCaptureClient* pAudioCaptureClient = NULL;
	HANDLE	eventWait[2] = {m_eventStop,m_audioImplEvent};

	UINT32  nBufFrames = 0;
	HRESULT hr = S_OK;	
	BYTE *	pData = NULL;
	UINT32	nNextPacketSize = 0;
	UINT	nNumFramesToRead = 0;
	DWORD	dwFlags = 0;
	LARGE_INTEGER tBegin = {0};
	LARGE_INTEGER tEnd = {0};
	LARGE_INTEGER tFreq = { 0 };

	if(!initAudioResample())
	{
		goto _end;
	}

	hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_LOOPBACK, m_hnsDuration, m_hnsDuration, m_audioFormat, 0);
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

	hr = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
	if(FAILED(hr) || NULL == pAudioCaptureClient) 
	{
		goto _end;
	}

	m_audioImpl = pAudioCaptureClient;

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

	QueryPerformanceFrequency(&tFreq);
	QueryPerformanceCounter(&tBegin);

	while(true)
	{	
		DWORD wait = WaitForMultipleObjects(sizeof(eventWait)/sizeof(eventWait[0]), eventWait, FALSE, (DWORD)(m_hnsDuration / REFTIMES_PER_MILLISEC));		
		if(WAIT_OBJECT_0 == wait) 
		{
			break;
		}

		bool done = false;		
		LONGLONG tCount = 0;

		QueryPerformanceCounter(&tEnd);
		tCount = tEnd.QuadPart - tBegin.QuadPart;
		tBegin = tEnd;

		do 
		{
			hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
			if(FAILED(hr))
			{
				break;
			}

			if (nNextPacketSize == 0) 
			{
				//填充静音数据
				if(!done)
				{	
					LONGLONG timeSpace = tCount * 100 / tFreq.QuadPart;
					int nDataSize = (int)((m_audioFormat->nAvgBytesPerSec * timeSpace) / 100);
					
					pData = (BYTE *)malloc(nDataSize);
					memset(pData,0,nDataSize);

					putRecordDataToList(pData,nDataSize);
					m_curVolume = 0;
					
					SAFE_FREE(pData);
				}

				done = true;

				break;
			}

			hr = pAudioCaptureClient->GetBuffer(&pData,&nNumFramesToRead,&dwFlags,NULL,NULL);
			if(FAILED(hr))
			{				
				break;
			}

			if( dwFlags&AUDCLNT_BUFFERFLAGS_SILENT)
			{
				int nDataSize = nNumFramesToRead * m_audioFormat->nBlockAlign;
				memset(pData,0,nDataSize);
			}

			if(0 < nNumFramesToRead)
			{
				putRecordDataToList(pData,nNumFramesToRead * m_audioFormat->nBlockAlign);
				calcPcmVolume(pData,nNumFramesToRead * m_audioFormat->nBlockAlign);
			}

			pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);

			done = true;
		} while(::WaitForSingleObject(m_eventStop,1) == WAIT_TIMEOUT);

		if(!done)
		{
			break;
		}
	}

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

void CPlaybackRecord::putKarokRecordData(BYTE* pbData,int nSize,ULONGLONG timeStamp)
{
	if (!m_karokRecord || NULL == pbData || 0 >= nSize || NULL == m_audioRecord)
	{
		return;
	}

	BYTE *	dataAudio = NULL;
	int		dataSize = 0;
	bool	ret = false;
	bool	resample = isRecordNeedResample();
	
	if(resample)
	{
		ret = resampleRecordAudioData(pbData,nSize,&dataAudio,dataSize);
		if(!ret)
		{
			return;
		}

		if( NULL == dataAudio || dataSize <= 0)
		{
			return;
		}
	}
	else
	{
		dataAudio = pbData;
		dataSize = nSize;
	}

	LPAUDIOPACKET audioPacket = NULL;
	CAutoLock autoLock(&m_crRecPacketList);

	do
	{
		audioPacket = new AUDIOPACKET;
		if(NULL == audioPacket)
		{
			break;
		}
		
		if(!audioPacket->pushDataAll(dataAudio,dataSize))
		{
			break;
		}

		audioPacket->timeStamp = timeStamp;
		m_listRecordPacket.push_back(audioPacket);
		ret = true;
	}while(false);

	if(!ret)
	{
		SAFE_DELETE(audioPacket);
	}

	if(resample)
	{
		SAFE_FREE(dataAudio);
		dataSize = 0;
	}

	return;
}

void CPlaybackRecord::putRecordDataToList(BYTE* pbData,int nSize)
{	
	if(NULL == pbData || nSize <=0)
	{
		return;
	}

	BYTE *	dataAudio = NULL;
	int		dataSize = 0;
	bool	ret = false;
	bool	resample = isNeedResample();
	ULONGLONG timeStamp = GetTickCount64();

	if(resample)
	{
		ret = resampleAudioData(pbData,nSize,&dataAudio,dataSize);
		if(!ret)
		{
			return;
		}

		if( NULL == dataAudio || dataSize <= 0)
		{
			return;
		}
	}
	else
	{
		dataAudio = pbData;
		dataSize = nSize;
	}

	LPAUDIOPACKET audioPacket = NULL;
	CAutoLock autoLock(&m_crPbkPacketList);

	do
	{
		audioPacket = new AUDIOPACKET;
		if(NULL == audioPacket)
		{
			break;
		}

		if(!audioPacket->pushDataAll(dataAudio,dataSize))
		{
			break;
		}

		audioPacket->timeStamp = timeStamp;
		m_listPlaybackPacket.push_back(audioPacket);
				
		ret = true;
	}while(false);
	
	if(!ret)
	{
		SAFE_DELETE(audioPacket);
	}

	if(resample)
	{
		SAFE_FREE(dataAudio);
		dataSize = 0;
	}

	return;
}

bool CPlaybackRecord::getKarokRecordData(ULONGLONG timeStamp,BYTE* pbData, int nSize)
{
	if(!m_karokRecord)
	{
		return false;
	}

	if(NULL == m_audioRecord || NULL==pbData|| 0>= nSize)
	{
		return false;
	}
		
	CAutoLock autoLock(&m_crRecPacketList);
	if(m_listRecordPacket.empty())
	{	
		return false;
	}

	BYTE* recordData = pbData;
	INT   recordSize = 0;

	ULONGLONG timeLength = (ULONGLONG)(nSize * 1000 * 8)/ (ULONGLONG)(m_setChannel * m_setSampleRate * m_setSampleBit);
	ULONGLONG timeMin = timeStamp - MAX_TIME_DIFFER;
	ULONGLONG timeMax = timeStamp + timeLength + MAX_TIME_DIFFER;

	// get new buffer
	while(!m_listRecordPacket.empty())
	{
		LPAUDIOPACKET audioPacket = dynamic_cast<LPAUDIOPACKET>(*m_listRecordPacket.begin());
		if(NULL==audioPacket)
		{
			m_listRecordPacket.pop_front();
			continue;
		}
 				
 		if(audioPacket->timeStamp < timeMin)		//over time,give up packet
 		{
 			m_listRecordPacket.pop_front();
 			SAFE_DELETE(audioPacket);
 
 			continue;
 		}

		if(audioPacket->timeStamp > timeMax)		//time no arrived,it should no be possible
		{	
			break;
		}

		int packSize = audioPacket->sizeAudio;
		int copySize = audioPacket->popData(recordData,nSize - recordSize);
		if(copySize==0)
		{
			m_listRecordPacket.pop_front();
			SAFE_DELETE(audioPacket);
			
			break;
		}

		recordSize += copySize;
		recordData += copySize;

		if(copySize == packSize)
		{
			m_listRecordPacket.pop_front();
			SAFE_DELETE(audioPacket);
		}
		else
		{
			int timeCome = (copySize * 1000 * 8)/ (m_setChannel * m_setSampleRate * m_setSampleBit);
			if(audioPacket)
			{
				audioPacket->timeStamp += timeCome;
			}
		}
		
		if(recordSize == nSize)
		{
			break;
		}
	}

	return recordSize > 0 ? true:false;
}

bool CPlaybackRecord::getPlaybackData(BYTE** ppData, int& nSize,ULONGLONG& timeStamp)
{
	if(NULL == ppData)
	{
		return false;
	}

	*ppData = NULL;
	nSize = 0;

	const UINT DEF_BUF_SIZE = 25600;

	BYTE *	audioBuf = NULL;
	int		bufSize = DEF_BUF_SIZE;
	int		getSize = 0;

	CAutoLock autoLock(&m_crPbkPacketList);
	if(m_listPlaybackPacket.size() < 20)
	{
		return false;
	}

	timeStamp = 0;			
	audioBuf = (BYTE*)malloc(DEF_BUF_SIZE);
	if(NULL == audioBuf)
	{
		return false;
	}
	
	while(m_listPlaybackPacket.size() > 5)
	{
		LPAUDIOPACKET audioPacket = dynamic_cast<LPAUDIOPACKET>(*m_listPlaybackPacket.begin());
		m_listPlaybackPacket.pop_front();

		if(NULL==audioPacket || audioPacket->dataAudio == NULL || audioPacket->sizeAudio <= 0)
		{
			SAFE_DELETE(audioPacket);
			continue;
		}

		if(timeStamp == 0)
		{
			timeStamp = audioPacket->timeStamp;
		}

		if((getSize + audioPacket->sizeAudio) > bufSize)
		{
			bufSize = getSize + audioPacket->sizeAudio;
			audioBuf = (BYTE*)realloc(audioBuf,bufSize);
		}

		int copySize = audioPacket->popData(audioBuf+getSize,audioPacket->sizeAudio);
		SAFE_DELETE(audioPacket);
		getSize += copySize;
	}

	*ppData = audioBuf;
	nSize = getSize;

	return true;
}

UINT __stdcall CPlaybackRecord::mixAuidoThreadProc(void* lParam)
{
	CPlaybackRecord* recordPlayback =(CPlaybackRecord*)lParam;
	if(NULL != recordPlayback)
	{
		return recordPlayback->doMixAudio();
	}
	
	return 0;
}

UINT CPlaybackRecord::doMixAudio()
{
	BYTE *	audioData = NULL;
	int		audioSize = 0;

	BYTE *	recordData = NULL;
	int		recordSize = 0;

	BYTE *  playbackData = NULL;
	int		playbackSize = 0;

	ULONGLONG startTime = 0;

	while(WaitForSingleObject(m_eventStopMix,100) != WAIT_OBJECT_0)
	{
		bool mixer = false;
		bool ret = getPlaybackData(&playbackData,playbackSize,startTime);
		if(!ret)
		{
			continue;
		}

		if(m_karokRecord)
		{
			recordSize = playbackSize;
			recordData = (BYTE*)malloc(recordSize);
			memset(recordData,NULL,recordSize);
			 
			if(getKarokRecordData(startTime,recordData,recordSize))
			{
				mixerAudioData(playbackData,playbackSize,recordData,recordSize,&audioData,audioSize);
				mixer = true;
			}
			else
			{
				audioData = playbackData;
				audioSize = playbackSize;
			}

			SAFE_FREE(recordData);
			recordSize = 0;
		}
		else
		{
			audioData = playbackData;
			audioSize = playbackSize;
		}

		//put to list
		putAudioDataToListEx(audioData,audioSize);

		SAFE_FREE(playbackData);
		playbackSize = 0;

		if(mixer)
		{
			SAFE_FREE(audioData);
			audioSize = 0;
		}
	}

	return 0;
}

void CPlaybackRecord::putAudioDataToListEx(BYTE* pbData,int dataSize)
{	
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

void CPlaybackRecord::mixerAudioData(BYTE* srcDataA,int nSizeA,BYTE* srcDataB,int nSizeB,BYTE** destData,int& destSize)
{
	if((NULL==srcDataA||nSizeA<=0)&&(NULL==srcDataB||nSizeB<=0))
	{
		return;
	}

	destSize = nSizeA >= nSizeB ? nSizeA : nSizeB;
	BYTE* mixData = (BYTE*)malloc(destSize);
	
	double r = 1.0;
	for(int i=0;i<destSize/2;i++)
	{
		short a = 0;
		short b = 0;
		short d = 0;

		if(nSizeA > i * 2)
		{
			a = *(short*)(srcDataA + i*2);
		}

		if(nSizeB > i * 2)
		{
			b = *(short*)(srcDataB + i*2);
		}

		if( a < 0 && b < 0)  
		{
			d = a+b - (a * b / -(pow((double)2,(int)(16-1))-1));
		}
		else  
		{
			d = a+b - (a * b / (pow((double)2,(int)(16-1))-1));
		}

		 *(short*)(mixData + i*2) = d;
	}

	*destData = mixData;
}
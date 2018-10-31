#ifndef _AUDIO_IMPL_BASE_H
#define _AUDIO_IMPL_BASE_H

#include "voice_was_interface.h"
#include "AudioInclude.h"
#include "AudioPacket.h"
#include <list>

using namespace std;

typedef struct SwrContext SwrContext;
typedef std::list<LPAUDIOPACKET>  AudioPacketList;

class CAudioImplBase : public IWasAudioImpl,
	public IMMNotificationClient
{
protected:
	enum audio_impl_type
	{
		AUDIO_IMPL_RENDER = 0,		
		AUDIO_IMPL_CAP_MIC,
		AUDIO_IMPL_CAP_SPK,		
	};

public:
	CAudioImplBase(IMMDevice* pDevice,audio_impl_type type);
	virtual ~CAudioImplBase();

public:	
	//for IWasAudioImpl
	virtual void setMute(bool mute);			//设置静音
	virtual bool getMute();						//是否静音

	virtual void setVolume(int volume);			//设置音量
	virtual int  getVolume();					//获取音量

	virtual void setParam(int channel,int sampleBit,int sampleRate);			//设置音频参数
	virtual void getParam(int& channel,int& sampleBit,int& sampleRate);			//获取设置音频参数
	virtual void getDeviceParam(int& channel,int& sampleBit,int& sampleRate);
	virtual void setAudioStateCallback(PDeviceStateProc,PAudioStateProc);

	virtual bool open();						//打开
	virtual void close();						//关闭
	virtual bool isOpen();						//判断是否打开

	virtual int getCurrentVolume();				//获取实时音量

	//for speaker 
	virtual void putRenderData(BYTE* pbData,int dataSize);			//for render

	virtual int	getRecordData(BYTE* pbData,int dataSize);			//for record

	//for microphone
	virtual void setAec(bool useAec);		//设置回声消除
	virtual void setPreview(bool preview,int spkIndex);

	//for playback record
	virtual void setKarok(bool useKarok,int micIndex);	//设置卡拉OK

	virtual void release() 
	{ 
		this->Release();
	};

	virtual void addRef()
	{
		this->AddRef();
	};
	
	virtual IMMDevice* getDevice()
	{
		IMMDevice* pDevice = m_audioDevice;
		if(NULL!= pDevice)
		{
			pDevice->AddRef();
		}

		return pDevice;
	}
public:
	//for IMMNotificationClient
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface);
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role,LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);   
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId,DWORD dwNewState);
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId,const PROPERTYKEY key);

public:
	void freeAudioPacketList();

protected:
	void initAudioImpl();
	void freeAudioImpl();
		
	bool AdjustFormatTo16Bits(WAVEFORMATEX * fmtWave);

	bool isNeedResample();
	bool initAudioResample();
	bool initSwrContext(SwrContext* pSwrContext ,int in_sample_bit,int in_channels ,int in_sample_rate ,int out_sample_bit,int out_channels,int out_sample_rate);
	bool resampleAudioData(BYTE* pInBuf,int nInSize,BYTE** ppOutBuf,int& nOutSize);
	void unitAudioResample();
	virtual void getResampleParam(int& in_sample_bit,int& in_channels ,int& in_sample_rate ,int& out_sample_bit,int& out_channels,int& out_sample_rate);

	UINT getAudioDataFromList(BYTE* pbData,int nSize);
	void putAudioDataToList(BYTE* pbData,int nSize);

	void calcPcmVolume(BYTE* pbData,int nSize);
	
	AVSampleFormat fromSampleBit(int sampleBit);
protected:
	static UINT _stdcall audioThreadProc(void* lParam);
	virtual UINT doAuidoImpl() = 0;

protected:
	bool			m_open;
	long			m_refCount;
	WCHAR			m_deviceId[MAX_PATH];
	
	HANDLE			m_threadHandle;	
	HANDLE			m_eventStart;
	HANDLE			m_eventStop;

	IMMDevice*		m_audioDevice;
	IAudioClient*	m_audioClient;
	IUnknown*		m_audioImpl;

	HANDLE			m_audioImplEvent;	
	WAVEFORMATEX*	m_audioFormat;
	REFERENCE_TIME	m_hnsDuration;

	int					m_setChannel;
	int					m_setSampleBit;
	int					m_setSampleRate;

	SwrContext *		m_swrContext;
	AudioPacketList		m_listAudioPacket;
	CRITICAL_SECTION	m_crstPacketList;

	PDeviceStateProc	m_procDeviceState;
	PAudioStateProc		m_procAudioState;

	BYTE*				m_outAudioBuf;
	int					m_outAudioSize;

	int					m_curVolume;

	bool				m_initialize;
	bool				m_aecRecord;
	bool				m_karokRecord;
	bool				m_setPreview;
	
	audio_impl_type		m_typeAudio;
};

#endif
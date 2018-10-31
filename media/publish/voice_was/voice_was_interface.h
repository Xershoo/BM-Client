#ifndef _VOICE_WAS_INTERFACE_H_
#define _VOICE_WAS_INTERFACE_H_

#include "common_macro.h"

#ifdef VOICE_WAS_EXPORTS
#define VOIVE_WAS_INTERFACE_API	__declspec(dllexport)
#else
#define VOIVE_WAS_INTERFACE_API	__declspec(dllimport)
#endif

enum
{
	AUDIO_STATE_START = 0,
	AUDIO_STATE_STOP,
};

enum
{
	DEVICE_STATE_ENABLE = 0,
	DEVICE_STATE_DISABLE,
};

typedef void (CALLBACK *PAudioStateProc)(int state);
typedef void (CALLBACK *PDeviceStateProc)(int state);

class IWasAudioImpl
{
public:
	virtual void setMute(bool mute) = 0;			//设置静音
	virtual bool getMute() = 0;						//是否静音

	virtual void setVolume(int volume) = 0;			//设置音量
	virtual int  getVolume() = 0;					//获取音量

	virtual void setParam(int channel,int sampleBit,int sampleRate) = 0;			//设置音频参数
	virtual void getParam(int& channel,int& sampleBit,int& sampleRate) = 0;			//获取设置音频参数
	virtual void setAudioStateCallback(PDeviceStateProc,PAudioStateProc) = 0;

	virtual bool open() = 0;						//打开
	virtual void close() = 0;						//关闭
	virtual bool isOpen() = 0;

	virtual int getCurrentVolume() = 0;				//获取实时音量

	//for speaker 
	virtual void putRenderData(BYTE* pbData,int dataSize) = 0;			//for render
	
	//microphone or playback
	virtual int	getRecordData(BYTE* pbData,int dataSize) = 0;			//for record

	//for microphone
	virtual void setAec(bool useAec) = 0;		//设置回声消除
	virtual void setPreview(bool preview,int spkIndex) = 0;

	//for playback
	virtual void setKarok(bool useKarok,int micIndex) = 0;	//设置卡拉OK

	virtual void addRef() = 0;
	virtual void release() = 0;
};

class IWasVoiceEngine
{
public:
	virtual int getMicCount() = 0;						//获取麦克风设备的数量
	virtual int getSpkCount() = 0;						//获取扬声器设备的数量
	
	virtual void getSpkName(int index,wchar_t* name,int length) = 0;		//获取扬声器设备的名称
	virtual void getMicName(int index,wchar_t* name,int length) = 0;		//获取麦克风设备的名称

	virtual int getDefaultMic() = 0;			//获取默认麦克风设备
	virtual int getDefaultSpk() = 0;			//获取默认扬声器设备

	virtual void setDefaultMic(int index) = 0;	//设置默认麦克风设备
	virtual void setDefaultSpk(int index) = 0;	//设置默认麦克风设备

	virtual IWasAudioImpl* getMicAudioImpl(int index) = 0;		//获取麦克风使用接口
	virtual IWasAudioImpl* getSpkAudioImpl(int index) = 0;		//获取扬声器使用接口
	virtual IWasAudioImpl* getPlaybackImpl(int spkIndex) = 0;		//获取扬声器回放采集接口
};

VOIVE_WAS_INTERFACE_API IWasVoiceEngine* CreateWasVoiceEngine();

#endif
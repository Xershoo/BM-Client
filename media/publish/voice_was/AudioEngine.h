#ifndef _AUDIO_ENGINE_H
#define _AUDIO_ENGINE_H

#include "AudioInclude.h"
#include "AudioRecord.h"
#include "AudioRender.h"
#include "PlaybackRecord.h"
#include <list>

using namespace std;

typedef list<CAudioImplBase*>  CAudioImplList;

class CAudioEngine : public IWasVoiceEngine
{
protected:
	CAudioEngine();
	virtual ~CAudioEngine();

public:
	static CAudioEngine* createInstance();
	static void freeInstance();

public:
	virtual int getMicCount();						//获取麦克风设备的数量
	virtual int getSpkCount();						//获取扬声器设备的数量

	virtual void getSpkName(int index,wchar_t* name,int length);		//获取扬声器设备的名称
	virtual void getMicName(int index,wchar_t* name,int length);		//获取麦克风设备的名称

	virtual int getDefaultMic();			//获取默认麦克风设备
	virtual int getDefaultSpk();			//获取默认扬声器设备

	virtual void setDefaultMic(int index);	//设置默认麦克风设备
	virtual void setDefaultSpk(int index);	//设置默认麦克风设备

	virtual IWasAudioImpl* getMicAudioImpl(int index);		//获取麦克风使用接口
	virtual IWasAudioImpl* getSpkAudioImpl(int index);		//获取扬声器使用接口

	virtual IWasAudioImpl* getPlaybackImpl(int spkIndex);
	
	virtual int getMicIndex(LPWSTR pwszDevId);
	virtual int getSpkIndex(LPWSTR pwszDevId);
protected:
	bool initialize();
	void release();

	void getDeviceName(IMMDeviceCollection* devCol,int index,wchar_t* name,int length);
	UINT getDefaultDevice(IMMDeviceCollection* devCol,EDataFlow eFlow);
	void getDeviceId(IMMDeviceCollection* devCol,UINT deviceIndex,LPWSTR deviceId,int lengthId);
	void getDeviceId(IMMDevice* device,LPWSTR deviceId,int lengthId);

	void setDefaultDevice(LPCWSTR devID);

	void initMicrophoneList();
	void initSpeakerList();

	void freeAudioImplList(CAudioImplList& listAudioImpl);
	CAudioImplBase* findAudioImpl(CAudioImplList& listAudioImpl,LPWSTR devId);
protected:
	IMMDeviceEnumerator* m_audioDeviceEnumer;
	IMMDeviceCollection* m_audioMicCollect;
	IMMDeviceCollection* m_audioSpkCollect;

	CAudioImplList		m_listMicrophoeImpl;
	CAudioImplList		m_listSpeakerImpl;
	CAudioImplList		m_listPlaybackImpl;

	long  m_refCount;
protected:
	static CAudioEngine * m_instance;
};

#endif
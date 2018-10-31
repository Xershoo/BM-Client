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
	virtual int getMicCount();						//��ȡ��˷��豸������
	virtual int getSpkCount();						//��ȡ�������豸������

	virtual void getSpkName(int index,wchar_t* name,int length);		//��ȡ�������豸������
	virtual void getMicName(int index,wchar_t* name,int length);		//��ȡ��˷��豸������

	virtual int getDefaultMic();			//��ȡĬ����˷��豸
	virtual int getDefaultSpk();			//��ȡĬ���������豸

	virtual void setDefaultMic(int index);	//����Ĭ����˷��豸
	virtual void setDefaultSpk(int index);	//����Ĭ����˷��豸

	virtual IWasAudioImpl* getMicAudioImpl(int index);		//��ȡ��˷�ʹ�ýӿ�
	virtual IWasAudioImpl* getSpkAudioImpl(int index);		//��ȡ������ʹ�ýӿ�

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
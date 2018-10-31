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
	virtual void setMute(bool mute) = 0;			//���þ���
	virtual bool getMute() = 0;						//�Ƿ���

	virtual void setVolume(int volume) = 0;			//��������
	virtual int  getVolume() = 0;					//��ȡ����

	virtual void setParam(int channel,int sampleBit,int sampleRate) = 0;			//������Ƶ����
	virtual void getParam(int& channel,int& sampleBit,int& sampleRate) = 0;			//��ȡ������Ƶ����
	virtual void setAudioStateCallback(PDeviceStateProc,PAudioStateProc) = 0;

	virtual bool open() = 0;						//��
	virtual void close() = 0;						//�ر�
	virtual bool isOpen() = 0;

	virtual int getCurrentVolume() = 0;				//��ȡʵʱ����

	//for speaker 
	virtual void putRenderData(BYTE* pbData,int dataSize) = 0;			//for render
	
	//microphone or playback
	virtual int	getRecordData(BYTE* pbData,int dataSize) = 0;			//for record

	//for microphone
	virtual void setAec(bool useAec) = 0;		//���û�������
	virtual void setPreview(bool preview,int spkIndex) = 0;

	//for playback
	virtual void setKarok(bool useKarok,int micIndex) = 0;	//���ÿ���OK

	virtual void addRef() = 0;
	virtual void release() = 0;
};

class IWasVoiceEngine
{
public:
	virtual int getMicCount() = 0;						//��ȡ��˷��豸������
	virtual int getSpkCount() = 0;						//��ȡ�������豸������
	
	virtual void getSpkName(int index,wchar_t* name,int length) = 0;		//��ȡ�������豸������
	virtual void getMicName(int index,wchar_t* name,int length) = 0;		//��ȡ��˷��豸������

	virtual int getDefaultMic() = 0;			//��ȡĬ����˷��豸
	virtual int getDefaultSpk() = 0;			//��ȡĬ���������豸

	virtual void setDefaultMic(int index) = 0;	//����Ĭ����˷��豸
	virtual void setDefaultSpk(int index) = 0;	//����Ĭ����˷��豸

	virtual IWasAudioImpl* getMicAudioImpl(int index) = 0;		//��ȡ��˷�ʹ�ýӿ�
	virtual IWasAudioImpl* getSpkAudioImpl(int index) = 0;		//��ȡ������ʹ�ýӿ�
	virtual IWasAudioImpl* getPlaybackImpl(int spkIndex) = 0;		//��ȡ�������طŲɼ��ӿ�
};

VOIVE_WAS_INTERFACE_API IWasVoiceEngine* CreateWasVoiceEngine();

#endif
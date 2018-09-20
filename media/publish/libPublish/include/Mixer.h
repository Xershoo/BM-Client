#ifndef MIXER_H
#define MIXER_H

#include "windows.h"
#include <mmsystem.h>

#pragma comment (lib, "winmm.lib")
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
enum MixerDeice
{
	SPEAKERS=0,
	WAVEOUT,
	SYNTHESIZER,
	MICROPHONE
};

class CMixer 
{
public:
	CMixer();
	CMixer(const int VolRange);
	virtual ~CMixer();

public:
	bool GetMute(MixerDeice dev);               //����豸�Ƿ���
	bool SetMute(MixerDeice dev,bool vol);     //�����豸����
	bool SetVolume(MixerDeice dev,long vol);   //�����豸������
	unsigned GetVolume(MixerDeice dev);        //�õ��豸������dev=0��������1WAVE ,2MIDI ,3 LINE IN
private:
	bool GetVolumeControl(HMIXER hmixer ,long componentType,long ctrlType,MIXERCONTROL* mxc);
	bool SetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc, long volume);
	bool SetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc, bool mute);
	unsigned GetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc);
	long GetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc);
private:
	long m_VolRange ;
};
#endif

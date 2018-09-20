#include "Mixer.h"

CMixer::CMixer()
{
	m_VolRange = 100;
}

CMixer::~CMixer()
{

}

CMixer::CMixer(const int VolRange)
{
	m_VolRange = VolRange;
}
//----------------------------�趨����---------------------------------------

bool CMixer::GetVolumeControl(HMIXER hmixer ,long componentType,long ctrlType,MIXERCONTROL* mxc)
{
	MIXERLINECONTROLS mxlc;
	MIXERLINE mxl;
	bool exist = false;
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = componentType;

	if (componentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
	{//��ȡ¼����˷��豸
	    mxl.dwComponentType   =   MIXERLINE_COMPONENTTYPE_DST_WAVEIN;   
	    //   �õ�¼�������е�������   
	    mixerGetLineInfo(   (HMIXEROBJ)hmixer,   &mxl,   
		MIXER_OBJECTF_HMIXER   |   MIXER_GETLINEINFOF_COMPONENTTYPE   );   
	    //   ������������   
	    DWORD   dwConnections   =   mxl.cConnections;   
	    //   ׼����ȡ��˷��豸��ID   
	    DWORD   dwLineID   =   0;   
	    for   (   DWORD   i   =   0;   i   <   dwConnections;   i++   )   
	    {   
			//   ö��ÿһ���豸����Source��ID���ڵ�ǰ�ĵ�������   
			mxl.dwSource   =   i;   
			//   ����SourceID������ӵ���Ϣ   
			MMRESULT mr   =   mixerGetLineInfo(   (HMIXEROBJ)hmixer,   &mxl,   
			MIXER_OBJECTF_HMIXER   |   MIXER_GETLINEINFOF_SOURCE   );   
			//   �жϺ���ִ�д���   
			if(   mr   !=   0   )   
			{   
				break;   
			}   
			//   �����ǰ�豸��������˷磬������ѭ����   
			if(   mxl.dwComponentType   ==   MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE   )   
			{   
				exist = true;
				break;   
			}   
	    } 
	 }
	 else if(!mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
	 {
	     exist = true;
	 }

	 if (exist)
	 {
		  mxlc.cbStruct = sizeof(mxlc);
		  mxlc.dwLineID = mxl.dwLineID;
	      mxlc.dwControlType = ctrlType;
	      mxlc.cControls = 1;
	      mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	      mxlc.pamxctrl = mxc;
	      if(mixerGetLineControls((HMIXEROBJ)hmixer,&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE))
		      return 0;
	      else
		      return 1;
	 }
	return 0;
}
//---------------------------------------------------------------------------
long CMixer::GetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcd.hwndOwner = 0;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbDetails = sizeof(mxcdMute);
	mxcd.paDetails = &mxcdMute;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	if (mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd,MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
	   return -1;
	return mxcdMute.fValue;
}

//---------------------------------------------------------------------------
unsigned CMixer::GetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol; 
	vol.dwValue=0;
	mxcd.hwndOwner = 0;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	if(mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
	   return -1;
	return vol.dwValue;
}

//---------------------------------------------------------------------------
bool CMixer::SetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc, bool mute)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcdMute.fValue=mute;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(mxcdMute);
	mxcd.paDetails = &mxcdMute;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	if (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
	   return 0;
	return 1;
}

//---------------------------------------------------------------------------

bool CMixer::SetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc, long volume)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	vol.dwValue = volume;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	if(mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
	   return 0;
	return 1;
}


//---------------------------------------------------------------------------
unsigned /*WINAPI */CMixer::GetVolume(MixerDeice dev)//�õ��豸������dev=0��������1WAVE ,2MIDI ,3 LINE IN
{

	long device;
	unsigned rt=0;
	MIXERCONTROL volCtrl;
	HMIXER hmixer;
	switch (dev)
	{
		case WAVEOUT:
		  device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT; break;
		case SYNTHESIZER:
			 device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER; break;
		case MICROPHONE:
	   // device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break; // cd ����
			device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;   //��˷�����
	   // device=MIXERLINE_COMPONENTTYPE_SRC_LINE; break;         //PC ����������
	   //device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break;
  
		default:
			device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	}

	if(mixerOpen(&hmixer, 0, 0, 0, 0)) return 0;
	if(!GetVolumeControl(hmixer,device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
	{
		mixerClose(hmixer);
		return 0;
	}
	rt=GetVolumeValue(hmixer,&volCtrl)*m_VolRange /volCtrl.Bounds.lMaximum;
	mixerClose(hmixer);
	return rt;
}

//---------------------------------------------------------------------------

bool /*WINAPI*/ CMixer::SetVolume(MixerDeice dev,long vol)//�����豸������
{
	// dev =0,1,2   �ֱ��ʾ������,����,MIDI ,LINE IN
	// vol=0-m_VolRange      ��ʾ�����Ĵ�С , �����뷵��������ֵ�õ��ǰٷֱȣ���������0 - m_VolRange ���������豸�ľ���ֵ
	// retrun false ��ʾ���������Ĵ�С�Ĳ������ɹ�
	// retrun true ��ʾ���������Ĵ�С�Ĳ����ɹ�

	long device;
	bool rc=false;
	MIXERCONTROL volCtrl;
	HMIXER hmixer;
	switch (dev)
	{
	case WAVEOUT:
	   device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT; break;
	case SYNTHESIZER:
	   device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER; break;
	case MICROPHONE:
  
	   // device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break; // cd ����
	   device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;   //��˷�����
	   // device=MIXERLINE_COMPONENTTYPE_SRC_LINE; break;         //PC ����������
	   //device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break;
	default:
	   device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	}

	if(mixerOpen(&hmixer, 0, 0, 0, 0)) return 0;

	if(GetVolumeControl(hmixer,device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
	{
	   vol=vol*volCtrl.Bounds.lMaximum/m_VolRange ;
	   if(SetVolumeValue(hmixer,&volCtrl,vol))
		rc=true;
	}
	mixerClose(hmixer);
	return rc;
}

//---------------------------------------------------------------------------

bool /*WINAPI*/ CMixer::SetMute(MixerDeice dev,/*long*/bool vol)//�����豸����
{
	// dev =0,1,2   �ֱ��ʾ������,����,MIDI ,LINE IN
	// vol=0,1      �ֱ��ʾȡ������,���þ���
	// retrun false ��ʾȡ�������þ����������ɹ�
	// retrun true ��ʾȡ�������þ��������ɹ�

	long device;
	bool rc=false;
	MIXERCONTROL volCtrl;
	HMIXER hmixer;
	switch (dev)
	{
	case WAVEOUT:
	   device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT; break;
	case SYNTHESIZER:
	   device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER; break;
	case MICROPHONE:
	   // device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break; // cd ����
	   device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;   //��˷�����
	   // device=MIXERLINE_COMPONENTTYPE_SRC_LINE; break;         //PC ����������
	   //device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break;
	default:
	   device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	}

	if(mixerOpen(&hmixer, 0, 0, 0, 0)) return 0;
	if(GetVolumeControl(hmixer,device,MIXERCONTROL_CONTROLTYPE_MUTE,&volCtrl))
	   if(SetMuteValue(hmixer,&volCtrl,(bool)vol))
		rc=true;
	 mixerClose(hmixer);
	 return rc;
}

//---------------------------------------------------------------------------

bool /*WINAPI */CMixer::GetMute(MixerDeice dev)//����豸�Ƿ���
{
	//dev =0,1,2 �ֱ��ʾ������������,MIDI ,LINE IN
	// retrun false ��ʾû�о���
	// retrun true ��ʾ����
	long device;
	bool rc=false;
	MIXERCONTROL volCtrl;
	HMIXER hmixer;
	switch (dev)
	{
	case WAVEOUT:
		device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT; break;
	case SYNTHESIZER:
		device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER; break;
	case MICROPHONE:  
	   // device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break; // cd ����
	   //device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;   //��˷�����
	   // device=MIXERLINE_COMPONENTTYPE_SRC_LINE; break;         //PC ����������
	   //device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC; break;
	   device=MIXERLINE_COMPONENTTYPE_DST_WAVEIN; break;
	default:
		device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	}

	if(mixerOpen(&hmixer, 0, 0, 0, 0)) return 0;

	if(GetVolumeControl(hmixer,device,MIXERCONTROL_CONTROLTYPE_MUTE,&volCtrl))
	   rc=GetMuteValue(hmixer,&volCtrl);
	mixerClose(hmixer);
	return rc;
}

#ifndef MEDIACONFIG_H
#define MEDIACONFIG_H
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include "StreamHeader.h"


struct StreamParam
{

	VideoUnit  VU[4];

	int  nVUNum;
	int nVideoW;        //��Ƶ��
	int nVideoH;		//��Ƶ��
	int nVideoBitRate;	//��Ƶ����
	int nVideoFps;		//��Ƶ֡��

	HWND hShowHwnd;

	int nSelectMicID;//ѡ���id(0 <= id < GetDevMicCount())
	int bAudioKaraoke; 
	int nAudioBitRate; 
	int nAudioSmpRateCap;//��Ƶ����
	int nAudioChannels;

    
	bool bIsFullScreen; //�����Ƿ�ȫ��
	bool bDrawLocRect;
	bool bIsPublish;

	StreamParam()
	{
		memset(&VU,0,sizeof(VU));
		nVUNum = 0;
		nVideoBitRate = 300; // ??
		nVideoW = 640;
		nVideoH = 480;
		nVideoFps = 15;
		nSelectMicID = 0;
		bAudioKaraoke = 0;
		hShowHwnd = NULL;

		nAudioBitRate = 64000;
		nAudioSmpRateCap = 32000;
		nAudioChannels = 1;
		
		bIsFullScreen = true;
		bDrawLocRect = false;
		bIsPublish = true;
	}
};

class CMediaConfig
{
public:
	CMediaConfig();
	~CMediaConfig();
public:
	void copyMediaParam(PublishParam pparam,StreamParam &sparam,bool capScreen = false);
private:
	void getMediaConfig(MediabBusiness mb,MasterSlave ms,MediaRole mr,MediaLevel ml,AudioLevel al,SHOWSCALE ss,
		bool bIsManyCamera,bool bIsHighAudio,StreamParam &param);

	void getAnchorMainSmoothConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);
	void getAnchorMainStandConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);
	void getAnchorMainHighConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);
	void getAnchorMainMaxHighConfig(StreamParam &param,bool bIsManyCamera,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);

	void getAnchorSubSmoothConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);
	void getAnchorSubHighConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);
	void getAnchorSubStandConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);
	void getAnchorSubMaxHighConfig(StreamParam &param,bool bIsHighAudio,SHOWSCALE ss,AudioLevel al);

	void getListenerConfig(StreamParam &param,SHOWSCALE ss,AudioLevel al);
	void getListenerStandConfig(StreamParam &param,SHOWSCALE ss,AudioLevel al);
	void getListenerHighConfig(StreamParam &param,SHOWSCALE ss,AudioLevel al);
	
	void getCapScreenConfig(PublishParam pparam,StreamParam& param);
};

#endif
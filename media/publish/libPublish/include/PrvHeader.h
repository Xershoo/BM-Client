#pragma once
#include "SourAudio.h"
#include "h264encoder.h"
#include "aacencoder.h"
#include "SourCamera.h"
#include "SourScreen.h"
#include "SourFile.h"

#define MEDIAVIODE 1
#define MEDIAAUDIO 2

#define PUSHTONET  0x0001
#define SAVETOFILE 0x0010

struct SourceNode
{
	ISourData* pSD; //ָ��
	int nMediaType; //ý������
	int nUserType;
};

struct VideoEncoderNode
{
	int				nVideoW;        //��Ƶ��
	int				nVideoH;		//��Ƶ��
	int				nVideoBitRate;	//��Ƶ����
	int				nVideoFps;		//��Ƶ֡��
	CH264Encoder*	ph264Encoder;   
	void ReSet()
	{
		nVideoW = 0;
		nVideoH = 0;
		nVideoBitRate = 0;
		nVideoFps = 0;
		ph264Encoder = NULL;
	}
	VideoEncoderNode()
	{
		ReSet();
	}
};

struct PushFileNode
{
	char szFileName[260];
	HWND hPlayWnd;
	CSourFile*  pPlaySourFile;
	PushFileNode()
	{
		szFileName[0] = '\0';
		hPlayWnd = NULL;
		pPlaySourFile = NULL;
	}

	bool cmpFileIsPushing(const char* szFile)
	{
		if(strcmp(this->szFileName,szFile) == 0 && pPlaySourFile )
		{
			return true;
		}
		return false;
	}
};

struct AudioEncoderNode
{
	int				bAudioKaraoke; 
	int				nAudioBitRate; //��Ƶ����
	int				nAudioSmpRateCap;
	int             nAudioSmpRateEn;
	CAACEncoder*    paacEncoder;    
	void ReSet()
	{
		bAudioKaraoke = 0;
		nAudioBitRate = 0;
		nAudioSmpRateCap = 0;
		nAudioSmpRateEn = 0;
		paacEncoder = NULL;
	}
	AudioEncoderNode()
	{
		ReSet();
	}
};

#pragma once
#include "SourAudio.h"
#include "h264encoder.h"
#include "aacencoder.h"
#include "SourCamera.h"
#include "SourScreen.h"
#include "SourFile.h"

#define MEDIACAMERAVIODE 1 //摄像头视频
#define MEDIASCREENVIODE 2 //截屏
#define MEDIAMICAUDIO 3	  //声音
#define MEDIASPEKAUDIO 4	  //声音

#define PUSHTONET  0x0001
#define SAVETOFILE 0x0010

struct SourceNode
{
	ISourData* pSD; //指针
	int nMediaType; //媒体类型
	int nUserType;
	int nIndex;
	SourceNode()
	{
		pSD = NULL;
		nMediaType = -1;
		nUserType = -1;
		nIndex = -1;
	}
};

struct VideoEncoderNode
{
	int				nVideoW;        //视频宽
	int				nVideoH;		//视频高
	int				nVideoBitRate;	//视频码率
	int				nVideoFps;		//视频帧率
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
	int				nAudioBitRate; //音频采样
	int				nAudioSmpRateCap;
	CAACEncoder*    paacEncoder;    
	void ReSet()
	{
		bAudioKaraoke = 0;
		nAudioBitRate = 0;
		nAudioSmpRateCap = 0;
		paacEncoder = NULL;
	}
	AudioEncoderNode()
	{
		ReSet();
	}
};

enum STREAMPROTYPE
{
	RTMPTYPE=0,
	UDPTYPE=1,
	RTSPTYPE=2,
	HTTPTYPE=3
};

struct RtmpAddr
{
	char szKey[256];
	char szRealIP[32];
	unsigned short nPort;
	RtmpAddr()
	{
		szKey[0] = '\0';
		szRealIP[0] = '\0';
		nPort = 0;
	}
	void copy(const char* szAddrKey,const char* szAddrRealIP,unsigned short usPort)
	{
		strcpy(szKey,szAddrKey);
		strcpy(szRealIP,szAddrRealIP);
		nPort = usPort;
	}

	bool cmpIsMapping(const char* szAddrKey)
	{
		if(strcmp(szKey,szAddrKey) == 0)
		{
			return true;
		}
		return false;
	}

};

struct PicSourceNode
{
	ISourData* pSD;
	ScRect rc;
	PicSourceNode()
	{
		pSD = NULL;
		memset(&rc,0,sizeof(ScRect));
	}
};

struct PicLayer
{
	int nIndex; 
	int nW;
	int nH;
	unsigned char* pBuffer;
	unsigned int nBufferSize;
	ScRect rc;
	PicLayer()
	{
		nIndex = -1;
		nW = 0;
		nH = 0;
		pBuffer = NULL;
		nBufferSize = 0;
		memset(&rc,0,sizeof(ScRect));
	}
};

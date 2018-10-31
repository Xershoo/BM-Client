#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

typedef void* HIBITMAP;
typedef void*  HWNDHANDLE;

#define VIDEOTYPE 0x10000000
#define AUDIOTYPE 0x20000000

#define MPLATER_API  extern "C" __declspec(dllexport)

struct IMediaPlayerEvent
{
	virtual void ControlPaint() = 0;//播放器，底层绘制完交由上层再绘制相应部分
	virtual void ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH) = 0;
};

struct  PlayAddress
{
	int nMediaType;
	char szPushAddr[256];
	HWNDHANDLE hwnd;
	bool   bIsMainVideo;
	bool   bIsStudent;
	long   nUserID;
	PlayAddress()
	{
		szPushAddr[0] = '\0';
		nMediaType = 0;
		hwnd = NULL;
		bIsMainVideo = false;
		bIsStudent = true;
		nUserID = 0;
	}
};


//xiewb 2017.3.1
MPLATER_API bool   AVP_Init();
MPLATER_API void   AVP_Unit();

MPLATER_API bool   AVP_parsePalyAddrURL(const char* szPlayUrl,PlayAddress pa[4],int& nPaNum);

//播放
MPLATER_API bool   AVP_Play(const char* szPlayUrl,int nPlayStreamType,PlayAddress arrAddress[4],int narrAddressNum, HIBITMAP hBGbitmap, IMediaPlayerEvent* pCallback);

//播放
MPLATER_API bool   AVP_Change(const char* szPlayUrl,int nPlayStreamType,PlayAddress arrAddress[4],int narrAddressNum,HIBITMAP hBGbitmap);

//停止
MPLATER_API bool   AVP_Stop(const char* szPlayUrl,PlayAddress arrAddress[4],int narrAddressNum);


//播放文件
MPLATER_API bool   AVP_PlayFile(const char* szFileName,HWNDHANDLE hwnd,HIBITMAP hBGbitmap, IMediaPlayerEvent* pCallback);

//停止播放文件
MPLATER_API bool   AVP_StopFile(const char* szFileName,HWNDHANDLE hwnd);

//暂停
MPLATER_API bool   AVP_PauseFile(const char* szFileName,bool bIsPause);

//seek 文件
MPLATER_API bool   AVP_SeekFile(const char* szFileName,unsigned int  nPalyPos);
MPLATER_API bool   AVP_SeekFileStream(const char* szFileName,unsigned int  nPlayPos,bool bVideo);

//取得文件的总的播放时间长度
//参数：szFileName 本地音视频文件
//返回值：播放的时间总长度单位秒
MPLATER_API unsigned int  AVP_GetFileDuration(const char* szFileName);

//取得文件的当前播放的时间
//参数：szLocalFile 本地音视频文件
//返回值：播放的时间总长度单位秒
MPLATER_API unsigned int  getFileCurPlayTime(const char* szFileName);
MPLATER_API unsigned int  getFileStreamCurTime(const char* szFileName,bool bVideo);


MPLATER_API bool  playFileSwitch(const char* szCurPlayLocalFileName);

MPLATER_API bool  bIsExistVideoStream(const char* szCurPlayLocalFileName);
#endif
#ifndef STREAMHEADER_H
#define STREAMHEADER_H

#include "globalconfig.h"

//
#include "ave_global.h"
//#ifdef __cplusplus



#if defined LIBPUBLISH_EXPORTS
	#define MD_EXPORTS __declspec(dllexport)
#else 
	#define MD_EXPORTS __declspec(dllimport)
#endif 

#define VIDEOTYPE 0x10000000
#define AUDIOTYPE 0x20000000

#define SOURCECAMERA (VIDEOTYPE|0x01)   //摄像头视频
#define SOURCESCREEN (VIDEOTYPE|0x02)   //截屏视频
#define SOURCEDEVAUDIO (AUDIOTYPE|0x04) //音频
#define SOURCEDEMUSIC (AUDIOTYPE|0x08) //背景音乐音频

//DrawToolType
#define DRAWIAGER_SDL 1// sdl 显示图像
#define DRAWIAGER_DX  2// dx显示

enum SHOWSCALE
{
	SHOWSCALE4A3 = 1,
	SHOWSCALE16A9= 2
};

enum MDevStatus
{
	NORMAL = 0, //正常
	MUTE = 1,   //静音
	DEVERR = 2, //设备异常
	NODEV = 3,   //设备不存在
	CAMFORBID = 4, //摄像头被禁止
	MICFORBID   //麦克风被禁止
};

enum EventInfoType
{
	EVENT_HANDUP = 0,//举手
	EVENT_CANCLEHANDUP = 1, //取消举手
	TAKE_PHOTOS = 2, //拍照
	TRANCODE_FILE = 3,//转码
	EVENT_SHOW = 4 //显示
};

enum MediaLevel  //媒体的级别
{
	SMOOTHLEVEL = 0, //流畅
	STANDARRDLEVEL = 1, //标清
	HIGHLEVEL = 2, //高清
	SUPERHIGHLEVEL = 3 //超清
};

enum AudioLevel //音频级别
{
	AUDIOLOW = 0,
	AUDIONORMAL = 1,
	AUDIOMID = 2,
	AUDIOHIGH = 3
};

enum MediaRole //媒体角色
{
	ANCHORROLE = 0,//主播
	LISTENERROLE = 1 //听众
};

enum MediabBusiness //媒体业务
{
	EDUCATION = 0,//教育
	RECREATION = 1//娱乐
};

enum MasterSlave //主从机位
{
	MAINCAMERA=0,//主机位
	SUBCAMERA=1,//cong机位
};


enum DeviceType //设备类型
{
	Type_MICROPHONE = 1, //麦克风
	Type_CAMERA          //摄像头
};

enum DeviceState //设备状态
{
	State_ARRIVAL = 1, //插入
	State_REMOVAL      //拔出
};

struct ScRect
{
	int nTop;
	int nBottom;
	int nRight;
	int nLeft;
	ScRect()
	{
		nTop = 0;
		nBottom = 0;
		nRight = 0;
		nLeft = 0;
	}
};

struct VideoUnit
{
	int nSelectCameraID;//选择的id(0 <= id < GetDevCameraCount())
	int nType;
	ScRect showRect;

	int  nParam[2];
	int  nVEnMainID; 
	int  nVEnSubID;
	VideoUnit()
	{
		nType = SOURCECAMERA;
		nSelectCameraID = 0;//选择的id(0 <= id < GetDevCameraCount())
		memset(&showRect,0,sizeof(showRect));

		nParam[0] = 0;
		nParam[1] = 0;
		nVEnMainID = 0; 
	    nVEnSubID = 0;
	}
};

struct PublishParam
{
	MediaLevel ml; //媒体的级别
	MediaRole mr; //角色
	MediabBusiness mb; //业务
	MasterSlave ms; //主从
	AudioLevel  al;//音频级别
	SHOWSCALE   scale;//显示比例

	bool bIsManyCamera; //多视角
	VideoUnit  VU[4]; //视频单元
	int        nVUNum;
	HWND       hShowHwnd; //现实句柄

	bool bIsHighAudio; //是否需要高质量音频
	int  nSelectMicID; //mic id
	bool  bAudioKaraoke; //是否需要混音
	
	bool bIsFullScreen; //截屏是否全屏
	bool bDrawLocRect; //显示中心
	bool bIsPublish;   //是否推送网络

	PublishParam()
	{
		ml = SMOOTHLEVEL;
		mr = ANCHORROLE;
		mb = EDUCATION;
		ms = MAINCAMERA;
		al = AUDIONORMAL;
		scale = SHOWSCALE4A3;
		bIsManyCamera = false;
		nVUNum  = 0;
		hShowHwnd = NULL;
		bIsHighAudio = true;
		nSelectMicID = 0;
		bIsPublish = true;
		bAudioKaraoke = false;
		bIsFullScreen =true;
		bDrawLocRect = false;
	}
};

struct  PushAddress
{
	int nMediaType;
	char szPushAddr[256];
	PushAddress()
	{
		szPushAddr[0] = '\0';
		nMediaType = 0;
	}
};

struct TakePhptpsParam
{
	int nImageWidth;
	int nImageHeight;
	TakePhptpsParam()
	{
		nImageWidth = 0;
		nImageHeight = 0;
	}
};

struct ShowVideoParam
{
	int nImageWidth;
	int nImageHeight;
	HWND hShowHwnd;   //显示句柄
	ShowVideoParam()
	{
		nImageWidth = 0;
		nImageHeight = 0;
		hShowHwnd = NULL;
	}
};

struct TrancodeFileParam
{
	int  nCurTrancodePos; //当前转码的位置
	int  nTrancodeLen; //文件转码需要的总长度

	TrancodeFileParam()
	{
		nCurTrancodePos = 0;
		nTrancodeLen = 0;

	}
};




//事件回掉处理函数回调函数
typedef void (*scEventCallBack)(EventInfoType enmuType,void* dwUser,unsigned char* pBuf,unsigned int nBufSize,void* Param);

#endif
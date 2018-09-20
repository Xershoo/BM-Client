#include "stdafx.h"
#include "publishinterface.h"
#include "rtmppublish.h"
#include "MediaConfig.h"
CRTMPPublish* g_rtmpPublish = NULL;

//初始化lib运行环境
PUBLISH_MODULE_API void InitLibEnv()
{
	if(g_rtmpPublish == NULL)
	{
		HWND hShowHwnd = NULL;
		g_rtmpPublish = new CRTMPPublish(hShowHwnd);
		setSpeakersMute(false);
	}
}

//反初始化
PUBLISH_MODULE_API void   UninitLibEnv()
{
	if(g_rtmpPublish)
	{
		delete g_rtmpPublish;
		g_rtmpPublish = NULL;
	}
}

//功能:获取当前设备摄像头数目
PUBLISH_MODULE_API long   GetDevCameraCount()
{
	if(g_rtmpPublish)
	{
		return g_rtmpPublish->GetDevCameraCount();
	}
	return 0;
}

//功能：获取相关ID信息摄像头的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的摄像头相关联的名称
PUBLISH_MODULE_API bool   GetDevCameraName(int nCameraID,char szName[256])
{
	if(g_rtmpPublish)
		return g_rtmpPublish->GetDevCameraName(nCameraID,szName);
	return false;
}

//功能:获取当前设备麦克风数目
PUBLISH_MODULE_API long   GetDevMicCount()
{
	if(g_rtmpPublish)
	{
		return g_rtmpPublish->GetDevMicCount();
	}
	return 0;
}

//功能：获取相关ID信息麦克风的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的麦克风相关联的名称
PUBLISH_MODULE_API bool   GetDevMicName(int nMicID,char szName[256])
{
	if(g_rtmpPublish)
		return g_rtmpPublish->GetDevMicName(nMicID,szName);
	return false;
}


//功能:获取当前设备扬声器数目
PUBLISH_MODULE_API long   GetDevSpeakerCount()
{
	if(g_rtmpPublish)
	{
		return g_rtmpPublish->GetDevSpeakerCount();
	}
	return 0;
}

//功能：获取相关ID信息扬声器的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的扬声器的名称
PUBLISH_MODULE_API bool   GetDevSpeakerName(int nSpeakerID,char szName[256])
{
	if(g_rtmpPublish)
		return g_rtmpPublish->GetDevSpeakerName(nSpeakerID,szName);
	return false;
}

PUBLISH_MODULE_API bool  SetDevSpeaker(int index)
{
	if(g_rtmpPublish)
		return g_rtmpPublish->SetDevSpeaker(index);
	return false;
}


//功能：摄像头插入拔出消息
//参数: DeviceType,DeviceState,DeviceName, DeviceID
PUBLISH_MODULE_API bool   DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID)
{
	if(g_rtmpPublish)
		return g_rtmpPublish->DeviceChangeMsg(nDeviceType,nDeviceState,nDeviceName,DeviceID);
	return false;
}


//拍照
PUBLISH_MODULE_API bool   TakePhotos(const char* szUrl)
{
	if(g_rtmpPublish)
		return g_rtmpPublish->TakePhotos(szUrl);
	return false;
}

//设置外部显示的回掉函数
PUBLISH_MODULE_API bool SetShowEventCallback(scEventCallBack pEC,void* dwUser)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->SetShowEventCallback(pEC,dwUser);
}

PUBLISH_MODULE_API bool   parsePushAddrURL(const char* szRtmpPushUrl,PushAddress pa[4],int& nPaNum)
{
	if(pa == NULL || szRtmpPushUrl == NULL)
		return false;
	int i = 0;
	char szPushAddrHeader[128] = {'\0'};
	char szPushStreamID[128] = {'\0'};
	char szVideoURL[1024] = {'\0'};
	char *p = NULL;
	char *szSeparatorStream = "|&|";
	char *szSeparatorMedia = "|@|";
	p = (char*)strstr(szRtmpPushUrl,szSeparatorMedia);

	if(p != NULL)
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(pa[i].szPushAddr,p+strlen(szSeparatorMedia));
		pa[i++].nMediaType = AUDIOTYPE;
	}
	else
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}

	p = (char*)strstr(szVideoURL,szSeparatorStream);
	if(p)
	{
		strcpy(pa[i].szPushAddr,szVideoURL);
		strncpy(pa[i].szPushAddr,szVideoURL,p-szVideoURL);
		pa[i].szPushAddr[p-szVideoURL] = '\0';
		pa[i++].nMediaType = VIDEOTYPE;
		char *pp = p;
		p = p + strlen(szSeparatorStream);
		if(p)
		{
			
			int n = 0;
			int nLen = strlen(pa[i-1].szPushAddr);
			n = nLen;
			while(n > 0)
			{
				if(*pp == '\/')
				{
					strncpy(szPushAddrHeader,szVideoURL,pp-szVideoURL);
					szPushAddrHeader[pp-szVideoURL +1] = '\0';
					break;
				}
				pp--;
				n--;
			}
			char *q = p;
			char *Temp = p;
			p = strstr(q,szSeparatorStream);
			if(p)
			{
				strncpy(szPushStreamID,q,p-q);
				szPushStreamID[p-q+1] = '\0';
				sprintf(pa[i].szPushAddr,"%s/%s",szPushAddrHeader,szPushStreamID);
				pa[i++].nMediaType = VIDEOTYPE;
				p = p + strlen(szSeparatorStream);
				strcpy(szPushStreamID,p);
				sprintf(pa[i].szPushAddr,"%s/%s",szPushAddrHeader,szPushStreamID);
				pa[i++].nMediaType = VIDEOTYPE;
			}
			else
			{
				strcpy(szPushStreamID,q);
			}
		}
		
	}
	else
	{
		strcpy(pa[i].szPushAddr,szVideoURL);
		pa[i++].nMediaType = VIDEOTYPE;
	}
	nPaNum = i;
	return true;
}
//功能:rtmp 开始发布流到服务器
//参数:szRtmpPushUrl 发布流的地址
//	   nStreamType 发布流的类型，
//     nRoleType 角色的类型
//     Param 流相关的参数
//     hShowHwnd 本地视频的显示窗口句柄
PUBLISH_MODULE_API bool   rtmpPushStreamToServerBegin(const char* szRtmpPushUrl,int nStreamType,PublishParam Param)
{
	OutputDebugStringA("rtmpPushStreamToServerBegin..............\n");

	if(g_rtmpPublish== NULL || szRtmpPushUrl == NULL )
		return false;
	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}

	CMediaConfig  mc;
	StreamParam  sparam;
	mc.copyMediaParam(Param,sparam,(nStreamType&SOURCESCREEN) ==  SOURCESCREEN ? true : false);

	bool ret = false;
	if(strcmp(szAudioURL,"") == 0)
	{
		ret = g_rtmpPublish->PushStart(szRtmpPushUrl,nStreamType,sparam);
	}
	else
	{
		ret = g_rtmpPublish->PushStartEx(szVideoURL,szAudioURL,nStreamType,sparam);
	}

	if(ret)
	{
		OutputDebugStringA("rtmpPushStreamToServerBegin..............OK\n");
	}
	else
	{
		OutputDebugStringA("rtmpPushStreamToServerBegin..............failed\n");
	}

	return ret;
}


//功能:rtmp 结束发布流到服务器
//参数:szRtmpPushUrl 发布流的地址
PUBLISH_MODULE_API bool   rtmpPushStreamToServerEnd(const char* szRtmpPushUrl,bool bIsColseDev)
{
	OutputDebugStringA("rtmpPushStreamToServerEnd..................");
	if(g_rtmpPublish == NULL || szRtmpPushUrl == NULL)
		return false;	

	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}
	bool bIsAudioResult = true;
	bool bIsVideoResult = true;
	if(strcmp(szAudioURL,"") != 0)
	{
		 bIsAudioResult =  g_rtmpPublish->PushEnd(szAudioURL,bIsColseDev);
	}
	bIsVideoResult = g_rtmpPublish->PushEnd(szVideoURL,bIsColseDev);
	return (bIsAudioResult && bIsVideoResult);
}

//功能: 动态的切换发布到服务器流的格式
//参数:szRtmpPushUrl 发布流的地址
//	   nStreamType 发布流的类型，
//     nRoleType 角色的类型
//     Param 流相关的参数
//说明：这个函数可以在rtmp_PushStreamToServer_begin后，如果需要改变切换的流时可以调用此函数
PUBLISH_MODULE_API bool   rtmpPushStreamToServerChange(const char* szRtmpPushUrl,int nStreamType,PublishParam Param)
{
	if(g_rtmpPublish == NULL || szRtmpPushUrl == NULL)
		return false;	

	CMediaConfig  mc;
	StreamParam  sparam;
	mc.copyMediaParam(Param,sparam,(nStreamType&SOURCESCREEN) ==  SOURCESCREEN ? true : false);

	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}
	if(strcmp(szAudioURL,"") == 0)
		return  g_rtmpPublish->PushChange(szRtmpPushUrl,nStreamType,sparam);
	else
		return  g_rtmpPublish->PushChangeEx(szVideoURL,szAudioURL,nStreamType,sparam);
}


//移动图片到窗口的制定位置位置
//参数:szRtmpPushUrl 发布流的地址
//     nIndex 下标
//     nStreamType 下标对应的流类型
//     showRect  窗口的区域

PUBLISH_MODULE_API bool   moveImageShowRect(const char* szRtmpPushUrl,int nIndex,int nStreamType,ScRect showRect)
{
	if(g_rtmpPublish == NULL || szRtmpPushUrl == NULL)
		return false;	

	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}

	return g_rtmpPublish->moveImageShowRect(szVideoURL,nIndex,nStreamType,showRect);
}

PUBLISH_MODULE_API bool   rtmpPushFileToServerBegin(const char* szLocalFile,char* szRtmpServerURL,int nUseDrawToolType,HWND hShowHwnd)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	if(szRtmpServerURL && strncmp(szRtmpServerURL,"rtmp://",strlen("rtmp://")) != 0)
		return false;

	return g_rtmpPublish->PushFileToServerBegin(szLocalFile,szRtmpServerURL,nUseDrawToolType,hShowHwnd);
}

//功能:结束本地推送和播放文件
//参数：szLocalFile 本地音视频文件
PUBLISH_MODULE_API bool   rtmpPushFileToServerEnd(const char* szLocalFile)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL )
		return false;	
	return g_rtmpPublish->PushFileToServerEnd(szLocalFile);
}

PUBLISH_MODULE_API bool   rtmpPushFileSwitch(const char* szCurPlayLocalFileName)
{
	if(g_rtmpPublish == NULL || szCurPlayLocalFileName == NULL)
		return false;	
	return g_rtmpPublish->PushFileSwitch(szCurPlayLocalFileName);
}
//移动播放文件的位置
//参数：szLocalFile 本地音视频文件
//		nSeekTime   seek到的文件位置
//返回值：播放的时间总长度单位秒
PUBLISH_MODULE_API bool rtmpPushFileSeek(const char* szLocalFile,unsigned int nSeekTime)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->PushFileSeek(szLocalFile,nSeekTime);
}

//取得文件的d当前播放的时间
//参数：szLocalFile 本地音视频文件
//返回值：播放的时间总长度单位秒
PUBLISH_MODULE_API unsigned int getPushFileCurPlayTime(const char* szLocalFile)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->GetPushFileCurPlayTime(szLocalFile);
}

//取得文件的总的播放时间长度
//参数：szLocalFile 本地音视频文件
//返回值：播放的时间总长度单位秒
PUBLISH_MODULE_API unsigned int getPushFileDuration(const char* szLocalFile)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->GetPushFileDuration(szLocalFile);
}

//暂停/恢复
//参数：szLocalFile 本地音视频文件
//		bIsPause   暂停(true)/恢复(flase)
PUBLISH_MODULE_API bool rtmpPushFilePause(const char* szLocalFile,bool bIsPause)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->PushFilePause(szLocalFile,bIsPause);
}


//开始存本地推送的数据到本地的FLV文件
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;
	int nLen = strlen(szLocalFile) - 4;
	if(strcmp(szLocalFile+nLen,".flv") != 0)
		return false;

	return g_rtmpPublish->PushDataToLocalFlvFileBegin(szLocalFile,hRecordWindow);

}

//结束存本文件
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileEnd()
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->PushDataToLocalFlvFileEnd();
}

//设置本地麦克风的静音
//参数： vol true表示开启静音 false 表示关闭静音
PUBLISH_MODULE_API bool setMicMute(bool vol)
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->setMicMute(vol);
}

//设置本地麦克风的音量
//参数： vol范围:1--100
PUBLISH_MODULE_API bool setMicVolume(long vol)
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->setMicVolume(vol);
}

//获取本地麦克风的音量
//参数： vol范围:1--100
PUBLISH_MODULE_API long getMicVolume()
{
	if(g_rtmpPublish == NULL)
		return 0;
	return g_rtmpPublish->getMicVolume();
}

//获取实时麦克风的音量
//参数： vol范围:1--128
PUBLISH_MODULE_API int GetMicRTVolum()
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->GetMicRTVolum();
}

//获取本地扬声器播放的音量
//参数：vol表示音量大小（0-100）
PUBLISH_MODULE_API long getSpeakersVolume()
{
	if(g_rtmpPublish == NULL)
		return 0;
	return g_rtmpPublish->getSpeakersVolume();
}

//设置本地扬声器播放的静音
//参数： vol true表示开启静音 false 表示关闭静音
PUBLISH_MODULE_API bool setSpeakersMute(bool vol)
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->setSpeakersMute(vol);
}

//设置本地扬声器播放的音量
//参数：vol表示音量大小（0-100）
PUBLISH_MODULE_API bool setSpeakersVolume(long vol)
{
	if(g_rtmpPublish == NULL && (vol >= 0 && vol <= 100))
		return false;
	return g_rtmpPublish->setSpeakersVolume(vol);
}

//获取麦克风设备的状态
PUBLISH_MODULE_API MDevStatus getMicDevStatus()
{
	if(g_rtmpPublish == NULL)
		return NODEV;
	return g_rtmpPublish->getMicDevStatus();
}

//获取麦克风状态
PUBLISH_MODULE_API MDevStatus getCamDevStatus()
{
	if(g_rtmpPublish == NULL)
		return NODEV;
	return g_rtmpPublish->getCamDevStatus();
}

//获取扬声器状态
PUBLISH_MODULE_API MDevStatus getSpeakersDevStatus()
{
	if(g_rtmpPublish == NULL)
		return NODEV;
	return g_rtmpPublish->getSpeakersDevStatus();
}

//设置回掉函数
PUBLISH_MODULE_API bool SetEventCallback(scEventCallBack pEC,void* dwUser)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->SetEventCallback(pEC,dwUser);
}

//阅览声音
PUBLISH_MODULE_API  bool PreviewSound(int nMicID,bool bIsPreview)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->PreviewSound(nMicID,bIsPreview);
}

//转码文件
PUBLISH_MODULE_API bool TrancodeFile(const char* szSourFileName,const char* szDestFileName,char* szErr)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->TrancodeFile(szSourFileName,szDestFileName,szErr);
}

//转码文件
PUBLISH_MODULE_API bool StopTrancodeFile()
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->StopTrancodeFile();
}
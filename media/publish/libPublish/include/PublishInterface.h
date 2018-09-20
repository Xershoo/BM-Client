#ifndef PUBLISHINTERFACE_H
#define PUBLISHINTERFACE_H

#define PUBLISH_MODULE_API  extern "C" MD_EXPORTS

#include "StreamHeader.h"



//初始化lib运行环境
PUBLISH_MODULE_API void   InitLibEnv();

//反初始化
PUBLISH_MODULE_API void   UninitLibEnv();

//功能:获取当前设备摄像头数目
PUBLISH_MODULE_API long   GetDevCameraCount();

//功能：获取相关ID信息摄像头的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的摄像头相关联的名称
PUBLISH_MODULE_API bool   GetDevCameraName(int nCameraID,char szName[256]);


//功能:获取当前设备麦克风数目
PUBLISH_MODULE_API long   GetDevMicCount();

//功能：获取相关ID信息麦克风的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的摄像头相关联的名称
PUBLISH_MODULE_API bool   GetDevMicName(int nMicID,char szName[256]);

//功能:获取当前设备扬声器数目
PUBLISH_MODULE_API long   GetDevSpeakerCount();

//功能：获取相关ID信息扬声器的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的扬声器的名称
PUBLISH_MODULE_API bool   GetDevSpeakerName(int nSpeakerID,char szName[256]);


PUBLISH_MODULE_API bool   SetDevSpeaker(int index);

//功能：摄像头插入拔出消息
//参数: DeviceType,DeviceState,DeviceName, DeviceID
PUBLISH_MODULE_API bool   DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID);

//拍照
PUBLISH_MODULE_API bool   TakePhotos(const char* szUrl);


//设置外部显示的回掉函数
PUBLISH_MODULE_API bool SetShowEventCallback(scEventCallBack pEC,void* dwUser);


PUBLISH_MODULE_API bool   parsePushAddrURL(const char* szRtmpPushUrl,PushAddress pa[4],int& nPaNum);
//功能:rtmp 开始发布流到服务器
//参数:szRtmpPushUrl 发布流的地址
//	   nStreamType 发布流的类型，
//     nRoleType 角色的类型
//     Param 流相关的参数
PUBLISH_MODULE_API bool   rtmpPushStreamToServerBegin(const char* szRtmpPushUrl,int nStreamType,PublishParam Param);

//功能:rtmp 结束发布流到服务器
//参数:szRtmpPushUrl 发布流的地址
PUBLISH_MODULE_API bool   rtmpPushStreamToServerEnd(const char* szRtmpPushUrl,bool bIsColseDev = false);

//功能: 动态的切换发布到服务器流的格式
//参数:szRtmpPushUrl 发布流的地址
//	   nStreamType 发布流的类型，
//     nRoleType 角色的类型
//     Param 流相关的参数
//说明：这个函数可以在rtmp_PushStreamToServer_begin后，如果需要改变切换的流时可以调用此函数
PUBLISH_MODULE_API bool   rtmpPushStreamToServerChange(const char* szRtmpPushUrl,int nStreamType,PublishParam Param);

//移动图片到窗口的制定位置位置
//参数:szRtmpPushUrl 发布流的地址
//     nIndex 下标
//     nStreamType 下标对应的流类型
//     showRect  窗口的区域

PUBLISH_MODULE_API bool   moveImageShowRect(const char* szRtmpPushUrl,int nIndex,int nStreamType,ScRect showRect);

//功能：推送本地文件到远程服务器，同时本地播放
//参数：szLocalFile 本地音视频文件
//      szRtmpServerURL 远程退缩的服务器地址，可以为NULL这样就不推送
//      nUseDrawToolType  显示图书所借助的工具 DRAWIAGER_SDL 或者 DRAWIAGER_DX
//      hShowHwnd   文件本地播放的窗口句柄，可以为NULL（为NULL就自动的使用InitLibEnv 设置的窗口聚丙播放）
//返回值：成功返回true,失败返回flase
PUBLISH_MODULE_API bool   rtmpPushFileToServerBegin(const char* szLocalFile,char* szRtmpServerURL,int nUseDrawToolType,HWND hShowHwnd);


//功能:结束本地推送和播放文件
//参数：szLocalFile 本地音视频文件
PUBLISH_MODULE_API bool   rtmpPushFileToServerEnd(const char* szLocalFile);

//功能：播放切换
//当前正在播放的文件名称
PUBLISH_MODULE_API bool   rtmpPushFileSwitch(const char* szCurPlayLocalFileName);

//移动播放文件的位置
//参数：szLocalFile 本地音视频文件
//		nSeekTime   seek到的文件位置
//返回值：播放的时间总长度单位秒
PUBLISH_MODULE_API bool rtmpPushFileSeek(const char* szLocalFile,unsigned int nSeekTime);

//暂停/恢复
//参数：szLocalFile 本地音视频文件
//		bIsPause   暂停(true)/恢复(flase)
PUBLISH_MODULE_API bool rtmpPushFilePause(const char* szLocalFile,bool bIsPause);

//取得文件的当前播放的时间
//参数：szLocalFile 本地音视频文件
//返回值：播放的时间总长度单位秒
PUBLISH_MODULE_API unsigned int getPushFileCurPlayTime(const char* szLocalFile);

//取得文件的总的播放时间长度
//参数：szLocalFile 本地音视频文件
//返回值：播放的时间总长度单位秒
PUBLISH_MODULE_API unsigned int getPushFileDuration(const char* szLocalFile);


//开始存本地推送的数据到本地的FLV文件
//参数：szLocalFile 保存的文件路径
//      hRecordWindow 要记录的的句柄窗口
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow);

//结束保存文件
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileEnd();

//设置本地麦克风的静音
//参数： vol true表示开启静音 false 表示关闭静音
PUBLISH_MODULE_API bool setMicMute(bool vol);

//设置本地麦克风的音量
//参数： vol范围:1--100
PUBLISH_MODULE_API bool setMicVolume(long vol);

//获取本地麦克风的音量
//参数： vol范围:1--100
PUBLISH_MODULE_API long getMicVolume();

//得到Mic实时音量
PUBLISH_MODULE_API int GetMicRTVolum();

//获取本地扬声器播放的音量
//参数：vol表示音量大小（0-100）
PUBLISH_MODULE_API long getSpeakersVolume();

//设置本地扬声器播放的静音
//参数： vol true表示开启静音 false 表示关闭静音
PUBLISH_MODULE_API bool setSpeakersMute(bool vol);

//设置本地扬声器播放的音量
//参数：vol表示音量大小（0-100）
PUBLISH_MODULE_API bool setSpeakersVolume(long vol);

//获取麦克风设备的状态
PUBLISH_MODULE_API MDevStatus getMicDevStatus();

//获取麦克风状态
PUBLISH_MODULE_API MDevStatus getCamDevStatus();

//获取扬声器状态
PUBLISH_MODULE_API MDevStatus getSpeakersDevStatus();

//阅览声音
PUBLISH_MODULE_API  bool PreviewSound(int nMicID,bool bIsPreview);

//设置回掉函数
PUBLISH_MODULE_API bool SetEventCallback(scEventCallBack pEC,void* dwUser);

//转码文件
PUBLISH_MODULE_API bool TrancodeFile(const char* szSourFileName,const char* szDestFileName,char* szErr);

//转码文件
PUBLISH_MODULE_API bool StopTrancodeFile();
#endif
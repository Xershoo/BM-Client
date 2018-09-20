#ifndef MEDIAHEADER_H
#define MEDIAHEADER_H

#define SOURCECAMERA 0x01   //摄像头视频
#define SOURCEDEVAUDIO 0x08 //音频

struct StreamParam
{
	int nSelectCameraID;//选择的id(0 <= id < GetDevCameraCount())
	int nVideoW;        //视频宽
	int nVideoH;		//视频高
	int nVideoBitRate;	//视频码率
	int nVideoFps;		//视频帧率

	int bAudioKaraoke; 
	int nAudioBitRate; //音频采样
};

//功能:获取当前设备摄像头数目
long   GetDevCameraCount();

//功能：获取相关ID信息摄像头的名称
//参数: nCameraID id(0 <= id < GetDevCameraCount())
//		szName 获取到的摄像头相关联的名称
bool   GetDevCameraName(int nCameraID,char szName[256]);

//功能:rtmp 开始发布流到服务器
//参数:szRtmpPushUrl 发布流的地址
//	   nStreamType 发布流的类型，
//     nRoleType 角色的类型
//     Param 流相关的参数
//     hShowHwnd 本地视频的显示窗口句柄
bool   rtmp_PushStreamToServer_begin(const char* szRtmpPushUrl,int nStreamType,StreamParam Param,HWND hShowHwnd);

//功能:rtmp 结束发布流到服务器
//参数:szRtmpPushUrl 发布流的地址
bool   rtmp_PushStreamToServe_end(const char* szRtmpPushUrl);

#endif
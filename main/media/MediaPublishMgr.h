//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaPublishMgr.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.27
//	说明：rtmp流发布管理类定义头文件
//  接口：
//	修改记录：
//      
//**********************************************************************

#ifndef MEDIA_PUBLISH_MGR_H
#define MEDIA_PUBLISH_MGR_H

#include "RtmpPublisher.h"
#include "RtmpPlayer.h"
#include "MediaInitThread.h"

#include <string>
#include <vector>
#include <QtCore/QObject>

using namespace std;

#define MAX_RTMP_ADDR_NUM		(4)
#define MAX_PUSH_SEAT			(3)

enum CAMERA_TYPE
{
	CAMERA_LOCAL = 0,
	CAMERA_NETWORK,
	MICROPHONE_LOCAL,
    MICROPHONE_NETWORK,
    SPEAKER_LOCAL,
};

typedef struct MediaDeviceInfo
{
public:
	MediaDeviceInfo()
	{
		_id = 0;
		_name = "";
		_type = -1;
		_seat = -1;
	}

	int					_id;
	string		        _name;
	int					_type;	
	int					_seat;
}MEDIADEVICEINFO,*LPMEDIADEVICEINFO;

typedef struct PublishSeatInfo
{
public:
	enum
	{
		Publish_Seat_Unstart = 0,
		Publish_Seat_Starting,
		Publish_Seat_Started,
	};

	PublishSeatInfo()
	{
		_sid = -1;
		_url = "";
		_cid = -1;
		_ctype = -1;		
		_main = false;
        _state = Publish_Seat_Unstart;
        _rtmp = NULL;
	}

	int				_sid;
	string	        _url;
	int				_cid;
	int				_ctype;
	bool			_main;
    int             _state;
	int				_scale;
    CRTMPStream*    _rtmp;
}PUBLISHSEATINFO,*LPPUBLISHSEATINFO;

typedef vector<LPMEDIADEVICEINFO>	MediaDeviceList;
typedef vector<LPPUBLISHSEATINFO>	PublishSeatList;
typedef vector<CRTMPPlayer*>        CRTMPPlayerList;
typedef vector<CRTMPPublisher*>     CRTMPPublisherList;
typedef vector<CRtmpStreamCallback*>    CRtmpStreamCallbackList;

class CMediaPublishMgr : public QObject
{
    Q_OBJECT
public:
	CMediaPublishMgr();
	virtual ~CMediaPublishMgr();

	static CMediaPublishMgr* getInstance();
	static void freeInstance();

public:
    void setUserId(__int64 userId);
    __int64 getUserId();
    
public:
    void setSelCamera(int nCamera);
    void setSelMic(int nMic);
    void setSelSpeaker(int nSpeaker);

    inline int getSelCamera()
    {
        return m_selCarmer;
    };

    inline int getSelMic()
    {
        return m_selMic;
    };
    
    inline int getSelSpeaker()
    {
        return m_selSpeaker;
    };

public:     //多媒体一些其他相关接口
    bool setMicMute(bool vol);      //设置本地麦克风的静音
    bool setMicVolume(long vol);    //设置本地麦克风的音量
    long getMicVolume();            //获取本地麦克风的音量
    int GetMicRTVolum();            //得到Mic实时音量

    long getSpeakersVolume();       //获取本地扬声器播放的音量
    bool setSpeakersMute(bool vol); //设置本地扬声器播放的静音
    bool setSpeakersVolume(long vol);//设置本地扬声器播放的音量

    MDevStatus getMicDevStatus();   //获取麦克风设备的状态
    MDevStatus getCamDevStatus();   //获取麦克风状态
    MDevStatus getSpeakersDevStatus();//获取扬声器状态

    bool PreviewSound(int nMicID,bool bIsPreview);  //预览声音
public:    //url设置接口
    void setMediaUrl(LPCWSTR pwszPushUrl,LPCWSTR pwszPullUrl);
    void setPublishParam(bool isTeacher);

    string  getSeatPushUrl(int nIndex,bool bAudio = true);
    string  getSeatPullUrl(int nIndex,bool bAudio = true);

public:  //设备相关接口
	void loadLocalDevice();  //加载摄像头和麦克风本地设备       
    void addNetworkCamera(int nId,WCHAR* pszName); //添加网络摄像头
	void delNetworkCamera(int nId);//删除网络摄像头 0表示删除所有
    
    int	 getCameraCount();

    LPMEDIADEVICEINFO getMediaDevice(int nId,int nType);
    MediaDeviceList&  getMediaDeviceList();

    bool  isInitMedia();
public:  //RTMP发布相关   
    CRTMPPublisher * getRtmpPublisher(int nIndex);
    CRTMPPublisher * getRtmpPublisherByCamera(int nDevId);

	bool	startRecordScreen();
	void	stopRecordScreen();
public:   //机位相关接口
    void setMainSeat(int nIndex);
    int  getMainSeat();
    void setSeatVideoDevice(int nIndex,int nDevType,int nDevId);

    PublishSeatList& getPublishSeatList();
    LPPUBLISHSEATINFO getPublishSeatInfo(int nIndex);
	LPPUBLISHSEATINFO getPublishSeatInfo(int nDevType,int nDevId);

public:
    CRtmpStreamCallback* getCallback(CRTMPStream* rtmpStream);

public:
    void localVideoShow(unsigned char* pBuf,unsigned int nBufSize,ShowVideoParam* pVideoParam);

protected:
    void initPublisherList();
    void freePublisherList();

    void initPlayerList();
    void freePlayerList();

    void initPublishSeatList();
    void freePublishSeatList();

    void freeDeviceList();
    void freeCallbackList();

	void setRecordScreenUrl();
public:	
	void startAllSeatVideo();

    int  startSeatVideo(int nIndex);
    bool changeSeatVideo(int nIndex);
    void stopSeatVideo(int nIndex);

protected slots:
    void mediaInitFinish();    

signals:    
	void updatedPublishSeat(int seatIndex);
	void selCameraChanged(int nCamera);
	void startAllPublishVideoAsync();
	void startPublishSeatAsync(int nIndex);
	void initFinished();

protected:
    void updatePublishSeat(LPPUBLISHSEATINFO pSeat,bool restart = false);

protected:
	MediaDeviceList		m_lstCameraDevice;	
	PublishSeatList		m_lstPublishSeat; 
    CRTMPPlayerList     m_lstPlayer;
    CRTMPPublisherList  m_lstPublisher;
    CRtmpStreamCallbackList     m_listCallback;

	string      		m_urlPush;
	string      		m_urlPull;
    PushAddress			m_addrPush[MAX_RTMP_ADDR_NUM];
    PlayAddress			m_addrPull[MAX_RTMP_ADDR_NUM];
    int					m_addrPushNum;
    int					m_addrPullNum;

	char				m_szPushRecordUrl[MAX_PATH];
	bool				m_recordScreen;
    
    __int64             m_userId;
    UINT                m_selCarmer;
    UINT                m_selMic;
    
    int                 m_selSpeaker;
    int                 m_mainSeat;
    bool                m_initMedia;

    CMediaThread		m_mediaThread;
	bool				m_freeAll;
   
private:
	static CMediaPublishMgr*	m_pMediaPublishMgr;
};

#endif
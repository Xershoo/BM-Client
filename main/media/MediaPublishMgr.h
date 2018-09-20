//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�MediaPublishMgr.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.27
//	˵����rtmp�����������ඨ��ͷ�ļ�
//  �ӿڣ�
//	�޸ļ�¼��
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

public:     //��ý��һЩ������ؽӿ�
    bool setMicMute(bool vol);      //���ñ�����˷�ľ���
    bool setMicVolume(long vol);    //���ñ�����˷������
    long getMicVolume();            //��ȡ������˷������
    int GetMicRTVolum();            //�õ�Micʵʱ����

    long getSpeakersVolume();       //��ȡ�������������ŵ�����
    bool setSpeakersMute(bool vol); //���ñ������������ŵľ���
    bool setSpeakersVolume(long vol);//���ñ������������ŵ�����

    MDevStatus getMicDevStatus();   //��ȡ��˷��豸��״̬
    MDevStatus getCamDevStatus();   //��ȡ��˷�״̬
    MDevStatus getSpeakersDevStatus();//��ȡ������״̬

    bool PreviewSound(int nMicID,bool bIsPreview);  //Ԥ������
public:    //url���ýӿ�
    void setMediaUrl(LPCWSTR pwszPushUrl,LPCWSTR pwszPullUrl);
    void setPublishParam(bool isTeacher);

    string  getSeatPushUrl(int nIndex,bool bAudio = true);
    string  getSeatPullUrl(int nIndex,bool bAudio = true);

public:  //�豸��ؽӿ�
	void loadLocalDevice();  //��������ͷ����˷籾���豸       
    void addNetworkCamera(int nId,WCHAR* pszName); //�����������ͷ
	void delNetworkCamera(int nId);//ɾ����������ͷ 0��ʾɾ������
    
    int	 getCameraCount();

    LPMEDIADEVICEINFO getMediaDevice(int nId,int nType);
    MediaDeviceList&  getMediaDeviceList();

    bool  isInitMedia();
public:  //RTMP�������   
    CRTMPPublisher * getRtmpPublisher(int nIndex);
    CRTMPPublisher * getRtmpPublisherByCamera(int nDevId);

	bool	startRecordScreen();
	void	stopRecordScreen();
public:   //��λ��ؽӿ�
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
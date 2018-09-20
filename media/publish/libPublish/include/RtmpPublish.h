
// ScreenRecoderDlg.h : ͷ�ļ�
//

#pragma once

#include "PublishUnit.h"
#include "SaveFile.h"
#include "sc_CSLock.h"
#include "sc_Thread.h"
#include "lock.h"
#include "afxwin.h"
#include <list>
#include "Mixer.h"
#include "CUDPClient.h"
#include "PlayAudio.h"
#include "transcode.h"


using namespace std;

class CRTMPPublish
{
// ����
public:
	CRTMPPublish(HWND hShowHwnd);	// ��׼���캯��
	~CRTMPPublish();

public:
	bool PushStart(const char* szUrl,int nStreamType,StreamParam param);
	bool PushChange(const char* szUrl,int nStreamType,StreamParam param);
	bool PushEnd(const char* szUrl,bool bIsColseDev);

	bool PushChangeEx(const char* szVideoUrl,const char* szAudioUrl,int nStreamType,StreamParam param);
	bool PushStartEx(const char* szVideoUrl,const char* szAudioUrl,int nStreamType,StreamParam param);

	bool PushFileToServerBegin(const char* szLocalFile,char* szServerURL,int nUseDrawToolType,HWND hShowHwnd);
	bool PushFileToServerEnd(const char* szLocalFile);
	bool PushFileSeek(const char* szLocalFile,unsigned int nSeekTime);
	bool PushFilePause(const char* szLocalFile,bool bIsPause);
	unsigned int GetPushFileCurPlayTime(const char* szLocalFile);
	unsigned int GetPushFileDuration(const char* szLocalFile);
	bool PushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow);
	bool PushDataToLocalFlvFileEnd();
	bool PushFileSwitch(const char* szCurPlayLocalFileName);

	bool   moveImageShowRect(const char* szRtmpPushUrl,int nIndex,int nStreamType,ScRect showRect);
	bool TrancodeFile(const char* szSourFileName,const char* szDestFileName,char* szErr);
	bool StopTrancodeFile();

	long GetDevCameraCount();
	bool GetDevCameraName(long nIndex,char szName[256]);
	//����:��ȡ��ǰ�豸��˷���Ŀ
	long   GetDevMicCount();
    bool   GetDevMicName(int nMicID,char szName[256]);

	//���ܣ�����ͷ����γ���Ϣ
	long GetDevSpeakerCount();
	bool   GetDevSpeakerName(int nSpeakerID,char szName[256]);
	bool   SetDevSpeaker(int index);

	bool   DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID);

	void OnVideoTimer(UINT_PTR nIDEvent);
	void OnAudioTimer(UINT_PTR nIDEvent);

	void RecvAudio(VData* pvdata);

	bool setSpeakersMute(bool vol); //����
	bool setSpeakersVolume(long vol);//����

	bool setMicVolume(long vol);//������˷�����
	bool setMicMute(bool vol);//������˷羲��
	bool getMicMute();

	long getMicVolume();
	long getSpeakersVolume();
	//��ȡ��˷��豸��״̬
	MDevStatus getMicDevStatus();
	//��ȡ��˷�״̬
	MDevStatus getCamDevStatus();
	//��ȡ������״̬
	MDevStatus getSpeakersDevStatus();
	//��ȡ�������
	int    GetMicRTVolum();

	bool PreviewSound(int nMicID,bool bIsPreview);
	bool TakePhotos(const char* szUrl);
	
	void send_proc();
	void send_audio_proc();
	bool SetEventCallback(scEventCallBack pEC,void* dwUser);
	bool SetShowEventCallback(scEventCallBack pEC,void* dwUser);
	
private:
	bool OnInit();
	void OnUnInit();

	void PreviewSoundEx();
	int RTMP_SendPacket_Ex(CPublishUnit *pu, RTMPPacket *packet, int queue,int nUserType,bool bIsVideo=true);
	int UDP_SendPacket_Ex(CPublishUnit *pu,unsigned char* pBuf,unsigned nBufSize);
	CPublishUnit* FindPU(const char* szURL);
	CPublishUnit* FindPUAndRemove(const char* szURL);
	void SendMediaHeader(CPublishUnit *pu,bool bIsSend = false);

	int CalculateMicVolum(unsigned char* pBuf,unsigned int nBufSize);
private:
	HANDLE  m_hSendAudioThread;
	LONG	m_isAudioThreadRunning; 
	HANDLE  m_hVideoEvent;
	HANDLE  m_hAudioEvent;

	CLock			m_lock;
	int				m_iStartTime;
	CMutexLock      m_vLock;
	RTMPPacket      m_packetVideo;  
	RTMPPacket      m_packetAudio;  
	CMutexLock      m_aLock;
	UINT_PTR		m_nTimer;
	ITimerQueue		m_nAudioTimer;

private:
	list <CPublishUnit*> m_listPublish;
	CPublishUnit*        m_audioPublish;
	list<PushFileNode> m_listPushFile;   
	char m_szPlayFileName[512] ;
	CSaveFile *        m_saveFile;
	CTranscode*        m_trancode;
	bool               m_bIsTeancodeing;
private:
	CMixer   m_volMixer;
	CSourDataManager  m_sdManager;
	RTMP           *m_InitImpl;
	CPlayAudio     *m_playAudio;
	bool           m_bIsPreview;
	int            m_nMicVolum;
private:
	scEventCallBack  m_pEventCallBack;
	void *			 m_EventpUser;

	scEventCallBack  m_pShowEventCallBack;
	void *			 m_EShowventpUser;
};


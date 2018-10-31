#ifndef PLAYPROCESS_H
#define PLAYPROCESS_H

#include "PCMBuffer.h"
#include "playaudio.h"
#include "PlayVideoUnit.h"
#include "voice_was_interface.h"

#include <map>

using namespace std;
struct IMediaPlayerEvent;
#define MAXPLAYERNUM  24

class CPlayProcess;

struct ViedePlayNode
{
	CPlayProcess * pPlayProcess;
	CPlayVideoUnit* pVideoPlayUnit;
	ViedePlayNode()
	{
		pPlayProcess = NULL;
		pVideoPlayUnit = NULL;
	}
};

class CPlayProcess
{
public:
	CPlayProcess();
	~CPlayProcess();
public:
	bool  PlayMedia(const char* szURL,int nPlayStreamType,void* hPlayHand,void* hBGbitmap,bool bIsPlay,long nUserID,bool bIsMainVideo =false,bool bIsStudent=true,IMediaPlayerEvent* pCallback = NULL);
	bool  StopMedia(const char* szURL,void* hPlayHand);
	bool  StopALLMedia();
	bool  ChangeMedia(const char* szURL,int nPlayStreamType,HWNDHANDLE hwnd,HIBITMAP hBGbitmap,bool bIsPlay,long nUserID,bool bIsMainVideo =false,bool bIsStudent=true);

	bool  PlayFile(const char* szURL,void* hPlayHand,void* hBGbitmap);
	void  stopFile(const char* szURL);
	bool PauseFile(const char* szURL,bool bIsPause);
	bool SeekFile(const char* szURL,unsigned int nPalyPos);
	bool SeekFileStream(const char* szURL,unsigned int nPlayPos,bool bVideo);

	unsigned int GetFileDuration(const char* szURL);
	unsigned int getFileCurPlayTime(const char* szURL);
	unsigned int getFileStreamCurTime(const char* szURL,bool bVideo);

	bool SwitchPaly(const char* szURL,bool bIsFlag = false);
	bool getPalyStutas(const char* szURL);
	bool bIsHaveVideo(const char* szURL);

	static void  fill_audio(void *udata,unsigned char *stream,int len);
	void  SDLPlayAudio(void *udata,Uint8 *stream,int len);

	inline bool getIsPlayFile() {return  m_bIsPlayLocal;}

	void OnPlayTimer(int nEventID);//调用要回调的成员方法
	void OnReadMediaDataTimer(void * pUser);
	void OnReadAudioDataTimer(int nEventID);
	int  GetShowHandCount(const char* szURL);
	void SetCallBack(const char* szFileName,IMediaPlayerEvent* pCallback);

	static void static_SyncAudio(int64_t & nPlayAudioTime,bool& nHaveAudio,void* dwUser);
	void SyncAudio(int64_t & nPlayAudioTime,bool& nHaveAudio);
	bool  StopShowVideo(const char* szURL);

private:
	bool  DecodePCMDatatoAAC(bool bIsWaitFlag = true);
	void StopAudio();

	CNetProcess *CreateNetProcess(const char* szURL);
	void DestoryNetProcess(CNetProcess * np);
private:
	ViedePlayNode* findPVUnit(const char* szURL);
	ViedePlayNode* RemovePVUnit(const char* szURL);
	void           ADDPVUnit(ViedePlayNode* pvu);

protected:
	bool initWasAudioPlay();

private:
	char   m_szPlayAudioURL[512];
private:
	bool    m_bIsSeparate;
	LONG    m_isAudioPlaying;
	LONG    m_isVideoPlaying;
private:
	CNetProcess          *m_netAudioProcess;
	list<ViedePlayNode*>  m_playVideoUnit;	
	CMutexLock            m_playVideoLock;
	bool                  m_bIsPlayLocal; 

private:

	unsigned char *m_pFlvAudioParseBuf;
	unsigned int   m_nFlvAudioParseBufSize;
	unsigned int   m_FlvAudioParsePos;

	unsigned char  *m_pFlvAudioBuf;
	unsigned int    m_nFlvAudioBufSize;
private:
	CMediaDecoder            m_decoder;   
	CAudioJitterBuffer       m_audJitterBuf;
	CPCMBuffer               m_pcmLowbuffer;
	CPCMBuffer               m_pcmHighbuffer;
private:
	CPlayAudio*              m_playSound;
	IWasAudioImpl*			 m_playAudio;
	CMutexLock               m_sAudioLock;
private:
	int						m_samplerate;
	int						m_nchannel;
	DWORD                   m_nLastPlayPcmSysTime;
	DWORD                   m_nLastStopPcmSysTime;
	DWORD                   m_begPlayAudioSysTime;
	DWORD                   m_begPlayAudoPtsTime;
	DWORD                   m_nCurPlayAudoPtsTime;
	DWORD                   m_nPlayAudioSysDiffTime;
private:
	CSCThread		m_threadReadAudio;;
	int64_t			m_nPlayAudioTime;
	bool            m_bIsHaveAudio;
	bool            m_bIsOpenAudio;
	CMutexLock      m_PlayAudioLock;

	IMediaPlayerEvent* m_pCallback;
public:
	static CPlayProcess *g_PlayrArray[MAXPLAYERNUM];
	static CMutexLock    m_PlayrArrayLock;
	static int           m_nRefCount;
	static UINT_PTR		 m_nPlayAudioTimer;
};
#endif
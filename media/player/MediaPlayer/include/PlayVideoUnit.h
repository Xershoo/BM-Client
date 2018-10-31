#ifndef PLAYVIDEOUNIT_H
#define PLAYVIDEOUNIT_H

#include "ShowVideoByDX.h"
#include "VideoJitterBuffer.h"
#include "AudioJitterBuffer.h"

#include "rtmpprocess.h"
#include "udpprocess.h"
#include "MediaDecode.h"
#include "PlayFile.h"
#include "sc_CSLock.h"
#include <map>
using namespace std;

class CPlayVideoUnit;

struct ParseFlvParam
{
	CAudioJitterBuffer *audJitterBuf;
	bool                bIsAudio;
	bool                bIsAAC;
	int					samplerate;
	int					nchannel;  
	CPlayVideoUnit *    pPVU;

	ParseFlvParam()
	{
		audJitterBuf = NULL;
		bIsAudio = false;
		bIsAAC = false;
		samplerate = 0;
		nchannel = 0;
		pPVU = NULL;
	}
};

typedef void (* fEvent_Callback )(int64_t& nPlayAudioTime,bool & nHaveAudio,void* dwUser);

class CPlayVideoUnit
{
public:
	CPlayVideoUnit(fEvent_Callback fcallback,void* dwUser);

	~CPlayVideoUnit();
public:
	bool PlayMedia(const char* szURL,void* hPlayHand,void* hBGbitmap,ThreadProc thread_proc,void* pUserData,bool bIsMainVideo = false,IMediaPlayerEvent* pCallback = NULL);
	bool ChangeMedia(void* hwnd,void* hBGbitmap,bool bIsMainVideo = false);
	bool StopMeida(void* hPlayHand);

	int ReadMediaData();
	inline CVideoJitterBuffer* GetVideoJitterBuffer() {return &m_vidJitterBuf;}
	void OnShowVideo();
	int  GetShowHandCount();
	void netStopCommond();

	bool  PlayFile(const char* szURL,void* hPlayHand,void* hBGbitmap,play_audio_callback pCallback,void* dwUser,IMediaPlayerEvent* pVideoCallback = NULL);
	void  stopFile();
	bool PauseFile(bool bIsPause);
	bool SeekFile(unsigned int nPalyPos);
	bool SeekFileStream(unsigned int nPlayPos,bool bVideo);
	unsigned int GetFileDuration();
	unsigned int getFileCurPlayTime();
	unsigned int getFileStreamCurTime(bool bVideo);
	bool SwitchPaly(play_audio_callback pCallback,void* dwUser,bool bIsFlag = false);
	void  SDLPlayAudio(void *udata,Uint8 *stream,int len);
	bool getPalyStutas();

	inline const char* getPalyAddress() { return m_szPlayVideoURL ;}
	static int flvParse(unsigned char**pFlvPBuf,unsigned int& nFlvParseBufSize,unsigned int& nFlvParseBufPos,unsigned char * pBuf,unsigned int nBufSize,ParseFlvParam &pNode);
	static CNetProcess *CreateNetProcess(const char* szURL);
	static void DestoryNetProcess(CNetProcess * np);

	void  StopShowVideo();
	void  setPlayAudioTime(int64_t nPlayAudioTime,bool bIsHaveAudio);
	bool   bIsHaveVideo();

	void  PlayFileAndGetWaitTime(int& nSleepTime,VideoFrame&  vf);
	void  PlayLiveAndGetWaitTime(int& nSleepTime,VideoFrame&  vf);
    void SetCallBack(IMediaPlayerEvent* pCallback);
private:
	 CShowVideo*  FindByHand(HWND hShowWnd);
	 void*        GetShowHwnd();
	 void		  UpdateShowVideo(PBITMAP hBGbitmap,HWND hShowWnd,int nType);
	 void         AddShowVideo(PBITMAP hBGbitmap,HWND hShowWnd,int nType);
	 void         DelShowVideo(HWND hShowWnd);
	 void         ClearAllShowVideo();
	 void         DrawVideo(unsigned char* pBuf,unsigned int nBufSize,int nVH,int nVW,bool bIsPlayFile);
	 void         DrawBGImage();
	 int          flvParseVideo(unsigned char*pstart,unsigned int data_size,int nstmp);


public:
	unsigned char * m_pFlvParseBuf;
	unsigned int   m_nFlvParseBufSize;
	unsigned int   m_FlvParsePos;

	unsigned char  *m_pFlvBuf;
	unsigned int    m_nFlvBufSize;
	CSCThread	    m_threadReadMedia;
private:
	map<long,CShowVideo*>  m_mapShowHand;
	CMutexLock          m_mapShowLock;
	LONG				m_isVideoPlaying;

	CNetProcess *		m_netProcess;
	CSCThread			m_threadShowVideo;
	unsigned int        m_nLastReadMediaTime;

	CPlayFile			*m_playFile;
	bool				m_bIsPlayLocal;
private:


	CVideoJitterBuffer   m_vidJitterBuf;
	CMediaDecoder        m_decoder; 

	unsigned char m_H264PPSAndSPS[128];
	unsigned int  m_nH264PPSAndSPSSize;
private:
	int64_t			m_nPlayVideoTime;
	int64_t			m_nPlayAudioTime;
	DWORD           m_DrawBGBegin;
	bool            m_bIsHaveAudio;
	char			m_szPlayVideoURL[512];
	fEvent_Callback m_fcallback;
	void*			m_dwUser;
	bool			m_bIsMainVideo;
	bool            m_bIsShow;
	CMutexLock      m_ShowLock;

	IMediaPlayerEvent* m_pShowCallback;
};
#endif
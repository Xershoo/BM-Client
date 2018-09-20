#pragma once
#include "SourDataManager.h"
#include "flv_file.h"
#include "timerqueue.h"
using namespace biz_utility;

class CSaveFile
{
public:
	CSaveFile();
	~CSaveFile();
public:
	bool PushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow,CSourAudio*  pAudio);
	bool PushDataToLocalFlvFileEnd();

public:
	static unsigned CALLBACK SaveAudioCallBack(void* arg);
	void OnSaveVideoTimer(UINT_PTR nIDEvent);
	unsigned SaveAudioProc();
	void StartSendAudioThread();
	void StopSendAudioThread();

private:
	void SaveMediaMsgHeader();
	int SavePacketToFile(unsigned char* pBuf,unsigned int nBufSize,bool bIsVideo=true);

private:
	int             m_iSrcSaveVideoWidth;
	int             m_iSrcSaveVideoHeight;
	VideoEncoderNode m_vSaveExcEncoder;
	unsigned char*  m_szSaveRGBBuffer;
	unsigned char*  m_vBodyBuffer;
	unsigned char*  m_aBodyBuffer;
	CSourScreen *   m_pSourScreen;
	CFlvFile *		m_pFlvFile; 
	AudioEncoderNode m_aSaveCurExcEncoder;

private:
	CSourAudio      *m_pAudio;
	CMutexLock       m_aLock;
	AudioEncoderNode m_aCurExcEncoder;
	ULONG			 m_nAACInputSamples;
	BYTE*		     m_pbPCMBuffer;

private:
	HANDLE			m_hSendAudioThread;
	LONG			m_isAudioThreadRunning; 
	LONG			m_isCapturing;
	HANDLE			m_hVideoEvent;
	CLock			m_lock;
	DWORD		    m_iStartTime;
	ITimerQueue		m_nTimer;
	CMutexLock      m_vLock;
};
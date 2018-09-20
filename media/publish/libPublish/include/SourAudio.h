#pragma once
#include <string>
#include "voice_engine_export.h"
#include "bufferqueue.h"
#include "SourData.h"
#include "loadffmpeg.h"

using namespace std;
using namespace voice_engine;

class CSourAudio:ISourData
{
public:
	CSourAudio(string sname);
	virtual ~CSourAudio();
public:
	virtual bool GetBuffer(unsigned char** pBuf,unsigned int& nBufSize);

	virtual bool SetSourType(int nSourType);

	virtual bool SourOpen(void* param) ;

	virtual bool SourClose(bool bIsColseDev);

	virtual bool AVE_SetID(int nMainValue, int nSubValue, int nParams[2]);

	bool setMicMute(bool vol);

	bool setMicVolume(long vol);

	inline bool getMicMute() {return m_bMicMute;}
	long getMicVolume();
	bool GetBufferEx(unsigned char** pBuf,unsigned int& nBufSize);
	void setSaveFalg(bool bIsFlag);
	bool GetPreviewBufferEx(unsigned char** pBuf,unsigned int& nBufSize,unsigned int& nAudioSmpRate,unsigned int& nChannel);
	void setPreviewFalg(bool bIsFlag);
	bool SourPreviewOpen(int nSelectID) ;
public:
	long   GetDevMicCount();
	bool   GetDevMicName(long nIndex,char szName[256]);

	long GetDevSpeakerCount();
	bool   GetDevSpeakerName(long nIndex,char szName[256]);
	bool  SetDevOutPut(int index);

	inline bool   GetActiveing() {return m_bIsEffective;}
public:
	void RecvAudio(VData* pvdata);
private:
	bool    m_bIsEffective;
	IVoiceEngine*  m_pVoiceEngine;
	BufferQueue*   m_pAudioBufQue;
	BufferQueue*   m_pSaveAudioBufQue;
	BufferQueue*   m_pPreviewAudioBufQue;
	bool           m_bIsPreview;
	bool           m_bIsStart;
	char *         m_data;
	char *         m_resample;
	StreamParam    m_sParam;
	FILE *         fp_oa;
	bool           m_bMicMute;
	long             m_nMicVol;
	bool             m_saveFlag;
	int              m_nAudioSampleRate;
	ReSampleContext* m_resampler;
};
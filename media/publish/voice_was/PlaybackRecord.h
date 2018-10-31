#ifndef _PLAYBACK_RECORD_H_
#define _PLAYBACK_RECORD_H_

#include "AudioImplBase.h"

class CAudioRecord;
class CPlaybackRecord: public CAudioImplBase
{
public:
	CPlaybackRecord(IMMDevice* pDevice);
	virtual ~CPlaybackRecord();

	virtual void setKarok(bool useKarok,int micIndex);
	virtual void setParam(int channel,int sampleBit,int sampleRate);

public:
	void putKarokRecordData(BYTE* pbData,int nSize,ULONGLONG timeStamp);

	virtual bool open();
	virtual void close();
protected:
	virtual UINT doAuidoImpl();

	static UINT _stdcall mixAuidoThreadProc(void* lParam);
	virtual UINT doMixAudio();
protected:
	void freeRecordPacketList();
	void freePlaybackPacketList();

	void initRecordResample();
	void unitRecordResample();
	bool isRecordNeedResample();
	bool resampleRecordAudioData(BYTE* pInBuf,int nInSize,BYTE** ppOutBuf,int& nOutSize);

	void putRecordDataToList(BYTE* pbData,int nSize);

	bool getKarokRecordData(ULONGLONG timeStamp,BYTE* pbData, int nSize);
	bool getPlaybackData(BYTE** ppData, int& nSize,ULONGLONG& timeStamp);

	void mixerAudioData(BYTE* srcDataA,int nSizeA,BYTE* srcDataB,int nSizeB,BYTE** destData,int& destSize);

	void putAudioDataToListEx(BYTE* pbData,int nSize);
protected:
	CAudioRecord*		m_audioRecord;

	SwrContext *		m_swrContextRecord;

	AudioPacketList		m_listRecordPacket;
	CRITICAL_SECTION	m_crRecPacketList;

	AudioPacketList		m_listPlaybackPacket;
	CRITICAL_SECTION	m_crPbkPacketList;

	HANDLE				m_mixAudioThread;
	HANDLE				m_eventStopMix;

	int	m_karokMic;
};

#endif
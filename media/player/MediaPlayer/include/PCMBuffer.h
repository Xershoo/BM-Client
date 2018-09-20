#ifndef PCMBUFFER_H
#define PCMBUFFER_H
#include "sc_thread.h"
#include "sc_CSLock.h"
#define PCMFRAMETIME 80
#define PCMFRAMESIZE 2560
#define SIMPLERATENUM 16000


class CPCMBuffer
{
public:
	CPCMBuffer();
	~CPCMBuffer();
public:
	void WritePCMData(unsigned char* data,unsigned int nLen,int	samplerateint,int channel);
	void ReadPCMData(unsigned char** data,unsigned int& nLen,unsigned int& nZreoData,unsigned int& nPtsSpan);
	void ClearBuffer();
	unsigned int GetBusyPCMDataSize();
	void ReSetConfig(unsigned int nJitterAvgTime,unsigned int nMaxJitterYime);
	inline bool GetCacheStatus() {return !m_bFirstCahce;}
	unsigned int GetCacheDataTime();
	inline int  GetSsamplerate() {return m_samplerate;}

	void setConfig(bool bIsFlag);

public:
	CMutexLock     m_mutexLock;
private:
	unsigned char* m_pWPCMBuffer;
	unsigned int   m_nWPCMBufferSize;
	unsigned char* m_pRPCMBuffer;
	unsigned int   m_nRPCMBufferSize;
	unsigned int   nWritePCMPos;
	unsigned int   nReadPCMPos;
	int            m_nCycleWAR;
	bool           m_bFirstCahce;
	unsigned int   m_nErrCount;
	unsigned int   m_nEacheNum;
	unsigned int   m_nMaxEacheNum;
	unsigned int   m_nTempMaxEacheNum;
private:
	int			   m_samplerate;
	int			   m_nchannel;
	int			   m_lastsamplerate;
	int			   m_lastnchannel;
	int            m_nDorpTime;
	int            m_nLastPcmFormatSize;
	bool           m_bIsFlag;
};

#endif
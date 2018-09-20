#pragma once

#include "faac.h"
#pragma comment(lib, "libfaac.lib")

class CAACEncoder
{
public:
	CAACEncoder();
	~CAACEncoder();
	bool aac_EncodeInit(int nChannel,int nBitRate,int nAudioSamplerate,ULONG &nAACInputSamplesSize);
	bool aac_EncodeUnInit();
	bool aac_copyHeaderParam(unsigned char* pBuf,int &nSize,bool bKaraoke);
	bool aac_Encode(BYTE** pDestBuf,unsigned int& nDestBufSize,BYTE* pSrcBuf,unsigned int pSrcBufSize);
	inline bool aac_GetStatus() {return m_bIsInit;}
private:
	faacEncHandle  m_hAACEncoder;
	ULONG m_nAACInputSamples;
	ULONG m_nAACMaxOutputBytes;
	BYTE* m_pbPCMBuffer;
	BYTE* m_pbAACBuffer;
	int   m_nChannel;
	int   m_nBitRate;
	int   nAudioSmpRateEn;
	bool  m_bIsInit;
	FILE * fp_aac;
};
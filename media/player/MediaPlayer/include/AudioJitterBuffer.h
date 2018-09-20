#ifndef JITTERBUFFER_H
#define JITTERBUFFER_H
#include "PlayHeader.h"
#include "sc_CSLock.h"
#include <list>
extern "C"
{
	#include "speex\speex_jitter.h"
}

using namespace std;

class CAudioJitterBuffer
{
public:
	CAudioJitterBuffer();
	~CAudioJitterBuffer();
public:
	void Push(char* pData,unsigned int nSize,unsigned int nPts);
	int  Pop(char** pData,unsigned int& nSize,unsigned int& nPts,bool bIsWait = true);
	void unInit();
	void Init();
	inline  unsigned int GetAvgRecvTime() {return m_nTwoSecondAvgRecvTime;}
	inline  unsigned int GetMaxTime() {return m_nMaxRecvDiffTime;}
	void reset();
	inline bool GetStatus() {return m_bIsInit;}
private:
	void GetMemNode(MEMNode& mnode,int len);
	void PutMemNode(MEMNode& mnode);
private:
	int             m_framSize;
	unsigned int    m_nSequence;
	int             m_nMaxFrameSize;
	int             m_nSamplerate;
	int             m_nChannel;
	int             m_nPlayFrameSize;
	AACAudioFrame    m_lastAAFrame;
	AACAudioFrame    m_CurPlayAAFrame;
	DWORD            m_nSysSpan;
	JitterBuffer*	 m_pJitter;
	DWORD            m_CahceTime;
	bool             m_bCahceTime;
	int              m_nErrCount;
	bool             m_bIsInit;
private:
	list<MEMNode>   m_freeMemList;
	CMutexLock      m_memLock;
	CMutexLock      m_jitterBufLock;
private:
	DWORD           m_lastRecvSysTime;
	unsigned int    n_nRecvCount;
	unsigned int    n_nDiffTime;
	unsigned int    m_nMaxRecvDiffTime;
	unsigned int    m_nTwoSecondAvgRecvTime;

};

#endif
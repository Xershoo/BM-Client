#ifndef VIDEOJITTERBUFFER_H
#define VIDEOJITTERBUFFER_H

#include "sc_CSLock.h"
#include "PlayHeader.h"
#include <list>
using namespace std;

class CVideoJitterBuffer
{
public:
	CVideoJitterBuffer();
	~CVideoJitterBuffer();
public:
	void push(unsigned char *prefix,unsigned int nPrefixLen,unsigned char* pData,unsigned int nDataLen, int64_t pts);
	void pop(H264VideoFrame &hvf,int& nMaxJitterTime,int& nBufferCacheTime,bool bIsFlag);
	void clear();
	void freeVideoFrameMem(H264VideoFrame &hvf);

private:
	void PutMemNode(MEMNode& mnode);
	void GetMemNode(MEMNode& mnode,int len);
	void ClearMemNode();
private:
	list<H264VideoFrame>  m_listH264Video;
	CMutexLock            m_H264VideoLock;

private:
	list<MEMNode>   m_freeMemList;
	CMutexLock      m_memLock;

private:
	H264VideoFrame        m_lastVideoFrame;
	unsigned int          m_nMaxFrameSize;
	int                   m_VideoH;
	int                   m_VideoW;

	int                   m_nFrameCount;
	int                   m_nMaxVideoJitterTime;
	int                   m_nMaxTempJitterTime;
	unsigned int          m_nBufferCacheTime;
	unsigned int          m_nLastSysRecvFrameTime;
};
#endif
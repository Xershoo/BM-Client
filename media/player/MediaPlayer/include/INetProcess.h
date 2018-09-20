#ifndef INETPROCESS_H
#define INETPROCESS_H

typedef enum moudle_commond_t
{
	OPEN=0,
	CLOSE=1,
	PAUSE=2,
	STOP=3,
	SEEK=4,
	DURATION=5
}moudle_commond_t;

class CNetProcess
{
public:
	CNetProcess() {};
	virtual ~CNetProcess(){};

public:
	virtual void Activate(const char* szUrl,long nUserID) = 0;
	virtual bool SetCommond(moudle_commond_t& cmd,void* Param) = 0;
	virtual int ReadMedia(unsigned char* pBuf,unsigned nBufSize) = 0;
};

#endif
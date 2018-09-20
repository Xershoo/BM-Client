#ifndef SC_COND_H
#define SC_COND_H

class CSCCond
{
public:
	CSCCond();
	~CSCCond();
public:
	void cond_wait();
	void cond_signal();
	void conde_setwaittime(int waitTime);
private:
	void* m_cond;
	int   m_nRef;
	int   m_waittime;

};
#endif
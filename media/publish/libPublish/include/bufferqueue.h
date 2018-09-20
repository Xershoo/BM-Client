#pragma once
#include "lock.h"

class BufferQueueBase
{
public:
	virtual unsigned int size() = 0;
	virtual void clear() = 0;
	virtual bool push(const char* pData,unsigned int nSize) = 0;
	virtual bool pop(char* pData,unsigned int &nSize) = 0;
};

class BufferQueue : public BufferQueueBase
{
public:
	BufferQueue()
	{
	}

	virtual ~BufferQueue()
	{
		m_buf.clear();
	}


	virtual unsigned int size() 
	{   
		CLock::CAutoLock lock(m_lock);
		return m_buf.size();
	}

	virtual void clear() 
	{   
		CLock::CAutoLock lock(m_lock);
		m_buf.clear();
	}


	virtual bool push(const char* pData,unsigned int nSize)
	{
		CLock::CAutoLock lock(m_lock);
		if (!pData || nSize < 1)
			return false;

		m_buf.append(pData,nSize);

		return true;
	}

	virtual bool pop(char* pData,unsigned int &nSize)
	{
		CLock::CAutoLock lock(m_lock);

		if( m_buf.size() == 0 || !pData || nSize == 0)
			return false;

		if(m_buf.size() <= nSize)
		{
			nSize = m_buf.size();
			memcpy(pData,(char*)m_buf.data(),nSize);
			m_buf.clear();
		}
		else
		{
			std::string strTmp;
			strTmp.assign(m_buf,0,nSize);
			memcpy(pData,(char*)strTmp.data(),nSize);
			strTmp.clear();

			m_buf.erase(0,nSize);
		}
		
		return true;
	}
private:
	std::string  m_buf;
	CLock	m_lock;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 无锁缓冲队列.
class BufferQueueLockfree : public BufferQueueBase
{
public:

    #define   DEFAULT_BUFFER_SIZE 4*1024*1024 
	BufferQueueLockfree(int buf_size = DEFAULT_BUFFER_SIZE)   //注意：buf_size必须是2的N次方！！！ 
		: m_buffer_size(buf_size)
		, m_circle_buffer(NULL)
		, m_write_p(0)
		, m_read_p(0)
	{
		m_circle_buffer =  (char*)VirtualAlloc(NULL,m_buffer_size,MEM_COMMIT,PAGE_READWRITE);
	}

	~BufferQueueLockfree()
	{
		if(m_circle_buffer)
		{
			VirtualFree(m_circle_buffer,0,MEM_RELEASE);
			m_circle_buffer = NULL;
		}
	}

	virtual void clear()
	{
		m_write_p = 0;
		m_read_p = 0;
	}

	virtual unsigned int size()
	{
		return m_write_p - m_read_p;
	}

	virtual bool push(const char* pData, unsigned int nSize)
	{
		unsigned int l = 0;
		nSize = min(nSize, m_buffer_size - m_write_p + m_read_p);
		/* first put the data starting from fifo->in to buffer end */
		l = min(nSize, m_buffer_size - (m_write_p & (m_buffer_size - 1)));
		memcpy(m_circle_buffer + (m_write_p & (m_buffer_size - 1)), pData, l);
		/* then put the rest (if any) at the beginning of the buffer */
		memcpy(m_circle_buffer, pData + l, nSize - l);
		m_write_p += nSize;
		return true;
	}

	virtual bool pop(char* pData, unsigned int &nSize)
	{
		unsigned int l = 0; 
		nSize = min(nSize, m_write_p - m_read_p); 
		/* first get the data from fifo->out until the end of the buffer */ 
		l = min(nSize, m_buffer_size - (m_read_p & (m_buffer_size - 1))); 
		memcpy(pData, m_circle_buffer + (m_read_p & (m_buffer_size - 1)), l); 
		/* then get the rest (if any) from the beginning of the buffer */ 
		memcpy(pData + l, m_circle_buffer, nSize - l); 
		m_read_p += nSize; 
		return true; 
	}

private:
	int m_buffer_size;
	char* m_circle_buffer;
	unsigned int m_write_p;
	unsigned int m_read_p;
};

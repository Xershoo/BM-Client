#ifndef SCTHREAD_H
#define SCTHREAD_H

#ifndef CALLBACK
#define CALLBACK
#endif

/** @class CSCThread 
*  @brief 线程控制类.
*
* 提供线程控制类的安全使用
*/

typedef void* PVOID;
typedef void* HANDLE_THREAD;
typedef void (CALLBACK *ThreadProc)(PVOID pUserData);

class CSCThread
{
public:
	CSCThread(void);
	~CSCThread(void);
	
	/** 启动线程
	@param time_proc 线程的回调函数
	@param pUserData 回调函数的用户参数
	@return 创建线程是否成功
	@see End()
	@par	示例:
	@code

	void __stdcall Thread1Proc(VOID*pUserData)
	{
	}
	CVtThread thread1;
	thread1.Begin(Thread1Proc,NULL);

	@endcode
	*/
	bool Begin(ThreadProc thread_proc, PVOID pUserData);

	/** 结束线程。在某些应用中，比如关闭一个阻塞的网络线程，等待是无意义的，且会阻塞控制线程，所以此时可以直接调用End(0)杀掉线程。 	
	@param nWaitSec 等待的时间
	@return 是否成功
	@see Begin()
	*/	
	bool End(long nWaitSec = 5);

	/** 得到线程是否停止	
	@return 线程是否停止
	*/
	bool GetStop();

	void ThreadProcProxy();	
private:
	/** @brief 线程回调函数 */
	ThreadProc	    m_Callback;
	HANDLE_THREAD   m_hThread;
	PVOID           m_pUserData;
    bool            m_bStopThread;
};

#endif


#ifndef SCSYNC_H
#define SCSYNC_H
/** @class CSCLock CSCAutoLock
*  @brief 临界区类.
*
* 提供多线程访问同一块资源时锁的机制
*/

class CMutexLock
{
public:
	CMutexLock(void);
	~CMutexLock(void);

	/** 
	*@brief 加锁
	*/
	void Lock();
	/** 
	*@brief 释放锁
	*/
	void Unlock();
private:
	/** @brief 锁这里用指针通用各自的平台上的类去申请对象 */
	void *m_pLock;

};

class CAutoLock
{
public:
    CAutoLock(CMutexLock& lock);
    ~CAutoLock();

private:
    CAutoLock();
    CMutexLock* m_pLock;
};

#endif

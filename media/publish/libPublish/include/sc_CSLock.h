#ifndef SCSYNC_H
#define SCSYNC_H
/** @class CSCLock CSCAutoLock
*  @brief �ٽ�����.
*
* �ṩ���̷߳���ͬһ����Դʱ���Ļ���
*/

class CMutexLock
{
public:
	CMutexLock(void);
	~CMutexLock(void);

	/** 
	*@brief ����
	*/
	void Lock();
	/** 
	*@brief �ͷ���
	*/
	void Unlock();
private:
	/** @brief ��������ָ��ͨ�ø��Ե�ƽ̨�ϵ���ȥ������� */
	void *m_pLock;

};


#endif

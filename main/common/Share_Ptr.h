#ifndef APP_COMMON_SHARE_PAT_H
#define APP_COMMON_SHARE_PAT_H

namespace EDUAPP
{
	template<class T>
	class SharePtr
	{
	public:
		SharePtr()
		{
			_nSize = 0;
			_pBuffer = NULL;
		}
		SharePtr(int nSize)
		{
			if(nSize <= 0)
			{
				_pBuffer = NULL;
				_nSize = 0;
			}
			else
			{
				_pBuffer = new T[nSize];
				_nSize = nSize;
			}
		}
		~SharePtr()
		{
			if(_pBuffer)
				delete []_pBuffer;
		}
		int size()
		{
			return _nSize;
		}
		void SetSize(int nSize)
		{
			if(nSize <= 0)
				return;
			if(_pBuffer)
				delete []_pBuffer;
			_pBuffer = new T[nSize];
			_nSize = nSize;
		}
		T*   GetDataByIndex(int nIndex)
		{
			if(nIndex >= _nSize || nIndex < 0)
				return NULL;
			else
				return _pBuffer + nIndex;
		}
	private:
		SharePtr(SharePtr&);
		bool operator =(SharePtr&);

	private:
		T*	_pBuffer;
		int _nSize;
	};
}

#endif /*APP_COMMON_SHARE_PAT_H*/

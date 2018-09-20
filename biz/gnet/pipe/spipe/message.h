#pragma


namespace gnet
{
	// 4 byte size + data
	struct Message
	{
	public:
		Message(const char* msg, int len)
		{
			_msg = new char[len+4];
			*((int*)_msg) = len;
			memcpy(_msg+4, msg, len);
		}

		int Size(){ return *((int*)_msg) + 4; }
		const char* Head(){ return _msg; }


		static const char* GetNext(const char* begin, const char* end)
		{
			if (end - begin < 4)
				return NULL;
			int len = *((int*)begin);
			const char* ret = begin + 4 + len;
			if (ret <= end)
				return ret;
			return NULL;
		}

		~Message(){ delete[] _msg; }


	private:
		char*	_msg;
	};
}
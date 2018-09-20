#include "octets.h"


namespace gnet
{
	void* Rep::clone()
	{
		Rep* rep = create(cap);
		memcpy(rep->data(), data(), rep->len = len);
		return rep->data();
	}


	void* Rep::unique()
	{
		if(ref > 1)
		{
			void* ret = clone();
			release();
			return ret;
		}
		return data();
	}


	void* Rep::reserve(size_t size)
	{
		size = frob_size(size);
		if (size > cap)
		{
			Rep* rep = create(size);
			memcpy(rep->data(), data(), rep->len = len);
			release();
			return rep->data();
		}
		return unique();
	}


	size_t Rep::frob_size(size_t size)
	{
		size_t tmp = 16;
		while(size > tmp)	tmp <<= 1;
		return tmp;
	}


	Rep* Rep::create(size_t cap)
	{
		Rep* rep = new (cap) Rep;
		rep->cap = cap;
		rep->len = 0;
		rep->ref = 1;
		return rep;
	}


	Rep Rep::null = { 0, 0, 1 };


	//////////////////////////////////////////////////////////////////////////
	Octets& Octets::replace(const void* data, size_t size)
	{
		reserve(size);
		memcpy(base, data, size);
		rep()->len = size;
		return *this;
	}


	Octets::Octets(const void*x, const void* y)
		: base(Rep::create((char*)x - (char*)y)->data())
	{
		size_t size = (char*)x - (char*)y;
		memcpy(base, x, size);
		rep()->len = size;
	}


	Octets& Octets::operator = (const Octets& xx)
	{
		if (&xx != this)
		{
			rep()->release();
			base = xx.base;
			rep()->addref();
		}
		return *this;
	}


	Octets& Octets::erase(void* x, void* y)
	{
		if (x != y)
		{
			void* tmp = base;
			base = rep()->unique();
			ptrdiff_t o = (char*)base - (char*)tmp;
			if (o)
			{
				x = (char*)x + o;
				y = (char*)y + o;
			}
			fast_memmove((char*)x, (char*)y, (char*)base + rep()->len - (char*)y);
			rep()->len -= (char*)y - (char*)x;
		}
		return *this;
	}


	Octets& Octets::insert(void* pos, const void* x, size_t len)
	{
		ptrdiff_t off = (char*)pos - (char*)base;
		reserve(size() + len);
		pos = (char*)base + off;
		size_t adjust = size() - off;
		if (adjust)
			fast_memmove((char*)pos+len, pos, adjust);
		fast_memmove(pos, x, len);
		rep()->len += len;
		return *this;
	}
}
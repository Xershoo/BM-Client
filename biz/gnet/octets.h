#pragma once

#include "gnet/platform.h"
#include <memory>


namespace gnet
{
	inline void* fast_memmove(void* dest, const void* src, size_t n)
	{
		unsigned char* d = (unsigned char*)dest;
		unsigned char* s = (unsigned char*)src;
		if (s < d && s+n >= d)
			return memmove(dest, src, n);
		else
			return memcpy(dest, src, n);
	}


	struct Rep
	{
		size_t cap;
		size_t len;
		size_t ref;

		void addref(){ InterlockedIncrement((LONG*)&ref); }
		void release(){ if(0 == InterlockedDecrement((LONG*)&ref)) delete this; }
		void* data(){ return reinterpret_cast<void*>(this+1); }
		void* clone();
		void* unique();					// ref = 1
		void* reserve(size_t size);		// ref = 1


		//static
		static size_t frob_size(size_t size);
		static Rep* create(size_t cap);
		static void* operator new (size_t size, size_t extra){ return malloc(size+extra); }
		static void operator delete(void* p){ free(p); }
		static Rep null;
	};


	class Octets
	{
	public:
		virtual ~Octets(){ rep()->release(); }
		Octets() : base(Rep::null.data()){ rep()->addref(); }
		Octets(size_t size) : base(Rep::create(size)->data()){ rep()->len = size; }
		Octets(const void* x, size_t size) : base(Rep::create(size)->data()){ memcpy(base, x, size); rep()->len = size; }
		Octets(const void*x, const void* y);
		Octets(const Octets& xx) : base(xx.base) { rep()->addref(); }
		Octets& operator = (const Octets& xx);
		bool operator == (const Octets& xx) const { return size() == xx.size() && !memcmp(base, xx.base, size()); }
		bool operator != (const Octets& xx) const { return !operator == (xx); }
		Octets& swap(Octets& xx) { void* tmp = base; base = xx.base; xx.base = tmp; return *this; }
		void* begin() { unique(); return base; }
		void* end() { unique(); return (char*)base + rep()->len; }
		const void* begin() const { return base; }
		const void* end() const { return (char*)base + rep()->len; }
		size_t size() const { return rep()->len; }
		size_t capacity() const { return rep()->cap; }
		Octets& clear(){ unique(); rep()->len = 0; return *this; }
		Octets& erase(size_t pos, size_t len){ char* x = (char*)begin(); return erase(x+pos, x+pos+len); }
		Octets& erase(void* x, void* y);
		Octets& insert(void* pos, const void* x, size_t len);
		Octets& insert(void* pos, const void* x, const void* y){ return insert(pos, x, (char*)y-(char*)x); }
		Octets& resize(size_t size){ reserve(size); rep()->len = size; return *this; }
		Octets& replace(const void* data, size_t size);
		Octets& reserve(size_t size){ base = rep()->reserve(size); return *this; }


	private:
		Rep* rep() const { return reinterpret_cast<Rep*>(base) - 1; }
		void unique(){ base = rep()->unique(); }


	private:
		void* base;
	};
}
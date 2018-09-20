#pragma once

#include "octets.h"
#include "byteorder_i386.h"
#include "aliasing.h"
#include <string>
#include "boo.h"
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>


namespace gnet
{
	template<typename T>
	inline T& remove_const(const T& t){ return const_cast<T&>(t); }

	class OctetsStream;

	class Marshal
	{
	public:
		class Exception {};
		virtual OctetsStream& marshal(OctetsStream&) const = 0;
		virtual const OctetsStream& unmarshal(const OctetsStream&) = 0;
		virtual ~Marshal(){}
	};


	class OctetsStream
	{
	public:
		OctetsStream() : pos(0){}
		OctetsStream(const Octets& o) : data(o), pos(0){}
		OctetsStream(const OctetsStream& os) : data(os.data), pos(0){}

		OctetsStream& operator = (const OctetsStream& os)
		{
			if (&os != this)
			{
				pos = os.pos;
				data = os.data;
			}
			return *this;
		}

		bool operator == (const OctetsStream& os) const { return data == os.data; }
		bool operator != (const OctetsStream& os) const { return data != os.data; }
		size_t size() const { return data.size(); }
		void swap(OctetsStream& os) { data.swap(os.data); }
		operator Octets& () { return data; }
		size_t position() const { return pos; }

		void* begin() { return data.begin(); }
		void* end() { return data.end(); }
		const void* begin() const { return data.begin(); }
		const void* end() const { return data.end(); }
		void reserve(size_t size) { data.reserve(size); }
		void insert(void* pos, const void* x, size_t len){ data.insert(pos, x, len); }
		void insert(void* pos, const void* x, void* y){ data.insert(pos, x, y); }
		void erase(void* x, void* y){ data.erase(x, y); }
		void clear() { data.clear(); pos = 0; }
		bool eos() const { return pos == data.size(); }


	public:
		//output
		OctetsStream& operator << (char x) { return push_byte(x); }
		OctetsStream& operator << (unsigned char x) { return push_byte(x); }
		OctetsStream& operator << (bool x) { return push_byte(x); }
		OctetsStream& operator << (short x) { return push_byte(byteorder_16(x)); }
		OctetsStream& operator << (unsigned short x) { return push_byte(byteorder_16(x)); }
		OctetsStream& operator << (int x) { return push_byte(byteorder_32(x)); }
		OctetsStream& operator << (unsigned int x) { return push_byte(byteorder_32(x)); }
		OctetsStream& operator << (__int64 x) { return push_byte(byteorder_64(x)); }
		OctetsStream& operator << (unsigned __int64 x) { return push_byte(byteorder_64(x)); }
		OctetsStream& operator << (float x) { return push_byte(byteorder_32(aliasing_cast<int>(x))); }
		OctetsStream& operator << (double x) { return push_byte(byteorder_64(aliasing_cast<unsigned long long>(x))); }
		OctetsStream& operator << (const Marshal& x) { return x.marshal(*this); }
		OctetsStream& operator << (const Octets& x) 
		{
			compact_uint32(x.size());
			data.insert(data.end(), x.begin(), x.end());
			return *this;
		}
		template<typename T>
		OctetsStream& operator << (const std::basic_string<T>& x)
		{
			STATIC_ASSERT(sizeof(T) == 1);

			size_t bytes = x.length()*sizeof(T);
			compact_uint32(bytes);
			insert(end(), x.c_str(), bytes);
			return *this;
		}
		OctetsStream& push_byte(const char* x, size_t len)
		{
			insert(end(), x, len);
			return *this;
		}
		template<typename T1, typename T2>
		OctetsStream& operator << (const std::pair<T1, T2>& x)
		{
			return *this << x.first << x.second;
		}
		template<typename T>
		OctetsStream& operator << (const std::vector<T>& x)
		{
			return *this << MarshalContainer(remove_const(x));
		}
		template<typename T>
		OctetsStream& operator << (const std::set<T>& x)
		{
			return *this << MarshalContainer(remove_const(x));
		}
		template<typename T>
		OctetsStream& operator << (const std::list<T>& x)
		{
			return *this << MarshalContainer(remove_const(x));
		}
		template<typename T>
		OctetsStream& operator << (const std::deque<T>& x)
		{
			return *this << MarshalContainer(remove_const(x));
		}
		template<typename T1, typename T2>
		OctetsStream& operator << (const std::map<T1, T2>& x)
		{
			return *this << MarshalContainer(remove_const(x));
		}


		//input
		const OctetsStream& operator >> (char& x) const { x = pop_byte8(); return *this; }
		const OctetsStream& operator >> (unsigned char& x) const { x = pop_byte8(); return *this; }
		const OctetsStream& operator >> (bool& x) const { x = !!pop_byte8(); return *this; }
		const OctetsStream& operator >> (short& x) const { x = pop_byte16(); return *this; }
		const OctetsStream& operator >> (unsigned short& x) const { x = pop_byte16(); return *this; }
		const OctetsStream& operator >> (int& x) const { x = pop_byte32(); return *this; }
		const OctetsStream& operator >> (unsigned int& x) const { x = pop_byte32(); return *this; }
		const OctetsStream& operator >> (__int64& x) const { x = pop_byte64(); return *this; }
		const OctetsStream& operator >> (unsigned __int64& x) const { x = pop_byte64(); return *this; }
		const OctetsStream& operator >> (float& x) const { unsigned long l = pop_byte32(); x = aliasing_cast<float>(l); return *this; }
		const OctetsStream& operator >> (double& x) const { unsigned long long ll = pop_byte64(); x = aliasing_cast<double>(ll); return *this; }
		const OctetsStream& operator >> (Marshal& x) const { return x.unmarshal(*this); }
		const OctetsStream& operator >> (Octets& x) const
		{
			size_t len;
			uncompact_uint32(len);
			if (len > data.size() - pos) 
			{
				throw Marshal::Exception();
			}

			x.replace((char*)data.begin()+pos, len);
			pos += len;
			return *this;
		}
		template<typename T>
		const OctetsStream& operator >> (std::basic_string<T>& x) const
		{
			STATIC_ASSERT(sizeof(T) == 1);

			size_t bytes;
			uncompact_uint32(bytes);
			if (bytes % sizeof(T)) 
				throw Marshal::Exception();
			if (bytes > data.size() - pos) 
				throw Marshal::Exception();
			x.assign((T*)((char*)data.begin()+pos), bytes/sizeof(T));
			pos += bytes;
			return *this;
		}
		void pop_byte(char* x, size_t len) const
		{
			if (pos + len > data.size())
			{
				throw Marshal::Exception();
			}
			memcpy(x, (char*)data.begin()+pos, len);
			pos += len;
		}
		template<typename T1, typename T2>
		const OctetsStream& operator >> (std::pair<T1, T2>& x) const
		{
			return *this >> remove_const(x.first) >> x.second;
		}
		template<typename T>
		const OctetsStream& operator >> (std::vector<T>& x) const
		{
			return *this >> MarshalContainer(x);
		}
		template<typename T>
		const OctetsStream& operator >> (std::set<T>& x) const
		{
			return *this >> MarshalContainer(x);
		}
		template<typename T>
		const OctetsStream& operator >> (std::list<T>& x) const
		{
			return *this >> MarshalContainer(x);
		}
		template<typename T>
		const OctetsStream& operator >> (std::deque<T>& x) const
		{
			return *this >> MarshalContainer(x);
		}
		template<typename T1, typename T2>
		const OctetsStream& operator >> (std::map<T1, T2>& x) const
		{
			return *this >> MarshalContainer(x);
		}


		//compact
		OctetsStream& compact_uint32(unsigned int x)
		{
			if (x < 0x80)	return push_byte((unsigned char)x);
			else if (x < 0x4000) return push_byte((byteorder_16(x|0x8000)));
			else if (x < 0x20000000) return push_byte(byteorder_32((x|0xc0000000)));
			push_byte((unsigned char)0xe0);
			return push_byte(byteorder_32(x));
		}

		const OctetsStream& uncompact_uint32(unsigned int& x) const
		{
			if (pos == data.size())	
				throw Marshal::Exception();

			switch(*((unsigned char*)data.begin()+pos) & 0xe0)
			{
			case 0xe0:
				pop_byte8();
				x = pop_byte32();
				return *this;
			case 0xc0:
				x = pop_byte32() & ~0xc0000000;
				return * this;
			case 0xa0:
			case 0x80:
				x = pop_byte16() & ~0x8000;
				return *this;
			}
			x = pop_byte8();
			return *this;
		}

		OctetsStream& compact_sint32(int x)
		{
			if (x > 0)
			{
				if (x < 0x40) return push_byte((unsigned char)x);
				else if (x < 0x2000) return push_byte(byteorder_16(x|0x8000));
				else if (x < 0x10000000) return push_byte(byteorder_32(x|0xc0000000));
				push_byte((unsigned char)0xe0);
				return push_byte(byteorder_32(x));
			}
			if (-x > 0)
			{
				x = -x;
				if (x < 0x40) return push_byte((unsigned char)x|0x40);
				else if (x < 0x2000) return push_byte(byteorder_16(x|0xa000));
				else if (x < 0x10000000) return push_byte(byteorder_32(x|0xd0000000));
				push_byte((unsigned char)0xf0);
				return push_byte(byteorder_32(x));
			}
			push_byte((unsigned char)0xf0);
			return push_byte(byteorder_32(x));
		}

		const OctetsStream& uncompact_sint32(int& x) const
		{
			if (pos == data.size())	
			{
				throw Marshal::Exception();
			}
			switch(*((unsigned char*)data.begin()+pos) & 0xf0)
			{
			case 0xf0:
				pop_byte8();
				x = 0 - pop_byte32();
				return *this;
			case 0xe0:
				pop_byte8();
				x = pop_byte32();
				return *this;
			case 0xd0:
				x = 0 - (pop_byte32() & ~0xd0000000);
				return *this;
			case 0xc0:
				x = pop_byte32() & ~0xc0000000;
				return * this;
			case 0xb0:
			case 0xa0:
				x = 0 - (pop_byte16() & ~0xa000);
				return *this;
			case 0x90:
			case 0x80:
				x = pop_byte16() & ~0x8000;
				return *this;
			case 0x70:
			case 0x60:
			case 0x50:
			case 0x40:
				x = 0 - (pop_byte8() & ~0x40);
				return *this;
			}
			x = pop_byte8();
			return *this;
		}


	private:
		template<typename T> 
		OctetsStream& push_byte(T t)
		{
			data.insert(data.end(), &t, sizeof(t));
			return *this;
		}

		template<typename T> 
		void pop_byte(T& t) const
		{
			if (pos + sizeof(t) > data.size())
			{
				throw Marshal::Exception();
			}
			t = *(T*)((char*)data.begin() + pos);
			pos += sizeof(t);
		}

		unsigned char pop_byte8() const
		{
			unsigned char c;
			pop_byte(c);
			return c;
		}

		unsigned short pop_byte16() const
		{
			unsigned short s;
			pop_byte(s);
			return byteorder_16(s);
		}

		unsigned long pop_byte32() const
		{
			unsigned long l;
			pop_byte(l);
			return byteorder_32(l);
		}

		unsigned long long pop_byte64() const
		{
			unsigned long long ll;
			pop_byte(ll);
			return byteorder_64(ll);
		}


	private:
	public:
		Octets data;
		mutable size_t pos;
	};


	class CompactUINT : public Marshal
	{
	public:
		explicit CompactUINT(unsigned int& i) : pi(&i){}
		virtual OctetsStream& marshal(OctetsStream& os) const
		{
			return os.compact_uint32(*pi);
		}
		virtual const OctetsStream& unmarshal(const OctetsStream& os)
		{
			return os.uncompact_uint32(*pi);
		}


	private:
		unsigned int* pi;
	};


	class CompactSINT : public Marshal
	{
	public:
		explicit CompactSINT(int& i) : pi(&i){}
		virtual OctetsStream& marshal(OctetsStream& os) const
		{
			return os.compact_sint32(*pi);
		}
		virtual const OctetsStream& unmarshal(const OctetsStream& os)
		{
			return os.uncompact_sint32(*pi);
		}


	private:
		int* pi;
	};


	template<typename Container>
	class STLContainer : public Marshal
	{
	public:
		explicit STLContainer(Container& c) : pc(&c){}
		virtual OctetsStream& marshal(OctetsStream& os) const
		{
			size_t size = pc->size();
			os << CompactUINT(size);
			for (typename Container::const_iterator it = pc->begin(), e = pc->end(); it != e; it++)
				os << *it;
			return os;
		}
		const OctetsStream& unmarshal(const OctetsStream& os)
		{
			size_t size;
			for (os >> CompactUINT(size); size > 0; --size)
			{
				typename Container::value_type tmp;
				os >> tmp;
				pc->insert(pc->end(), tmp);
			}
			return os;
		}


	private:
		Container* pc;
	};


	template<typename Container>
	inline STLContainer<Container> MarshalContainer(const Container& c)
	{
		//强制转化传入类型为非const，否则模板中的unmarshal不能实例化
		return STLContainer<Container>(remove_const(c));
	}
}
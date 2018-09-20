#pragma once

#define STAT_MIN5(ds,value)
#define STAT_HOUR(ds,value)
#define STAT_DAY(ds,value)

#include <map>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

#include "gnet/octets.h"
#include "gnet/socket/perf.h"
#include "gnet/socket/streamcompress.h"


namespace gnet
{
	enum { RANDOM = 0, NULLSECURITY = 1, ARCFOURSECURITY = 2, MD5HASH = 3, HMAC_MD5HASH = 4, 
		COMPRESSARCFOURSECURITY = 5, DECOMPRESSARCFOURSECURITY = 6, SHA1HASH = 7 };

	class Security
	{
	public:
		typedef unsigned int Type;
	private:
		Type type;
		typedef std::map<Type, const Security *> Map;
		static Map& GetMap();
	protected:
		Security() { }
		virtual ~Security() { }
		Security(Type t) : type(t)
		{
			if (GetMap().find(type) == GetMap().end()) GetMap().insert(std::make_pair(type, this));
		}
	public:
		Security(const Security &rhs) : type(rhs.type) { }
		virtual void SetParameter(const Octets &) { }
		virtual void GetParameter(Octets &) { }
		virtual Octets& Update(Octets &) = 0;
		virtual Octets& Final(Octets &o) { return o; }
		virtual Security *Clone() const = 0;
		virtual void Destroy() { delete this; }
		static Security* Create(Type type)
		{
			Map::const_iterator it = GetMap().find(type);
			if (it == GetMap().end())
			{
				assert(false);
				return NULL; // it = GetMap().find(NULLSECURITY);
			}
			return (*it).second->Clone();
		}
	};

#ifndef WIN32
	class Random : public Security
	{
		static int fd;
		static int Init() { return open("/dev/urandom", O_RDONLY); }
		Security *Clone() const { return new Random(*this); }
	public:
		Random() { }
		Random(Type type) : Security(type) { }
		Random(const Random &rhs) : Security(rhs) { }
		Octets& Update(Octets &o) { read(fd, o.begin(), o.size()); return o; }
	};
#else
	class Random : public Security
	{
		static int fd;
		Security *Clone() const { return new Random(*this); }
		static int Init() { srand( (unsigned int)time( NULL)); return 0; }
	public:
		Random() { }
		Random(Type type) : Security(type) { }
		Random(const Random &rhs) : Security(rhs) { }
		Octets& Update(Octets &o) {
			size_t	nSize = o.size();
			for( size_t i = 0; i < nSize; i ++)
				((unsigned char*)o.begin())[i] = ( (unsigned char)rand() & 0xFF);
			return o;
		}
	};
#endif

	class NullSecurity : public Security
	{
		Security *Clone() const { return new NullSecurity(*this); }
	public:
		NullSecurity() { }
		NullSecurity(Type type) : Security(type) { }
		NullSecurity(const NullSecurity &rhs) : Security(rhs) { }
		Octets& Update(Octets &o) { return o; }
	};

#if defined __i386__
	class ARCFourSecurity : public Security
	{
		unsigned char context[264];
		Security *Clone() const { return new ARCFourSecurity(*this); }
	public:
		ARCFourSecurity() { }
		ARCFourSecurity(Type type) : Security(type) { }
		ARCFourSecurity(const ARCFourSecurity &rhs) : Security(rhs) { memcpy(context, rhs.context, sizeof(context)); }
		void SetParameter(const Octets &param) { rc4_init_context(context, param.begin(), param.size()); }
		Octets& Update(Octets &o) { rc4_update(context, o.begin(), o.size()); return o; }
	};

#else
	class ARCFourSecurity : public Security
	{
		unsigned char index1;
		unsigned char index2;
		unsigned char perm[256];
		Security *Clone() const { return new ARCFourSecurity(*this); }
	public:
		ARCFourSecurity() { }
		ARCFourSecurity(Type type) : Security(type) { }
		ARCFourSecurity(const ARCFourSecurity &rhs) : Security(rhs), index1(rhs.index1), index2(rhs.index2)
		{
			memcpy(perm, rhs.perm, sizeof(perm));
		}
		void SetParameter(const Octets &param)
		{
			unsigned char *IV = (unsigned char *)param.begin();
			size_t keylen = param.size();
			unsigned char j;
			size_t i;
			for (i = 0; i < 256; i++)
				perm[i] = (unsigned char) i;
			for (j = i = 0; i < 256; i++)
			{
				j += perm[i] + IV[i % keylen];
				std::swap(perm[i], perm[j]);
			}
			index1 = index2 = 0;
		}
		Octets& Update(Octets &o)
		{
			unsigned char *p = (unsigned char *)o.begin();
			unsigned char *q = (unsigned char *)o.end();
			while ( p != q )
			{
				unsigned char j1 = perm[++index1];
				unsigned char j2 = perm[index2 += j1];
				perm[index2] = j1;
				perm[index1] = j2;
				*p++ ^= perm[(unsigned char)(j1+j2)];
			}
			return o;
		}
	};
#endif

#if defined __i386__
	class MD5Hash : public Security
	{
		unsigned char context[88];
		Security *Clone() const { return new MD5Hash(*this); }
	public:
		MD5Hash() { md5_init_context(context); }
		MD5Hash(Type type) : Security(type) { md5_init_context( context ); }
		MD5Hash(const MD5Hash &rhs) : Security(rhs) { memcpy(context, rhs.context, sizeof(context)); }
		void Reset(){ md5_init_context(context); };
		Octets& Update(Octets &o) { md5_update(context, o.begin(), o.size()); return o; }
		Octets& Final(Octets &digest)
		{
			md5_final( context );
			return digest.replace( context, 16 );
		}

		static Octets Digest(const Octets &o)
		{
			unsigned char digest[16];
			md5_digest(digest, o.begin(), o.size());
			return Octets(digest, 16);
		}
	};
#else
	class MD5Hash : public Security
	{
		unsigned int state[4];
		unsigned int low_count, high_count, remain;
		unsigned char buffer[64];
		Security *Clone() const { return new MD5Hash(*this); }
		void transform (const unsigned char block[64])
		{
#define f(x, y, z) (z ^ (x & (y ^ z)))
#define g(x, y, z) (y ^ (z & (x ^ y)))
#define h(x, y, z) ((x) ^ (y) ^ (z))
#define i(x, y, z) ((y) ^ ((x) | (~z)))
#define rotate_left(x, n) (((x) << (n)) | ((x) >> (32-(n))))

			unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
			unsigned int *x = (unsigned int *)block;

			/* round 1 */
#define ff(a, b, c, d, x, s, ac) { \
	(a) += f ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = rotate_left ((a), (s)); \
	(a) += (b); \
			}
			ff (a, b, c, d, x[ 0],   7, 0xd76aa478); /* 1 */
			ff (d, a, b, c, x[ 1],  12, 0xe8c7b756); /* 2 */
			ff (c, d, a, b, x[ 2],  17, 0x242070db); /* 3 */
			ff (b, c, d, a, x[ 3],  22, 0xc1bdceee); /* 4 */
			ff (a, b, c, d, x[ 4],   7, 0xf57c0faf); /* 5 */
			ff (d, a, b, c, x[ 5],  12, 0x4787c62a); /* 6 */
			ff (c, d, a, b, x[ 6],  17, 0xa8304613); /* 7 */
			ff (b, c, d, a, x[ 7],  22, 0xfd469501); /* 8 */
			ff (a, b, c, d, x[ 8],   7, 0x698098d8); /* 9 */
			ff (d, a, b, c, x[ 9],  12, 0x8b44f7af); /* 10 */
			ff (c, d, a, b, x[10],  17, 0xffff5bb1); /* 11 */
			ff (b, c, d, a, x[11],  22, 0x895cd7be); /* 12 */
			ff (a, b, c, d, x[12],   7, 0x6b901122); /* 13 */
			ff (d, a, b, c, x[13],  12, 0xfd987193); /* 14 */
			ff (c, d, a, b, x[14],  17, 0xa679438e); /* 15 */
			ff (b, c, d, a, x[15],  22, 0x49b40821); /* 16 */
#undef ff

			/* round 2 */
#define gg(a, b, c, d, x, s, ac) { \
	(a) += g ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = rotate_left ((a), (s)); \
	(a) += (b); \
			}
			gg (a, b, c, d, x[ 1],   5, 0xf61e2562); /* 17 */
			gg (d, a, b, c, x[ 6],   9, 0xc040b340); /* 18 */
			gg (c, d, a, b, x[11],  14, 0x265e5a51); /* 19 */
			gg (b, c, d, a, x[ 0],  20, 0xe9b6c7aa); /* 20 */
			gg (a, b, c, d, x[ 5],   5, 0xd62f105d); /* 21 */
			gg (d, a, b, c, x[10],   9,  0x2441453); /* 22 */
			gg (c, d, a, b, x[15],  14, 0xd8a1e681); /* 23 */
			gg (b, c, d, a, x[ 4],  20, 0xe7d3fbc8); /* 24 */
			gg (a, b, c, d, x[ 9],   5, 0x21e1cde6); /* 25 */
			gg (d, a, b, c, x[14],   9, 0xc33707d6); /* 26 */
			gg (c, d, a, b, x[ 3],  14, 0xf4d50d87); /* 27 */
			gg (b, c, d, a, x[ 8],  20, 0x455a14ed); /* 28 */
			gg (a, b, c, d, x[13],   5, 0xa9e3e905); /* 29 */
			gg (d, a, b, c, x[ 2],   9, 0xfcefa3f8); /* 30 */
			gg (c, d, a, b, x[ 7],  14, 0x676f02d9); /* 31 */
			gg (b, c, d, a, x[12],  20, 0x8d2a4c8a); /* 32 */
#undef gg 

			/* round 3 */
#define hh(a, b, c, d, x, s, ac) { \
	(a) += h ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = rotate_left ((a), (s)); \
	(a) += (b); \
			}
			hh (a, b, c, d, x[ 5],   4, 0xfffa3942); /* 33 */
			hh (d, a, b, c, x[ 8],  11, 0x8771f681); /* 34 */
			hh (c, d, a, b, x[11],  16, 0x6d9d6122); /* 35 */
			hh (b, c, d, a, x[14],  23, 0xfde5380c); /* 36 */
			hh (a, b, c, d, x[ 1],   4, 0xa4beea44); /* 37 */
			hh (d, a, b, c, x[ 4],  11, 0x4bdecfa9); /* 38 */
			hh (c, d, a, b, x[ 7],  16, 0xf6bb4b60); /* 39 */
			hh (b, c, d, a, x[10],  23, 0xbebfbc70); /* 40 */
			hh (a, b, c, d, x[13],   4, 0x289b7ec6); /* 41 */
			hh (d, a, b, c, x[ 0],  11, 0xeaa127fa); /* 42 */
			hh (c, d, a, b, x[ 3],  16, 0xd4ef3085); /* 43 */
			hh (b, c, d, a, x[ 6],  23,  0x4881d05); /* 44 */
			hh (a, b, c, d, x[ 9],   4, 0xd9d4d039); /* 45 */
			hh (d, a, b, c, x[12],  11, 0xe6db99e5); /* 46 */
			hh (c, d, a, b, x[15],  16, 0x1fa27cf8); /* 47 */
			hh (b, c, d, a, x[ 2],  23, 0xc4ac5665); /* 48 */
#undef hh 

			/* round 4 */
#define ii(a, b, c, d, x, s, ac) { \
	(a) += i ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = rotate_left ((a), (s)); \
	(a) += (b); \
			}
			ii (a, b, c, d, x[ 0],   6, 0xf4292244); /* 49 */
			ii (d, a, b, c, x[ 7],  10, 0x432aff97); /* 50 */
			ii (c, d, a, b, x[14],  15, 0xab9423a7); /* 51 */
			ii (b, c, d, a, x[ 5],  21, 0xfc93a039); /* 52 */
			ii (a, b, c, d, x[12],   6, 0x655b59c3); /* 53 */
			ii (d, a, b, c, x[ 3],  10, 0x8f0ccc92); /* 54 */
			ii (c, d, a, b, x[10],  15, 0xffeff47d); /* 55 */
			ii (b, c, d, a, x[ 1],  21, 0x85845dd1); /* 56 */
			ii (a, b, c, d, x[ 8],   6, 0x6fa87e4f); /* 57 */
			ii (d, a, b, c, x[15],  10, 0xfe2ce6e0); /* 58 */
			ii (c, d, a, b, x[ 6],  15, 0xa3014314); /* 59 */
			ii (b, c, d, a, x[13],  21, 0x4e0811a1); /* 60 */
			ii (a, b, c, d, x[ 4],   6, 0xf7537e82); /* 61 */
			ii (d, a, b, c, x[11],  10, 0xbd3af235); /* 62 */
			ii (c, d, a, b, x[ 2],  15, 0x2ad7d2bb); /* 63 */
			ii (b, c, d, a, x[ 9],  21, 0xeb86d391); /* 64 */
#undef ii 
#undef rotate_left 
#undef i
#undef h
#undef g
#undef f
			state[0] += a;
			state[1] += b;
			state[2] += c;
			state[3] += d;
		}
		void update(const unsigned char *input, unsigned int inputlen)
		{
			if ( remain )
			{
				unsigned int copy_len = 64 - remain;
				if ( inputlen < copy_len )
				{
					memcpy( buffer + remain, input, inputlen );
					remain += inputlen;
					return;
				}
				memcpy(buffer + remain, input, copy_len);
				transform( buffer );
				inputlen -= copy_len;
				input    += copy_len;
				if ( (low_count += 512) == 0 ) high_count++;
			}
			for ( ;inputlen >= 64 ; inputlen -= 64, input += 64)
			{
				transform( input );
				if ( (low_count += 512) == 0 ) high_count++;
			}
			if ( (remain = inputlen) > 0 ) memcpy ( buffer, input, remain );
		}
		void init()
		{
			state[0] = 0x67452301;
			state[1] = 0xefcdab89;
			state[2] = 0x98badcfe;
			state[3] = 0x10325476;
			low_count = high_count = remain = 0;
		}
	public:
		MD5Hash() { init(); }
		MD5Hash(Type type) : Security(type) { init(); }
		MD5Hash(const MD5Hash &rhs) : Security(rhs), low_count(rhs.low_count), high_count(rhs.high_count), remain(rhs.remain)
		{
			memcpy(state, rhs.state, sizeof(state));
			memcpy(buffer,rhs.buffer,sizeof(buffer));
		}
		void Reset(){ init(); }
		Octets& Update(Octets &o)
		{
			update((const unsigned char *)o.begin(), o.size());
			return o;
		}
		Octets& Final(Octets &digest)
		{
			static unsigned char padding[64] = {
				0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			unsigned int l = low_count, h = high_count;
			if ( (l += remain * 8) < l ) h++;
			unsigned int index = remain  & 0x3f;
			unsigned int padlen = (index < 56) ? (56 - index) : (120 - index);
			update (padding, padlen);
			update ((const unsigned char *)&l, sizeof(l));
			update ((const unsigned char *)&h, sizeof(h));
			return digest.replace( state, sizeof(state) );
		}

		static Octets Digest(const Octets &o)
		{
			Octets digest;
			MD5Hash ctx;
			ctx.update((const unsigned char *)o.begin(), o.size());
			return ctx.Final(digest);
		}

		const unsigned char * GetState() const
		{
			return (const unsigned char *)state;
		}
	};
#endif

	class HMAC_MD5Hash : public Security
	{
		Octets k_opad;
		MD5Hash md5hash;
		Security *Clone() const { return new HMAC_MD5Hash(*this); }
	public:
		HMAC_MD5Hash() : k_opad(64) { k_opad.resize(64); }
		HMAC_MD5Hash(Type type) : Security(type), k_opad(64) { k_opad.resize(64); }
		HMAC_MD5Hash(const HMAC_MD5Hash &rhs) : Security(rhs), k_opad(rhs.k_opad),
			md5hash(rhs.md5hash) { }
		void SetParameter(const Octets &param)
		{
			Octets k_ipad(64); k_ipad.resize(64);
			unsigned char *ipad = (unsigned char *)k_ipad.begin();
			unsigned char *opad = (unsigned char *)k_opad.begin();
			size_t keylen = param.size();
			if (keylen > 64)
			{
				Octets key = MD5Hash::Digest(param);
				memcpy(ipad, key.begin(), key.size());
				memcpy(opad, key.begin(), key.size());
				keylen = key.size();
			}
			else
			{
				memcpy(ipad, param.begin(), keylen);
				memcpy(opad, param.begin(), keylen);
			}
			for (size_t i = 0; i < keylen; i++) { ipad[i] ^= 0x36; opad[i] ^= 0x5c; }
			memset(ipad + keylen, 0x36, 64 - keylen);
			memset(opad + keylen, 0x5c, 64 - keylen);
			md5hash.Update(k_ipad);
		}
		Octets& Update(Octets &o) { return md5hash.Update(o); }
		Octets& Final(Octets &digest)
		{
			md5hash.Final(digest);
			MD5Hash ctx;
			ctx.Update(k_opad);
			ctx.Update(digest);
			return ctx.Final(digest);
		}
	};

	class CompressARCFourSecurity : public Security
	{
		ARCFourSecurity arc4;
		class Compress compress;
		Security *Clone() const { return new CompressARCFourSecurity(*this); }
	public:
		CompressARCFourSecurity() { }
		CompressARCFourSecurity(Type type) : Security(type) { }
		void SetParameter(const Octets &param) { arc4.SetParameter(param); }
		Octets& Update(Octets &o)
		{
			STAT_MIN5("CompressSrcSize", o.size());
			arc4.Update(compress.Final(o));
			STAT_MIN5("CompressComSize", o.size());
			return o;
		}
	};

	class DecompressARCFourSecurity : public Security
	{
		ARCFourSecurity arc4;
		Decompress decompress;
		Security *Clone() const { return new DecompressARCFourSecurity(*this); }
	public:
		DecompressARCFourSecurity() { }
		DecompressARCFourSecurity(Type type) : Security(type) { }
		void SetParameter(const Octets &param) { arc4.SetParameter(param); }
		Octets& Update(Octets &o) { return decompress.Update(arc4.Update(o)); }
	};

#if defined __i386__
	class SHA1Hash : public Security
	{
		unsigned char context[92];
		Security *Clone() const { return new SHA1Hash(*this); }
	public:
		SHA1Hash() { sha1_init_context(context); }
		SHA1Hash(Type type) : Security(type) { sha1_init_context( context ); }
		SHA1Hash(const SHA1Hash &rhs) : Security(rhs) { memcpy(context, rhs.context, sizeof(context)); }
		Octets& Update(Octets &o) { sha1_update(context, o.begin(), o.size()); return o; }
		Octets& Final(Octets &digest)
		{
			sha1_final( context );
			return digest.replace( context, 20 );
		}
		static Octets Digest(const Octets &o)
		{
			unsigned char digest[20];
			sha1_digest(digest, o.begin(), o.size());
			return Octets(digest, 20);
		}
	};
#else
	class SHA1Hash : public Security
	{
		unsigned int state[5];
		unsigned int low_count, high_count, remain;
		unsigned char buffer[64];
		Security *Clone() const { return new SHA1Hash(*this); }
		static inline unsigned int rol1(unsigned int x)  { return (x << 1) | (x >> 31); }
		static inline unsigned int rol5(unsigned int x)  { return (x << 5) | (x >> 27); }
		static inline unsigned int rol30(unsigned int x) { return (x << 30) | (x >> 2); }
		void transform (const unsigned char block[64])
		{
			unsigned int x[16];
			unsigned int *p = (unsigned int *)block;
			unsigned int a = state[0];
			unsigned int b = state[1];
			unsigned int c = state[2];
			unsigned int d = state[3];
			unsigned int e = state[4];

#define K1  0x5A827999L
#define K2  0x6ED9EBA1L
#define K3  0x8F1BBCDCL
#define K4  0xCA62C1D6L
#define F1(x,y,z)   (z^(x&(y^z)))
#define F2(x,y,z)   (x^y^z)
#define F3(x,y,z)   ((x&y)|(z&(x|y)))
#define F4(x,y,z)   (x^y^z)
#define M(i) ( x[i&0x0f] = rol1(x[(i-14)&15]^x[(i-8)&15]^x[(i-3)&15]^x[i&15]) )
#define R(a,b,c,d,e,f,k,m)  e += rol5(a)+f(b,c,d)+k+m; b=rol30(b)

			R( a, b, c, d, e, F1, K1, (x[ 0] = byteorder_32(p[ 0])));
			R( e, a, b, c, d, F1, K1, (x[ 1] = byteorder_32(p[ 1])));
			R( d, e, a, b, c, F1, K1, (x[ 2] = byteorder_32(p[ 2])));
			R( c, d, e, a, b, F1, K1, (x[ 3] = byteorder_32(p[ 3])));
			R( b, c, d, e, a, F1, K1, (x[ 4] = byteorder_32(p[ 4])));
			R( a, b, c, d, e, F1, K1, (x[ 5] = byteorder_32(p[ 5])));
			R( e, a, b, c, d, F1, K1, (x[ 6] = byteorder_32(p[ 6])));
			R( d, e, a, b, c, F1, K1, (x[ 7] = byteorder_32(p[ 7])));
			R( c, d, e, a, b, F1, K1, (x[ 8] = byteorder_32(p[ 8])));
			R( b, c, d, e, a, F1, K1, (x[ 9] = byteorder_32(p[ 9])));
			R( a, b, c, d, e, F1, K1, (x[10] = byteorder_32(p[10])));
			R( e, a, b, c, d, F1, K1, (x[11] = byteorder_32(p[11])));
			R( d, e, a, b, c, F1, K1, (x[12] = byteorder_32(p[12])));
			R( c, d, e, a, b, F1, K1, (x[13] = byteorder_32(p[13])));
			R( b, c, d, e, a, F1, K1, (x[14] = byteorder_32(p[14])));
			R( a, b, c, d, e, F1, K1, (x[15] = byteorder_32(p[15])));
			R( e, a, b, c, d, F1, K1, M(16) );
			R( d, e, a, b, c, F1, K1, M(17) );
			R( c, d, e, a, b, F1, K1, M(18) );
			R( b, c, d, e, a, F1, K1, M(19) );
			R( a, b, c, d, e, F2, K2, M(20) );
			R( e, a, b, c, d, F2, K2, M(21) );
			R( d, e, a, b, c, F2, K2, M(22) );
			R( c, d, e, a, b, F2, K2, M(23) );
			R( b, c, d, e, a, F2, K2, M(24) );
			R( a, b, c, d, e, F2, K2, M(25) );
			R( e, a, b, c, d, F2, K2, M(26) );
			R( d, e, a, b, c, F2, K2, M(27) );
			R( c, d, e, a, b, F2, K2, M(28) );
			R( b, c, d, e, a, F2, K2, M(29) );
			R( a, b, c, d, e, F2, K2, M(30) );
			R( e, a, b, c, d, F2, K2, M(31) );
			R( d, e, a, b, c, F2, K2, M(32) );
			R( c, d, e, a, b, F2, K2, M(33) );
			R( b, c, d, e, a, F2, K2, M(34) );
			R( a, b, c, d, e, F2, K2, M(35) );
			R( e, a, b, c, d, F2, K2, M(36) );
			R( d, e, a, b, c, F2, K2, M(37) );
			R( c, d, e, a, b, F2, K2, M(38) );
			R( b, c, d, e, a, F2, K2, M(39) );
			R( a, b, c, d, e, F3, K3, M(40) );
			R( e, a, b, c, d, F3, K3, M(41) );
			R( d, e, a, b, c, F3, K3, M(42) );
			R( c, d, e, a, b, F3, K3, M(43) );
			R( b, c, d, e, a, F3, K3, M(44) );
			R( a, b, c, d, e, F3, K3, M(45) );
			R( e, a, b, c, d, F3, K3, M(46) );
			R( d, e, a, b, c, F3, K3, M(47) );
			R( c, d, e, a, b, F3, K3, M(48) );
			R( b, c, d, e, a, F3, K3, M(49) );
			R( a, b, c, d, e, F3, K3, M(50) );
			R( e, a, b, c, d, F3, K3, M(51) );
			R( d, e, a, b, c, F3, K3, M(52) );
			R( c, d, e, a, b, F3, K3, M(53) );
			R( b, c, d, e, a, F3, K3, M(54) );
			R( a, b, c, d, e, F3, K3, M(55) );
			R( e, a, b, c, d, F3, K3, M(56) );
			R( d, e, a, b, c, F3, K3, M(57) );
			R( c, d, e, a, b, F3, K3, M(58) );
			R( b, c, d, e, a, F3, K3, M(59) );
			R( a, b, c, d, e, F4, K4, M(60) );
			R( e, a, b, c, d, F4, K4, M(61) );
			R( d, e, a, b, c, F4, K4, M(62) );
			R( c, d, e, a, b, F4, K4, M(63) );
			R( b, c, d, e, a, F4, K4, M(64) );
			R( a, b, c, d, e, F4, K4, M(65) );
			R( e, a, b, c, d, F4, K4, M(66) );
			R( d, e, a, b, c, F4, K4, M(67) );
			R( c, d, e, a, b, F4, K4, M(68) );
			R( b, c, d, e, a, F4, K4, M(69) );
			R( a, b, c, d, e, F4, K4, M(70) );
			R( e, a, b, c, d, F4, K4, M(71) );
			R( d, e, a, b, c, F4, K4, M(72) );
			R( c, d, e, a, b, F4, K4, M(73) );
			R( b, c, d, e, a, F4, K4, M(74) );
			R( a, b, c, d, e, F4, K4, M(75) );
			R( e, a, b, c, d, F4, K4, M(76) );
			R( d, e, a, b, c, F4, K4, M(77) );
			R( c, d, e, a, b, F4, K4, M(78) );
			R( b, c, d, e, a, F4, K4, M(79) );
#undef K1
#undef K2
#undef K3
#undef K4
#undef F1
#undef F2
#undef F3
#undef F4
#undef M
#undef R
			state[0] += a;
			state[1] += b;
			state[2] += c;
			state[3] += d;
			state[4] += e;
		}

		void update(const unsigned char *input, unsigned int inputlen)
		{
			if ( remain )
			{
				unsigned int copy_len = 64 - remain;
				if ( inputlen < copy_len )
				{
					memcpy( buffer + remain, input, inputlen );
					remain += inputlen;
					return;
				}
				memcpy(buffer + remain, input, copy_len);
				transform( buffer );
				inputlen -= copy_len;
				input    += copy_len;
				if ( (low_count += 512) == 0 ) high_count++;
			}
			for ( ; inputlen >= 64; inputlen -= 64, input += 64)
			{
				transform( input );
				if ( (low_count += 512) == 0 ) high_count++;
			}
			if ( (remain = inputlen) > 0 ) memcpy ( buffer, input, remain );
		}
		void init()
		{
			state[0] = 0x67452301;
			state[1] = 0xefcdab89;
			state[2] = 0x98badcfe;
			state[3] = 0x10325476;
			state[4] = 0xc3d2e1f0;
			low_count = high_count = remain = 0;	
		}
	public:
		SHA1Hash() { init(); }
		SHA1Hash(Type type) : Security(type) { init(); }
		SHA1Hash(const SHA1Hash &rhs) : Security(rhs), low_count(rhs.low_count), high_count(rhs.high_count), remain(rhs.remain)
		{
			memcpy(state, rhs.state, sizeof(state));
			memcpy(buffer,rhs.buffer,sizeof(buffer));
		}
		Octets& Update(Octets &o)
		{
			update((const unsigned char *)o.begin(), o.size());
			return o;
		}
		Octets& Final(Octets &digest)
		{
			static unsigned char padding[64] = {
				0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			unsigned int l = low_count, h = high_count;
			if ( (l += remain * 8) < l ) h++;
			h = byteorder_32(h);
			l = byteorder_32(l);
			unsigned int index = remain  & 0x3f;
			unsigned int padlen = (index < 56) ? (56 - index) : (120 - index);
			update (padding, padlen);
			update ((const unsigned char *)&h, sizeof(h));
			update ((const unsigned char *)&l, sizeof(l));
			state[0] = byteorder_32(state[0]);
			state[1] = byteorder_32(state[1]);
			state[2] = byteorder_32(state[2]);
			state[3] = byteorder_32(state[3]);
			state[4] = byteorder_32(state[4]);
			return digest.replace( state, sizeof(state) );
		}

		static Octets Digest(const Octets &o)
		{
			Octets digest;
			SHA1Hash ctx;
			ctx.update((const unsigned char *)o.begin(), o.size());
			return ctx.Final(digest);
		}

	};
#endif
}

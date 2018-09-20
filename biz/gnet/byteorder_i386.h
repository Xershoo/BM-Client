#pragma once


namespace gnet
{
#if defined BYTE_ORDER_BIG_ENDIAN
#define byteorder_16(x)	(x)
#define byteorder_32(x)	(x)
#define byteorder_64(x)	(x)

#elif defined __GNUC__
	//no imp


#elif defined WIN32

	inline unsigned __int16 byteorder_16(unsigned __int16 x)
	{
		__asm ror x, 8
		return x;
	}

	inline unsigned __int32 byteorder_32(unsigned __int32 x)
	{
		__asm mov eax, x
		__asm bswap eax
		__asm mov x, eax
		return x;
	}


	inline unsigned __int64 byteorder_64(unsigned __int64 x)
	{
		union
		{
			unsigned __int64 i64;
			unsigned __int32 i32[2];
		}i, o;
		i.i64 = x;
		o.i32[0] = byteorder_32(i.i32[1]);
		o.i32[1] = byteorder_32(i.i32[0]);
		return o.i64;
	}

#endif
}
#include <stdafx.h>
#include "datautils.h"

#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))   
int hex2bin(char *str, char **hex)  
{  
	char *ptr;  
	int i, l = strlen(str);  
	if (l & 1)  
		return 0;  
	*hex = (char *)malloc(l/2);  
	ptr = *hex;  
	if (!ptr)  
		return 0;  
	for (i=0; i<l; i+=2)  
		*ptr++ = (HEX2BIN(str[i]) << 4) | HEX2BIN(str[i+1]);  
	return l/2;  
}  


char * put_byte( char *output, uint8_t nVal )  
{  
	output[0] = nVal;  
	return output+1;  
}  


char * put_be16(char *output, uint16_t nVal )  
{  
	output[1] = nVal & 0xff;  
	output[0] = nVal >> 8;  
	return output+2;  
}  

char * put_be24(char *output,uint32_t nVal )  
{  
	output[2] = nVal & 0xff;  
	output[1] = nVal >> 8;  
	output[0] = nVal >> 16;  
	return output+3;  
}  

char * put_be32(char *output, uint32_t nVal )  
{  
	output[3] = nVal & 0xff;  
	output[2] = nVal >> 8;  
	output[1] = nVal >> 16;  
	output[0] = nVal >> 24;  
	return output+4;  
}  

char *  put_be64( char *output, uint64_t nVal )  
{  
	output=put_be32( output, nVal >> 32 );  
	output=put_be32( output, nVal );  
	return output;  
}  

char * put_amf_string( char *c, const char *str )  
{  
	uint16_t len = strlen( str );  
	c=put_be16( c, len );  
	memcpy(c,str,len);  
	return c+len;  
}  

char * put_amf_double( char *c, double d )  
{  
	*c++ = AMF_NUMBER;  /* type: Number */  
	{  
		unsigned char *ci, *co;  
		ci = (unsigned char *)&d;  
		co = (unsigned char *)c;  
		co[0] = ci[7];  
		co[1] = ci[6];  
		co[2] = ci[5];  
		co[3] = ci[4];  
		co[4] = ci[3];  
		co[5] = ci[2];  
		co[6] = ci[1];  
		co[7] = ci[0];  
	}  
	return c+8;  
} 

char * put_amf_bool( char *c, bool b ) 
{ 
	*c++ = AMF_BOOLEAN; 
	{ 
		c[0] = b ? 1:0;
	} 
	return c+1; 
}

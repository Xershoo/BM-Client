#ifndef DATAUTILS_H
#define DATAUTILS_H
#include "rtmp_sys.h"
#include "log.h"
#include "amf.h"

//amf 格式封装
//十六进制到2制止数据
int hex2bin(char *str, char **hex);

//8字节拷贝到内存
char * put_byte( char *output, uint8_t nVal );

//16字节拷贝到内存
char * put_be16(char *output, uint16_t nVal );
//24字节拷贝到内存
char * put_be24(char *output,uint32_t nVal );

//32字节拷贝到内存
char * put_be32(char *output, uint32_t nVal );

//64字节拷贝到内存
char *  put_be64( char *output, uint64_t nVal );

//字符串拷贝到内存
char * put_amf_string( char *c, const char *str );

//浮点行数据开被盗内存
char * put_amf_double( char *c, double d );

//bool字符串拷贝到内存
char * put_amf_bool( char *c, bool b ) ;
#endif
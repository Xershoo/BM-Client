#ifndef DATAUTILS_H
#define DATAUTILS_H
#include "rtmp_sys.h"
#include "log.h"
#include "amf.h"

//amf ��ʽ��װ
//ʮ�����Ƶ�2��ֹ����
int hex2bin(char *str, char **hex);

//8�ֽڿ������ڴ�
char * put_byte( char *output, uint8_t nVal );

//16�ֽڿ������ڴ�
char * put_be16(char *output, uint16_t nVal );
//24�ֽڿ������ڴ�
char * put_be24(char *output,uint32_t nVal );

//32�ֽڿ������ڴ�
char * put_be32(char *output, uint32_t nVal );

//64�ֽڿ������ڴ�
char *  put_be64( char *output, uint64_t nVal );

//�ַ����������ڴ�
char * put_amf_string( char *c, const char *str );

//���������ݿ������ڴ�
char * put_amf_double( char *c, double d );

//bool�ַ����������ڴ�
char * put_amf_bool( char *c, bool b ) ;
#endif
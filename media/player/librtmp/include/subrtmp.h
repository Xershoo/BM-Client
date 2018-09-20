#ifndef  SUBRTMP_H
#define  SUBRTMP_H
#include "rtmp_sys.h"
#include "log.h"
#include "amf.h"
/************************************************************************/
/* rtmp��ز����ĸ��麯��                                                                     */
/************************************************************************/
#if !_WIN32
//˯�߶�����
#define Sleep(n)  usleep((n)*(1000))
#endif

#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))

//���绷����ʼ��
int InitSockets();

//���绷������ʼ��
void CleanupSockets();


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
char *  put_be64( char *output, t_uint64_t nVal );

//�ַ����������ڴ�
char * put_amf_string( char *c, const char *str );

//���������ݿ������ڴ�
char * put_amf_double( char *c, double d );

//ͼ���ʽYCbCrת��ΪBUR
void ConvertYCbCr2BGR(unsigned char *pYUV,unsigned char *pBGR,int iWidth,int iHeight);

//ͼ���ʽRGBת��ΪYUV
void ConvertRGB2YUV(int w,int h,unsigned char *bmp,unsigned char *yuv);

#endif

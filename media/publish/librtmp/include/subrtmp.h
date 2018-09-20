#ifndef  SUBRTMP_H
#define  SUBRTMP_H
#include "rtmp_sys.h"
#include "log.h"
#include "amf.h"
/************************************************************************/
/* rtmp相关操作的辅组函数                                                                     */
/************************************************************************/
#if !_WIN32
//睡眠多少秒
#define Sleep(n)  usleep((n)*(1000))
#endif

#define HEX2BIN(a)      (((a)&0x40)?((a)&0xf)+9:((a)&0xf))

//网络环境初始化
int InitSockets();

//网络环境反初始化
void CleanupSockets();


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
char *  put_be64( char *output, t_uint64_t nVal );

//字符串拷贝到内存
char * put_amf_string( char *c, const char *str );

//浮点行数据开被盗内存
char * put_amf_double( char *c, double d );

//图像格式YCbCr转换为BUR
void ConvertYCbCr2BGR(unsigned char *pYUV,unsigned char *pBGR,int iWidth,int iHeight);

//图像格式RGB转换为YUV
void ConvertRGB2YUV(int w,int h,unsigned char *bmp,unsigned char *yuv);

#endif

//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：Util.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.1
//	说明：公共常用函数定义头文件
//  接口：
//      
//      
//	修改记录：
//      
//**********************************************************************

#ifndef UTIL_H
#define UTIL_H

#include <QtCore\QString>
#include <string>
#include <vector>

namespace Util
{
    //计算字符串的校验值
    extern unsigned int crc32(unsigned char * buf, int len);

    //QString与char* ，std::string ,std::wstring的转换
    extern void AnsiToQString(const char* s,int length,QString& qs);
    extern void QStringToAnsi(const QString& str,char* s,int length);    
    extern void QStringToString(const QString& qstr,std::string& str);
    extern void QStringToWString(const QString& qstr,std::wstring& wstr);

    //Unicode与Ansi的转换
    extern void AnsiToUnicode(const char* s,int slength,wchar_t * u,int ulength);
    extern void UnicodeToAnsi(const wchar_t * u,int ulength,char* s,int slength);

    //字符串与16进制的转换
    extern int  StringToHex(char *string, unsigned char *cbuf, int len);
    extern void HexToString(char *ptr,unsigned char *buf,int len);

    //MD5
    QString Digest2String(const unsigned char *pDigest, int nDigest);
    bool    String2Digest(const unsigned char* hash_str, unsigned char *pDigest, int nLen = 16);
    bool	Char2Digest(const char* pChar,int nCharLen, char* pDigest);
    bool	Digest2Char(const char* pDigest, int nDigestLen, char* pChar);
    bool    CaleFileMd5(QString strFile, QString& strMd5);
    QString CaleBufferMd5(QString& strBuffer);

    //URL
    extern void URLEncode(const std::string& strSrc,std::string& strDst);
    extern void URLDecode(const std::string& strSrc,std::string& strDst);
    
    //UTF8

    //QString -> char*(含中文）
    bool QStringToChar(const QString &strSource, char *pszDes,int nSize);

    bool isAllNumber(const QString &text);
    
    bool IsExistDir(QString dir);

    template < class T >
    void releaseArray(std::vector< T >& v)
    {
        for (auto it = v.begin(); it != v.end(); it ++) 
        {
            if (NULL != *it) 
            {
                delete *it; 
                *it = NULL;
            }            
        }
        v.clear();
    }

	//获取磁盘信息
	extern QString getMaxFreeSpaceDisk();
	extern qint64 getDiskFreeSpace(const QString& strDisk);
};

#endif
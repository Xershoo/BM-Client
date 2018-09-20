//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�Util.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.1
//	˵�����������ú�������ͷ�ļ�
//  �ӿڣ�
//      
//      
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef UTIL_H
#define UTIL_H

#include <QtCore\QString>
#include <string>
#include <vector>

namespace Util
{
    //�����ַ�����У��ֵ
    extern unsigned int crc32(unsigned char * buf, int len);

    //QString��char* ��std::string ,std::wstring��ת��
    extern void AnsiToQString(const char* s,int length,QString& qs);
    extern void QStringToAnsi(const QString& str,char* s,int length);    
    extern void QStringToString(const QString& qstr,std::string& str);
    extern void QStringToWString(const QString& qstr,std::wstring& wstr);

    //Unicode��Ansi��ת��
    extern void AnsiToUnicode(const char* s,int slength,wchar_t * u,int ulength);
    extern void UnicodeToAnsi(const wchar_t * u,int ulength,char* s,int slength);

    //�ַ�����16���Ƶ�ת��
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

    //QString -> char*(�����ģ�
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

	//��ȡ������Ϣ
	extern QString getMaxFreeSpaceDisk();
	extern qint64 getDiskFreeSpace(const QString& strDisk);
};

#endif
//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：Util.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.1
//	说明：公共常用函数实现源码文件
//  接口：
//      
//      
//	修改记录：
//      
//**********************************************************************
#include "Util.h"
#include <QtCore\QString>
#include <QtCore\QTextCodec>
#include <stdio.h>
#include <string>
#include <QFile>
#include <QTextcodec>
#include <QDataStream>
#include <QDir>
#include "varname.h"
#include "md5.h"
#include "setdebugnew.h"

#include <Windows.h>
using namespace std;

namespace Util
{
    static unsigned int _CRC32[256] = {0};
    static char  _bInitCRC32 = 0;

    //初始化表
    static void init_table()
    {
        int	i,j;
        UINT  crc;
        for(i = 0;i < 256;i++)
        {
            crc = i;
            for(j = 0;j < 8;j++)
            {
                if(crc & 1)
                {
                    crc = (crc >> 1) ^ 0xEDB88320;
                }
                else
                {
                    crc = crc >> 1;
                }
            }
            _CRC32[i] = crc;
        }
    }

    //crc32实现函数
    unsigned int crc32(unsigned char * buf, int len)
    {
        UINT ret = 0xFFFFFFFF;
        int   i;
        if( !_bInitCRC32 )
        {
            init_table();
            _bInitCRC32 = 1;
        }
        for(i = 0; i < len;i++)
        {
            ret = _CRC32[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
        }
        ret = ~ret;

        return ret;
    }

    void AnsiToQString(const char* s,int length,QString& qs)
    {   
        if(NULL == s || NULL == s[0] || length <= 0 )
        {
            return ;
        }

		QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
        qs = QTextCodec::codecForLocale()->toUnicode(s,length);

        return ;
    }

	void Utf8ToQString(const char* s,int length,QString& qs)
	{   
		if(NULL == s || NULL == s[0] || length <= 0 )
		{
			return ;
		}

		QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
		qs = QTextCodec::codecForLocale()->toUnicode(s,length);

		return ;
	}

    void QStringToAnsi(const QString& str,char* s,int length)
    {
        if(str.isEmpty() || NULL == s || length <= 0)
        {
            return;
        }

		QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
        QByteArray sArray = QTextCodec::codecForLocale()->fromUnicode(str);
        length --;

        length = length > sArray.length() ? sArray.length() : length;

        strncpy(s,sArray.data(),length);

        return;
    }

    void AnsiToUnicode(const char* s,int slength,wchar_t * u,int ulength)
    {
        if( NULL == s || NULL == s[0] || slength <= 0||
            NULL == u || ulength <= 0)
        {
            return ;
        }

        QString qstr ;
        AnsiToQString(s,slength,qstr);

        if(qstr.length() <= 0)
        {
            return;
        }

        int       l = qstr.length() + 1;
        wchar_t * w = new wchar_t[l];

        memset(w,0,l * sizeof(wchar_t));

        qstr.toWCharArray(w);

        ulength = ulength > wcslen(w) ? wcslen(w) : ulength;
        wcsncpy(u,w,ulength);

        delete[] w;

        return;
    }

    void UnicodeToAnsi(const wchar_t * u,int ulength,char* s,int slength)
    {
        if( NULL == s || slength <= 0|| NULL == u || 
            NULL == u[0] || ulength <= 0 )
        {
            return ;
        }

        QString qstr = QString::fromWCharArray(u,ulength);
        if(qstr.length() <= 0)
        {
            return;
        }

        QStringToAnsi(qstr,s,slength);
    }

    void QStringToString(const QString& qstr,string& str)
    {
        if(qstr.isEmpty())
        {
            return;
        }

        int     l = qstr.length() * 2 + 1;
        char*   s = new char[l];

        memset(s,0,sizeof(char) * l);

        QStringToAnsi(qstr,s,l);

        str = string(s);
        delete[] s;

        return;
    }

    void QStringToWString(const QString& qstr,wstring& wstr)
    {
        if(qstr.isEmpty())
        {
            return;
        }

        int       l = qstr.length() + 1;
        wchar_t * w = new wchar_t[l];

        memset(w,0,l * sizeof(wchar_t));

        qstr.toWCharArray(w);

        wstr = wstring(w);

        delete[]w;

        return;
    }

    /**************************************************************************** 
    函数名称: StringToHex 
    函数功能: 字符串转换为十六进制 
    输入参数: string 字符串 cbuf 十六进制 len 字符串的长度。 
    输出参数: 无 
    *****************************************************************************/   
    int StringToHex(char *string, unsigned char *cbuf, int len)  
    {  
        BYTE high, low;  
        int idx, ii=0;  
        for (idx=0; idx<len; idx+=2)   
        {  
            high = string[idx];  
            low = string[idx+1];  

            if(high>='0' && high<='9')  
                high = high-'0';  
            else if(high>='A' && high<='F')  
                high = high - 'A' + 10;  
            else if(high>='a' && high<='f')  
                high = high - 'a' + 10;  
            else  
                return -1;  

            if(low>='0' && low<='9')  
                low = low-'0';  
            else if(low>='A' && low<='F')  
                low = low - 'A' + 10;  
            else if(low>='a' && low<='f')  
                low = low - 'a' + 10;  
            else  
                return -1;  

            cbuf[ii++] = high<<4 | low;  
        }  
        return 0;  
    }  

    /**************************************************************************** 
    函数名称: HexToString 
    函数功能: 十六进制转字符串 
    输入参数: ptr 字符串 buf 十六进制 len 十六进制字符串的长度。 
    输出参数: 无 
    *****************************************************************************/
    void HexToString(char *ptr,unsigned char *buf,int len)  
    {  
        for(int i = 0; i < len; i++)  
        {  
            sprintf(ptr, "%02x",buf[i]);  
            ptr += 2;  
        }  
    }


    void URLEncode(const string& strSrc,string& strDst)
    {
        QString ret;
        wchar_t uni_buf[16384];
        int uni_len = 16384;
        wcscpy(uni_buf, reinterpret_cast<const wchar_t *>(QString(strSrc.c_str()).utf16()));
        uni_len = wcslen(uni_buf);

        for (int i = 0; i < uni_len; i++)
        {
            unsigned char mb_buf[128] = {0};
            int mb_len = 128;
            
            QString strbuf = QString::fromWCharArray(&uni_buf[i], 1);
            char szbuf[8] = {0};
            QStringToChar(strbuf, szbuf,8);
            strncpy((char*)mb_buf, szbuf, sizeof(mb_buf)-1);
            mb_len = strlen((char*)mb_buf);
            if (mb_len > 1)
            {
                for (int j = 0; j < mb_len; j++)
                {
                    QString szTemp;
                    szTemp.sprintf("%%%2X", mb_buf[j]);
                    ret += szTemp;
                }
            }
            else
            {
                ret += (char*)mb_buf;
            }
        }
        strDst = ret.toStdString();
    }
    
    void URLDecode(const string& strSrc,string& strDst)
    {

    }


    QString Digest2String(const unsigned char *pDigest, int nDigest)
    {
        QString ret = "";
        if (NULL == pDigest)
        {
            return ret;
        }
        char szTemp[8] = {0};
        for (int i = 0; i < nDigest; i++)
        {
            _snprintf(szTemp, 8, "%02X", pDigest[i]);
            ret += szTemp;
        }
        return ret;
    }

    bool    String2Digest(const unsigned char* hash_str, unsigned char *pDigest, int nLen)
    {
        if(hash_str == NULL || pDigest == NULL)
            return false;
        unsigned char *h = pDigest;
        char szTemp[16] = {0};

        for(int i = 0; i < nLen; i++)
        {
            strncpy(szTemp, (char*)hash_str + i * 2, 2);
            int ch;
            sscanf(szTemp, "%02X", &ch);
            h[i] = ch;
        }
        return true;
    }

    bool	Char2Digest(const char* pChar,int nCharLen, char* pDigest)
    {
        if(pChar == NULL || nCharLen <= 0 || pDigest == NULL)
            return false;
        for (int i = 0; i < nCharLen; i++)
        {
            *pDigest = (*pChar >> 4);
            *pDigest++;
            *pDigest = *pChar & 0xF;
            pDigest++;		
            pChar++;
        }
        return true;
    }

    bool	Digest2Char(const char* pDigest, int nDigestLen, char* pChar)
    {
        if(pDigest == NULL || nDigestLen <= 0 || pChar == NULL)
            return false;
        for (int i = 0; i < nDigestLen; i = i + 2)
        {
            *pChar = *pDigest << 4;
            pDigest++;	
            *pChar = *pChar | *pDigest;
            pDigest++;
            pChar++;
        }
        return true;
    }

    bool  CaleFileMd5(QString strFile, QString& strMd5)
    {
        if (strFile.isEmpty())
        {
            return false;
        }
		
		char fileName[MAX_PATH]={0};
		QStringToAnsi(strFile,fileName,MAX_PATH);

		if(NULL==fileName||NULL==fileName[0]){
			return false;
		}

		FILE* file_p = fopen(fileName,"rb");
		if(NULL==file_p){
			return false;
		}

		CMD5 md5;
		md5.Init();
		unsigned char md5_output[32]={0};
		unsigned char file_buf[1024]={0};
		unsigned int  buf_size = 0;

		while((buf_size=fread(file_buf,sizeof(char),sizeof(file_buf),file_p))){
			md5.Update((unsigned char *)file_buf,buf_size);
		}

		fclose(file_p);
		file_p = NULL;

		memcpy(md5_output,md5.Finalize(),16); 
		strMd5 = Digest2String(md5_output, 16);

		return true;
		/*
        QFile file(strFile);
        
        if (file.exists() && file.open(QIODevice::ReadOnly))
        {
            long long nFileSize = file.size();
            if (0 == nFileSize || 0xFFFFFFFF == nFileSize)
            {
                file.close();
                return false;
            }
            const int nBufferLen = 1024;
            unsigned char buf[nBufferLen] = {0};
            DWORD dwCaluSize = 0;
            CMD5 md5;
            DWORD dwRead = 0;
			QDataStream fileStream(&file);

            while (dwCaluSize < nFileSize)
            {
                dwRead = (DWORD)fileStream.readRawData((char*)buf, nBufferLen);
                if (-1 == dwRead)
                {
                    file.close();
                    return false;
                }
                md5.Update(buf, nBufferLen);
                dwCaluSize += dwRead;
            }
            file.close();

            unsigned char hashmd5[16] = {0};
            memcpy(hashmd5, md5.Finalize(), 16);

            strMd5 = Digest2String(hashmd5, 16);
            return true;
        }
        return false;
		*/
		
    }

    QString CaleBufferMd5(QString& strBuffer)
    {
        CMD5 md5;
        md5.Init();
        md5.Update((uchar*)strBuffer.toLatin1().data(), strBuffer.length()*sizeof(char));
        unsigned char hashmd5[16] = {0};
        memcpy(hashmd5, md5.Finalize(), 16);

        return QString(Digest2String(hashmd5, 16));
    }

    bool QStringToChar(const QString &strSource, char *pszRes,int nSize)
    {
        if (strSource.isEmpty())
        {
            return false;
        }

        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QByteArray source = codec->fromUnicode(strSource);        
		memset(pszRes,NULL,nSize*sizeof(char));
        
		strcpy_s(pszRes,nSize-1,source.data());

        return true;
    }

    bool isAllNumber(const QString &text)
    {
        for (int i = 0; i < text.size(); i++)
        {
            if (text.at(i) >= '0' && text.at(i) <= '9')
                continue;
            else
                return false;
        }
        return true;
    }

    bool IsExistDir(const QString& strdir)
    {
        if (strdir.isEmpty())
        {
            return false;
        }

        QDir dir;
        if (!dir.exists(strdir))
        {
            QString str = strdir.left(strdir.lastIndexOf("\\"));
            IsExistDir(str);
        }
        dir.mkdir(strdir);
        return true;
    }

	bool isFileExists(const QString& fileName){
		if(fileName.isEmpty()||fileName.isNull()){
			return false;
		}

		QFile file(fileName);
		bool  br = file.open(QIODevice::ReadOnly);
		if(br){
			br = file.size() > 0 ? true : false;
			file.close();
		}

		return br;
	}

	QString getMaxFreeSpaceDisk()
	{
		QFileInfoList drvList=QDir::drives();
		
		QString diskOne;
		QString diskTwo;		
		qint64  fistFree = 0;
		qint64  secdFree = 0;

		for(int i=0; i<drvList.count();i++)
		{
			QFileInfo drvInfo = drvList.at(i);
			QString  drvPath = drvInfo.filePath();

			qint64 freeSize = getDiskFreeSpace(drvPath);
			if(freeSize > fistFree)
			{
				diskOne = drvPath;
				continue;
			}

			if(freeSize > secdFree)
			{
				diskTwo = drvPath;
				continue;
			}
		}

		if (diskOne.indexOf(QString("c:\\"),0,Qt::CaseInsensitive) == -1 &&
			diskOne.indexOf(QString("c:/"),0,Qt::CaseInsensitive) == -1)
		{
			return diskOne;
		}

        if(diskTwo.isEmpty())
        {
            return diskOne;
        }

		return diskTwo;
	}

	qint64 getDiskFreeSpace(const QString& strDisk)
	{
		if(strDisk.isEmpty())
		{
			return 0;
		}

		LPCWSTR lpcwstrDriver=(LPCWSTR)strDisk.utf16(); 
		ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes; 
		if( !::GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) ) 
		{ 
			return 0;  
		}

		return (quint64) liTotalFreeBytes.QuadPart;  
	}

	bool IsXPHigher()
	{
		bool bIsXPHigher = false;

		// 取得当前系统版本
		OSVERSIONINFO VersionInfo = {0};
		VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&VersionInfo);

		// 判断并返回
		bIsXPHigher = VersionInfo.dwMajorVersion > 5;
		
		return bIsXPHigher;
	}

	void PrintTrace(char* format,...)
	{
		char szText[1024]={0};
		va_list args;
		va_start(args, format);
		vsprintf_s(szText, sizeof(szText)-1, format, args);
		va_end(args);

		OutputDebugStringA(szText);
		OutputDebugStringA("\n");
	}

	bool GetDebugPrivilege()
	{
		HANDLE hToken = NULL;
		LUID sedebugnameValue;
		TOKEN_PRIVILEGES tkp;

		if (!::OpenProcessToken(::GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			return false;
		}

		if (!::LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ))
		{
			CloseHandle( hToken );
			return false;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = sedebugnameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!::AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
		{
			CloseHandle( hToken );
			return false;
		}

		return true;
	}
};


//**********************************************************************
//	Copyright （c） 2015,All rights reserved.
//	文件名称：courseware.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#include "courseware.h"
#include <QString>
#include <string.h>
#include <fstream>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QDir>
#include "Env.h"
#include "util.h"
#include "./session/classsession.h"
#include "Config.h"

int GetCoursewareFileType(LPCWSTR pszFile)
{
    if (NULL == pszFile || NULL == pszFile[0])
    {
        return COURSEWARE_UNKNOWN;
    }

    LPWSTR pszExt = (LPWSTR)(wcsrchr(pszFile, L'.'));
    if (NULL == pszExt){
        return COURSEWARE_UNKNOWN;
	}

    pszExt++;
   if (NULL == pszExt)
   {
       return COURSEWARE_UNKNOWN;
   }
   if( _wcsicmp(pszExt,L"ppt") == 0 ||
       _wcsicmp(pszExt,L"pptx") == 0 )
   {
       return COURSEWARE_PPT;
   }

   if ( _wcsicmp(pszExt,L"pdf") == 0)
   {
       return COURSEWARE_PDF;
   }

   if (_wcsicmp(pszExt,L"bmp") == 0 ||
       _wcsicmp(pszExt,L"jpg") == 0 ||
       _wcsicmp(pszExt,L"jpeg") == 0 ||
       _wcsicmp(pszExt,L"png") == 0 ||
       _wcsicmp(pszExt,L"gif") == 0 )
   {
       return COURSEWARE_IMG;
   }

   if (_wcsicmp(pszExt,L"avi") == 0 ||
       _wcsicmp(pszExt,L"wmv") == 0 ||
       _wcsicmp(pszExt,L"flv") == 0 ||      
       _wcsicmp(pszExt,L"mp4") == 0 ||
       _wcsicmp(pszExt,L"rmvb") == 0 )
   {
       return COURSEWARE_VIDEO;
   }

   if ( _wcsicmp(pszExt,L"swf") == 0)
   {
	   return COURSEWARE_FLASH;
   }

   if ( _wcsicmp(pszExt,L"wav") == 0 ||
	   _wcsicmp(pszExt,L"mp3") == 0)
   {
       return COURSEWARE_AUDIO;
   }

   if (_wcsicmp(pszExt,L"txt") == 0 )
   {
       return COURSEWARE_TEXT;
   }

   if (_wcsicmp(pszExt,L"xls") == 0 ||
       _wcsicmp(pszExt,L"xlsx") == 0  )
   {
       return COURSEWARE_EXCLE;
   }

   if (_wcsicmp(pszExt,L"doc") == 0 ||
       _wcsicmp(pszExt,L"docx") == 0  )
   {
       return COURSEWARE_DOC;
   }

   return COURSEWARE_UNKNOWN;
}

void GetCoursewareUploadFileName(LPWSTR pszLocalFile,LPWSTR pszRemoteFile,int nLength)
{
    if (NULL == pszLocalFile || NULL == pszLocalFile[0])
    {
        return;
    }

    LPWSTR pszName = (LPWSTR)(wcsrchr(pszLocalFile, L'\\'));
    if (NULL == pszName || NULL == pszName[1])
    {
        return;
    }
    pszName++;
    wcscpy_s(pszRemoteFile, nLength, pszName);
}

void GetCoursewareDownloadFileName(LPWSTR pszRemoteFile,LPWSTR pszLocalFile,int nLength)
{
    if (NULL == pszRemoteFile || NULL == pszRemoteFile[0])
    {
        return;
    }

    wchar_t szFileName[MAX_PATH] = {0};
    QString strPath = QString("%1Cursewave\\").arg(Env::GetUSerConfigPath(ClassSeeion::GetInst()->_nUserId));
    wcscpy(pszLocalFile, wstring((wchar_t*)(strPath).unicode()).data());

    QFileInfo fi(strPath);
    if (!fi.isDir())
    {
        QDir dir;
        dir.mkpath(strPath);
    }

    LPWSTR pszFileName = wcschr(pszRemoteFile,L'/');
    if(NULL == pszFileName)
    {
        pszFileName = wcschr(pszRemoteFile,L'/');
        if(NULL == pszFileName)
        {
            pszFileName = pszRemoteFile;
        }
        else
        {
            pszFileName++;
        }
    }
    else
    {
        pszFileName ++;
    }

    LPWSTR pszFileNameReal = wcschr(pszFileName,L':');
    if(NULL != pszFileNameReal)
    {
        wcsncpy_s(szFileName,pszFileName,pszFileNameReal - pszFileName);
        pszFileName = szFileName;
    }

    wcscat_s(pszLocalFile,nLength,pszFileName);
}

void GetCoursewareTransFile(LPWSTR pszMD5, LPWSTR pszCopyFile,LPWSTR pszType, int nLength)
{
    if (NULL == pszMD5 || NULL == pszMD5[0])
    {
        return;
    }

    QString strPath = QString("%1Cursewave\\").arg(Env::GetUSerConfigPath(ClassSeeion::GetInst()->_nUserId));
    wcscpy(pszCopyFile, wstring((wchar_t*)(strPath).unicode()).data());

    QFileInfo fi(strPath);
    if (!fi.isDir())
    {
        QDir dir;
        dir.mkpath(strPath);
    }
    wcscat_s(pszCopyFile,nLength,pszMD5);
    wcscat_s(pszCopyFile,nLength,pszType);
}

void GetCoursewareTransFile(LPWSTR pszMD5,LPWSTR pszCopyFile,int nLength)
{
    if (NULL == pszMD5)
    {
        return;
    }

    QString strPath = QString("%1Cursewave\\").arg(Env::GetUSerConfigPath(ClassSeeion::GetInst()->_nUserId));
    wcscpy(pszCopyFile, wstring((wchar_t*)(strPath).unicode()).data());
    QFileInfo fi(strPath);
    if (!fi.isDir())
    {
        QDir dir;
        dir.mkpath(strPath);
    }
    wcscat_s(pszCopyFile,nLength,pszMD5);
    wcscat_s(pszCopyFile,nLength,L".pdf");
}

void GetCoursewareTransFileMedia(LPWSTR pszMD5, LPWSTR pszCopyFile, LPWSTR pszType, int nLength)
{
    if (NULL == pszMD5)
    {
        return;
    }

    QString strPath = QString("%1Cursewave\\").arg(Env::GetUSerConfigPath(ClassSeeion::GetInst()->_nUserId));
    wcscpy(pszCopyFile, wstring((wchar_t*)(strPath).unicode()).data());
    
    QFileInfo fi(strPath);
    if (!fi.isDir())
    {
        QDir dir;
        dir.mkpath(strPath);
    }
    wcscat_s(pszCopyFile,nLength,pszMD5);
	wcscat_s(pszCopyFile,nLength,pszType);
// 
//     if (0 == wcscmp(pszType, L".mp3") || 0 == wcscmp(pszType, L".MP3") || 
// 		0 == wcscmp(pszType, L".mp4") || 0 == wcscmp(pszType, L".MP4") )
// 	{
// 		wcscat_s(pszCopyFile,nLength,pszType);
// 	}
// 	else
// 	{
// 		wcscat_s(pszCopyFile,nLength,L".mp4");
// 	}
//	
}

long long GetCoursewareFileSize(LPCWSTR pszFile)
{
    if (NULL == pszFile || NULL == pszFile[0])
    {
        return 0;
    }
    QString strPath = QString::fromWCharArray(pszFile);

    QFileInfo fi(strPath);
    if (!fi.exists())
    {
        return 0;
    }

    return fi.size();//返回文件大小 byte
}

//xiewb 2017.08.22 增加文件内容的MD5判断
bool IsExistFile(LPCWSTR pszFilePath,LPCWSTR pszMD5/*=NULL*/)
{
    if (NULL == pszFilePath || NULL == pszFilePath[0])
    {
        return false;
    }

    QString strPath = QString::fromWCharArray(pszFilePath);
    QFileInfo fi(strPath);
	if(!fi.exists())
	{
		return false;
	}

	if(NULL==pszMD5||NULL==pszMD5[0])
	{
		return true;
	}

	QString strMD5;
	if (Util::CaleFileMd5(strPath,strMD5) && 
		strMD5.compare(QString::fromWCharArray(pszMD5),Qt::CaseInsensitive) == 0)
	{
		return true;
	}
	
	if(!QFile::rename(strPath,strPath+".bak")){
		QFile::remove(strPath);
	};

    return false;
}

bool GetUploadToken(char *pszToken, __int64 nUserId)
{
    if (0 != CUploadTokenMgr::GetInstance()->GetToken(pszToken, nUserId))
    {
        return false;
    }
    return true;
}

bool CopyFileToMD5Path(LPCWSTR pszMD5Path, LPCWSTR pszSourcePath)
{
    if (NULL == pszMD5Path || NULL == pszSourcePath)
    {
        return false;
    }

	QString srcFile = QString::fromWCharArray(pszSourcePath);
	QString md5File = QString::fromWCharArray(pszMD5Path);

	if(QFile::exists(md5File))
	{
		QFile::remove(md5File);
	}

	return QFile::copy(srcFile,md5File);
/*
    FILE *in_file, *out_file;
    char data[512];
    size_t bytes_in, bytes_out;
    long len = 0;
    QString strSource = QString::fromWCharArray(pszSourcePath);
    QString strMd5 = QString::fromWCharArray(pszMD5Path);

    char szSource[MAX_PATH] = {0};
    char szMd5[MAX_PATH] = {0};
    Util::QStringToChar(strSource, szSource,MAX_PATH);
    Util::QStringToChar(strMd5, szMd5,MAX_PATH);

    if ( (in_file = fopen(szSource, "rb")) == NULL )
    {
        return false;
    }
    if ( (out_file = fopen(szMd5, "wb")) == NULL )
    {
        return false;
    }

    while ( (bytes_in = fread(data, 1, 512, in_file)) > 0 )
    {
        bytes_out = fwrite(data, 1, bytes_in, out_file);
        if ( bytes_in != bytes_out )
        {
            return false;
        }
        len += bytes_out;
    }

    fclose(in_file);
    fclose(out_file);
    return true;
*/
}

bool GetHttpDownUrl(LPWSTR pszUrl, int nLength)
{
    string strUrl = Config::getConfig()->m_urlCoursewareDown;
    Util::AnsiToUnicode(strUrl.c_str(),strUrl.length(),pszUrl,nLength);

    return true;
}

bool GetHttpUploadUrl(LPWSTR pszUrl, int nLength)
{
    int nCourseID = ClassSeeion::GetInst()->_nCourseId;
    int nClassRoomID = ClassSeeion::GetInst()->_nClassRoomId;
    QString strUrl;

    strUrl=QString("%1%2/%3").arg(QString(Config::getConfig()->m_urlCoursewareUpload.c_str())).arg(nCourseID).arg(nClassRoomID);
    
    wcsncpy_s(pszUrl,nLength,wstring((wchar_t*)(strUrl).unicode()).data(), wcslen(wstring((wchar_t*)(strUrl).unicode()).data()));	

    return true;
}

bool DeleteCoursewareFile(LPCWSTR pszFilePath)
{
    if (NULL == pszFilePath || NULL == pszFilePath[0])
    {
        return false;
    }

    QFile file;
    QString strFile = QString::fromWCharArray(pszFilePath);
    return file.remove(strFile);
}

bool DeleteFloderContent(LPCWSTR pszFloder, bool isDelSelf)
{
    if (NULL == pszFloder || NULL == pszFloder[0])
    {
        return false;
    }
    QString strFloder = QString::fromWCharArray(pszFloder);
    QDir dir(strFloder);
    QFileInfoList filelist;
    QFileInfo curFile;
    if (!dir.exists())
    {
        return false;
    }
    filelist = dir.entryInfoList(QDir::Dirs|QDir::Files
                                 |QDir::Readable|QDir::Writable
                                 |QDir::Hidden|QDir::NoDotAndDotDot,
                                 QDir::Name);
    while (filelist.size() > 0)
    {
        int nforNum = filelist.size();
        for (int i = nforNum-1; i >= 0; i--)
        {
            curFile = filelist[i];
            if (curFile.isFile())
            {
                QFile fileTemp(curFile.filePath());
                fileTemp.remove();
                filelist.removeAt(i);
            }
            if (curFile.isDir())
            {
                QDir dirTemp(curFile.filePath());
                QFileInfoList fileList1=dirTemp.entryInfoList(QDir::Dirs|QDir::Files
                                                             |QDir::Readable|QDir::Writable
                                                             |QDir::Hidden|QDir::NoDotAndDotDot
                                                             ,QDir::Name);
                if(fileList1.size()==0)//下层没有文件或文件夹
                {
                    dirTemp.rmdir(".");
                    filelist.removeAt(i);
                }
                else//下层有文件夹或文件
                {
                    for(int j=0;j<fileList1.size();j++)
                    {
                        if(!(filelist.contains(fileList1[j])))
                            filelist.append(fileList1[j]);
                    }
                }
            }
        }
    }
    if (isDelSelf)
    {
        dir.mkdir(".");
    }
    return true;
}

bool RenameFile(LPCWSTR pszFileName, LPCWSTR pszNewFileName)
{
    if (NULL == pszNewFileName || NULL == pszFileName)
    {
        return false;
    }
    QFile file;
    QString strFile = QString::fromWCharArray(pszFileName);
    QString strNewFile = QString::fromWCharArray(pszNewFileName);
    return file.rename(strFile, strNewFile);
}

bool GetErrorInfo(LPWSTR pszErr, int nError)
{
    QString strError = "";
    switch (nError)
    {
    case COURSEWARE_ERR_OK:
        {

        }
        break;

    case COURSEWARE_ERR_DOWNFAILED:
        {
            strError = QString(QObject::tr("Failed to download the courseware!"));
        }
        break;

    case COURSEWARE_ERR_UPLOADFAILED:
        {
            strError = QString(QObject::tr("Failed to upload the courseware!"));
        }
        break;

    case COURSEWARE_ERR_REPEAT:
        {
            strError = QString(QObject::tr("File already exists!"));
        }
        break;

    case COURSEWARE_ERR_PATH:
        {
            strError = QString(QObject::tr("File path error!"));
        }
        break;

    case COURSEWARE_ERR_URL:
        {
            strError = QString(QObject::tr("URL error!"));
        }
        break;

    case COURSEWARE_ERR_TOKEN:
        {
            strError = QString(QObject::tr("Token error!"));
        }
        break;

    case COURSEWARE_ERR_TRANSMEDIO:
        {
            strError = QString(QObject::tr("Audio and video transcoding error!"));
        }
        break;

    case COURSEWARE_ERR_TYPE:
        {
            strError = QString(QObject::tr("File format error!"));
        }
        break;
    }
    wcscpy(pszErr, wstring((wchar_t*)(strError).unicode()).data());
    return true;
}
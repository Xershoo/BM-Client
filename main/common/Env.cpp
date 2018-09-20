#include "Env.h"
#include <QTemporaryFile>
#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include <QKeySequence>
#include "setdebugnew.h"

QString Env::strExePath = "";
QString Env::m_strAppFileName = "";
QString Env::strTempPath = "";
QString Env::strGtPath = "";
QString Env::strGtVersionPath = "";
int Env::nLanguageId_ = 0;

BYTE Env::chMainVer = 0;
BYTE Env::chSubVer = 0;
BYTE Env::chExVer = 0;
DWORD Env::chBuildNO = 0;
BYTE Env::szMac[] = "\0";

bool Env::m_init = false;

Env::Env(void)
{
    InitExePath();
}

Env::~Env(void)
{

}

QString Env::GetUSerConfigPath(UINT64 u64UserID)
{
	QString strTmp = QString("%1\\Users\\%2\\").arg(strGtPath).arg(u64UserID);
    QDir dir;
    if (dir.mkpath(strTmp))
        return strTmp;
    return "";
}

QString Env::GetGTVersion()
{
    // 版本号是静态变量，只取一次即可
    
    TCHAR szFileName[MAX_PATH] = { 0};
    ::GetModuleFileName(NULL, szFileName, MAX_PATH - 1);
    DWORD dwLen = ::GetFileVersionInfoSize(szFileName, NULL);
    if(dwLen > 0)
    {
        BYTE* pBuffer = new BYTE[dwLen];
        if(::GetFileVersionInfo(szFileName, NULL, dwLen, pBuffer))
        {
            VS_FIXEDFILEINFO* lpFileInfo = NULL;
            UINT nLen = 0;
            if(::VerQueryValue(pBuffer, TEXT("\\"), (void**)&lpFileInfo, &nLen))
            {
                DWORD dwNumber = 65536;
                chMainVer = lpFileInfo->dwFileVersionMS / dwNumber;
                chSubVer = lpFileInfo->dwFileVersionMS % dwNumber;
                chExVer = lpFileInfo->dwFileVersionLS / dwNumber;
                chBuildNO = lpFileInfo->dwFileVersionLS % dwNumber;
                //strGTVersion.Format(_T("%d.%d.%d.%d"), chMainVer,chSubVer, chExVer,chBuildNO);
                strGtVersionPath = QString("%1.%2.%3.%4").arg(chMainVer).arg(chSubVer).arg(chExVer).arg(chBuildNO);
            }
        }
        delete []pBuffer;
    }

    return strGtVersionPath;
}

QString Env::GetUserPicCachePath(UINT64 u64UserID)
{
    QString strPicCachePath = GetUSerConfigPath(u64UserID) + "Images\\";
    QDir dir(strPicCachePath);
    // 判断路径是否存在
    if (!dir.exists())
    {
        // 创建目录
	    QDir dir;
        if (dir.mkpath(strPicCachePath))
            return strPicCachePath;
    }else
		return strPicCachePath;
    return "";
}

QString Env::GetAllPicCachePath()
{
	QString strTmp = QString("%1Users\\Images\\").arg(strGtPath);
    QDir dir;
    if (dir.mkpath(strTmp))
    {
        return strTmp;
    }
    return "";
}

void Env::InitExePath()
{
    if (m_init)
        return;
    strExePath = qApp->applicationDirPath();
	
    int pos = strExePath.lastIndexOf("/");
    strGtPath = strExePath.left(pos);//QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    strGtVersionPath = strExePath.left(pos);
    strGtVersionPath = strGtVersionPath = strGtVersionPath.right(strGtVersionPath.length() - strGtVersionPath.lastIndexOf("/") - 1);	
}

QString Env::GetTempPath()
{
	if(strTempPath.length() == 0)
	{
		strTempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
	}
	return strTempPath;
}

QString Env::GetTempFilePathName(QString ext)
{
    QTemporaryFile file;
    file.open();
    ext = file.fileName() + "." + ext;
    file.close();
	return ext;
}

QString Env::GetKeyName(Qt::Key key)
{
    return QKeySequence(key).toString();
}

QString Env::getExePath()
{
    if (!m_init)
        InitExePath();
    return strExePath;
}

QString Env::currentThemeResPath()
{
    return QString(":/res/res/image/default/");
}
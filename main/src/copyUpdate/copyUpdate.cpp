//拷贝更新文件

#include "copyUpdate.h"
#include <QString>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include "common/varname.h"

CCopyUpdateFile::CCopyUpdateFile()
{
    m_srcFolder = "";
    m_objFolder = "";
}

CCopyUpdateFile::~CCopyUpdateFile()
{

}

void CCopyUpdateFile::StartCopy(const QString srcFolder)
{
    m_srcFolder = srcFolder;
    wchar_t wszFloder[MAX_PATH] = {0};
    wchar_t wszObjFolder[MAX_PATH] = {0};
    QString folder;
    wcscpy(wszFloder, std::wstring((wchar_t*)(srcFolder).unicode()).data());
    wchar_t *pszFolder = wcsrchr(wszFloder, L'/');
    wcsncpy(wszObjFolder, wszFloder, wcslen(wszFloder) - wcslen(pszFolder));
    m_objFolder = QString("%1\\%2").arg(QString::fromWCharArray(wszObjFolder)).arg(UPDATE_FOLDER);
    Util::IsExistDir(m_objFolder);

    startthread(); 
}

bool CCopyUpdateFile::CopyUpdateFile(const QString srcFile, const QString objFile)
{
    FILE *in_file, *out_file;
    char data[512];
    size_t bytes_in, bytes_out;
    long len = 0;

    char szSource[MAX_PATH] = {0};
    char szMd5[MAX_PATH] = {0};
    Util::QStringToChar(srcFile, szSource,MAX_PATH);
    Util::QStringToChar(objFile, szMd5,MAX_PATH);

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
    return true;
}

void CCopyUpdateFile::run()
{
    QString fileName, srcFileName, objFileName;

    //extractzip.dll
    fileName = QString("\\extractzip.dll");
    srcFileName = m_srcFolder + fileName;
    objFileName = m_objFolder + fileName;
    CopyUpdateFile(srcFileName, objFileName);

    //xctrl.dll
    fileName = QString("\\xctrl.dll");
    srcFileName = m_srcFolder + fileName;
    objFileName = m_objFolder + fileName;
    CopyUpdateFile(srcFileName, objFileName);

    //xlayout.dll
    fileName = QString("\\xlayout.dll");
    srcFileName = m_srcFolder + fileName;
    objFileName = m_objFolder + fileName;
    CopyUpdateFile(srcFileName, objFileName);

    //update.exe
    fileName = QString("\\update.exe");
    srcFileName = m_srcFolder + fileName;
    objFileName = m_objFolder + fileName;
    CopyUpdateFile(srcFileName, objFileName);

    createShortCutToDeskTop();
    createShortCutToStartMenue();
    stopthread();
}

bool CCopyUpdateFile::createShortCutToDeskTop()
{
    QString filePath = m_objFolder + QString("/update.exe");
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }

    QString deskTopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString desktopLink = deskTopPath + QString("/") + QString(QObject::tr("productName")) + QString(".lnk");

    QFile::link(filePath,desktopLink);

    file.close();

    return true;
}

bool CCopyUpdateFile::createShortCutToStartMenue()
{
    QString filePath = m_objFolder + QString("/update.exe");    
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }

    QString startMenuePath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QString startMenueLink = startMenuePath + QString("/") + QString(QObject::tr("productName")) 
        + QString("/") + QString(QObject::tr("product")) + QString(".lnk");
    QString linkFile = startMenuePath + QString("/") + QString(QObject::tr("productName"));

    QDir dir;
    if (!dir.exists(linkFile))
    {
        dir.mkdir(linkFile);
    }

    QFile::link(filePath,startMenueLink);
    file.close();

    return true;
}
#ifndef ENV
#define ENV

#include <QDir>
#include <QString>
#include <QStringList>

#include <windows.h>
#include "psapi.h"
#include"stdio.h"
#include <tlhelp32.h>
#include <QStandardPaths>

#include <QByteArray>
#include <QFile>
#include <QCryptographicHash>

namespace ENV{

//define

//创建文件夹
static inline void createFolder(const QString &folderPath);
//查找当前是否有该进程运行
static inline bool processIsRun(const QString &proName);
//创建桌面快捷方式
static inline bool createShortCutToDeskTop(const QString &file);
//创建开始菜单快捷方式
static inline bool createShortCutToStartMenue(const QString &file, const QString exeName, const QString uninstallFile);
//得到文件md5
static inline QByteArray getFileMd5(QFile &file);
static inline QByteArray getFileMd5(const QString &filePath);
//对比文件md5
static inline bool cmpFileMd5(QFile &lhs,QFile &rhs);
static inline bool cmpFileMd5(const QString &lhs,const QString &rhs);


//implementation

void createFolder(const QString &folderPath){
    QDir dir;
    bool exist = dir.exists(folderPath);
    if(!exist){
        QStringList list = folderPath.split('/');
        QString filePath = "";
        int len = 0;
        if(QString(list.at(list.length()-1)).contains('.'))
            len = list.length()-1;
        else
            len = list.length();
        for(int i = 0;i < len;++i){
            filePath = filePath + list[i] + '/';
            exist = dir.exists(filePath);
            if(!exist)
                dir.mkdir(filePath);
        }
    }
}

static inline bool processIsRun(const QString &proName)
{
    int count = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {
        count += 0;
    }

    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    while(bMore)
    {
        if(proName.compare(QString::fromWCharArray(pe32.szExeFile),Qt::CaseInsensitive) == 0)
        {
            ++count;
        }

        bMore = ::Process32Next(hProcessSnap, &pe32);
    }

    if(count == 1)
    {
        return true;
    }
    else if(count > 1)
    {
        return false;
    }

    return false;
}

bool createShortCutToDeskTop(const QString &file, QString exeName){
    QString deskTopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFileInfo info(file);
    return QFile::link(file,deskTopPath+"/"+exeName+".lnk");
}

bool createShortCutToStartMenue(const QString &file, const QString exeName, const QString uninstallFile){
    QString startMenuePath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);    
    QString nameProduct = QString("/") + QString(QObject::tr("Product"));
    QString linkFile = startMenuePath + nameProduct;
    QDir dir;
    if (!dir.exists(linkFile))
    {
        dir.mkdir(linkFile);
    }

    QFile::link(file,linkFile+"/"+exeName+".lnk");
    QFile::link(uninstallFile,linkFile+"/Uninstall.lnk");

    return true;
}


QByteArray getFileMd5(QFile &file)
{
    if(!file.exists())
        return "";
    file.open(QIODevice::ReadOnly);
    QByteArray result = QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5);
    file.close();
    return result;
}

QByteArray getFileMd5(const QString &filePath){
    return getFileMd5(QFile(filePath));
}

bool cmpFileMd5(QFile &lhs,QFile &rhs)
{
    if(!lhs.exists() || !rhs.exists())
        return false;
    return (getFileMd5(lhs) == getFileMd5(rhs));
}

bool cmpFileMd5(const QString &lhs,const QString &rhs){
    return cmpFileMd5(QFile(lhs),QFile(rhs));
}

}


#endif // ENV


//拷贝更新文件

#pragma once

#include "common/Util.h"
#include "customThread.h"

#define  UPDATE_FOLDER      ("update")

class CCopyUpdateFile : public CustomThread
{
public:
    CCopyUpdateFile();

    ~CCopyUpdateFile();

public:

    void StartCopy(const QString strFolder);

protected:
    virtual void run();

    bool  CopyUpdateFile(const QString srcFile, const QString objFile);

    bool createShortCutToDeskTop();

    bool createShortCutToStartMenue();

protected:
    QString     m_objFolder;
    QString     m_srcFolder;
};

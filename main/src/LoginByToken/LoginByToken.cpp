
#include "LoginByToken.h"
#include <QStringList>
#include "./session/classsession.h"
#include "setdebugnew.h"

bool IsStartByWeb(char* lpstrCmdLine,WebStartParam& startParam)
{
    if (NULL == lpstrCmdLine || NULL == lpstrCmdLine[0])
    {
        return false;
    }

    QString commandLine = QString("%1").arg(lpstrCmdLine);
    if (commandLine == QString("updater_start"))
    {
        return false;
    }

    //class8:///uid=XXX&token=XXX&classid=XXX&courseid=XXX&acc=XXX
    QStringList strlist = commandLine.split("&");

    WebStartParam webparam;
    char token[1024] = {0};
    char acc[255] = {0};
    if (strlist.size() == 5)
    {
        startParam.uid = strlist.at(0).section('=', 1).toInt();
        startParam.token = strlist.at(1).section('=', 1);
        startParam.classid = strlist.at(2).section('=', 1).toInt();
        startParam.courseid = strlist.at(3).section('=', 1).toInt();
        startParam.acc = strlist.at(4).section('=', 1);
    }
    return true;
}

void SetLoginByWebInfo(WebStartParam webinfo)
{
    ClassSeeion::GetInst()->m_loginBytokenUid = true;
    ClassSeeion::GetInst()->m_classID = webinfo.classid;
    ClassSeeion::GetInst()->m_courseID = webinfo.courseid;
    ClassSeeion::GetInst()->_nUserId = webinfo.uid;
}
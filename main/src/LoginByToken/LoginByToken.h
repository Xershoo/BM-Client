
#pragma once

#include <QString>
#include "varname.h"

typedef struct tagWebStartParam
{
    __int64		 uid;
    __int64		 classid;
    __int64		 courseid;
    QString		 acc;
    QString		 token;
}WebStartParam;

bool IsStartByWeb(char* lpstrCmdLine,WebStartParam& startParam);
void SetLoginByWebInfo(WebStartParam);
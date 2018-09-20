//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：runtime.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：获取程序运行的时间
//	修改记录：    
//**********************************************************************

#include "runtime.h"
#include <QtCore\QTime>

namespace run_time
{
    QTime * g_runTime = NULL;
    
    void start()
    {
        g_runTime = new QTime;
        if(NULL != g_runTime)
        {
            g_runTime->start();
        }
    }

    int getRunTime()
    {
        if(NULL == g_runTime)
        {
            return 0;
        }

        return g_runTime->elapsed();
    }

    void stop()
    {
        if(NULL == g_runTime)
        {
            return;
        }

        delete g_runTime;
        g_runTime = NULL;
    }
};
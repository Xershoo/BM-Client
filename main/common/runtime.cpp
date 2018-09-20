//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�runtime.cpp
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.23
//	˵������ȡ�������е�ʱ��
//	�޸ļ�¼��    
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
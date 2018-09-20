//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：runtime.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：获取程序运行的时间，start和stop接口分别在程序的main的入口和出口调用
//	修改记录：    
//**********************************************************************

#ifndef RUN_TIMER_H
#define RUN_TIMER_H

namespace run_time
{
    void start();
    int  getRunTime();
    void stop();
};

#endif
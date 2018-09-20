//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：BizInterface.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：biz库的初始化接口和反初始化接口的定义
//	修改记录：    
//**********************************************************************

#ifndef BIZ_INTERFACE_H
#define BIZ_INTERFACE_H

#include "BizCallBack.h"
#include "biz/interface/interface.h"
#include "biz/interface/IUserInfoData.h"
#include "biz/interface/IClassRoomInfo.h"
#include "biz/interface/IClassRoomInfoContainer.h"
using namespace biz;
extern void initBizLibray();
extern void unitBizLibray();
extern CBizCallBack* getBizCallBack();

#endif
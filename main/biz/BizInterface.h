//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�BizInterface.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.11.23
//	˵����biz��ĳ�ʼ���ӿںͷ���ʼ���ӿڵĶ���
//	�޸ļ�¼��    
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